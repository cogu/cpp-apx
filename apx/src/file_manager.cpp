/*****************************************************************************
* \file      file_manager.cpp
* \author    Conny Gustafsson
* \date      2020-11-10
* \brief     APX File Manager
*
* Copyright (c) 2020 Conny Gustafsson
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
******************************************************************************/

#include <cassert>
#include "cpp-apx/pack.h"
#include "cpp-apx/file_manager.h"

namespace apx
{
   void FileManager::start()
   {
#ifndef UNIT_TEST
      m_worker.start();
#endif
   }
   void FileManager::stop()
   {
#ifndef UNIT_TEST
      m_worker.stop();
#endif
   }
   void FileManager::connected()
   {
      publish_local_files();
   }
   void FileManager::disconnected()
   {
      //TODO: close_local_files();
   }

   File* FileManager::create_local_file(rmf::FileInfo const& file_info)
   {
      auto* file = m_shared.create_local_file(file_info);
      if (file != nullptr)
      {
         file->set_file_manager(this);
      }
      return file;
   }

   error_t FileManager::message_received(std::uint8_t const* msg_data, std::size_t msg_len)
   {
      std::uint32_t address{ rmf::INVALID_ADDRESS };
      bool more_bit{ false };
      auto const header_size = rmf::address_decode(msg_data, msg_data + msg_len, address, more_bit);
      if (header_size > 0)
      {
         assert(msg_len >= header_size);
         auto const result = m_receiver.write(address, msg_data + header_size, msg_len - header_size, more_bit);
         if (result.error != APX_NO_ERROR)
         {
            return result.error;
         }
         else if (result.is_complete)
         {
            return process_message(result.address, result.data, result.size);
         }
         else
         {
            //Wait for more data to arrive
         }
      }
      else
      {
         return APX_INVALID_MSG_ERROR;
      }
      return APX_NO_ERROR;
   }

   error_t FileManager::send_local_const_data(std::uint32_t address, std::uint8_t const* data, std::size_t size)
   {
      auto* file = m_shared.find_file_by_address(address);
      if (file == nullptr)
      {
         return APX_FILE_NOT_FOUND_ERROR;
      }
      if (!file->is_open())
      {
         return APX_FILE_NOT_OPEN_ERROR;
      }
      m_worker.prepare_send_local_const_data(address, data, static_cast<std::uint32_t>(size));
      return APX_NO_ERROR;
   }

   error_t FileManager::send_local_data(std::uint32_t address, std::uint8_t* data, std::size_t size)
   {
      auto* file = m_shared.find_file_by_address(address);
      if (file == nullptr)
      {
         return APX_FILE_NOT_FOUND_ERROR;
      }
      if (!file->is_open())
      {
         return APX_FILE_NOT_OPEN_ERROR;
      }
      m_worker.prepare_send_local_data(address, data, static_cast<std::uint32_t>(size));
      return APX_NO_ERROR;
   }

   error_t FileManager::send_open_file_request(std::uint32_t address)
   {
      m_worker.prepare_send_open_file_request(address);
      return APX_NO_ERROR;
   }

#ifdef UNIT_TEST
   bool FileManager::run()
   {
      return m_worker.run();
   }
#endif

   void FileManager::publish_local_files()
   {
      std::vector<rmf::FileInfo*> local_file_list;
      m_shared.copy_local_file_info(local_file_list);
      for (auto& file_info : local_file_list)
      {
         m_worker.prepare_publish_local_file(file_info);
      }
   }

   error_t FileManager::process_message(std::uint32_t address, std::uint8_t const* data, std::size_t size)
   {
      if (address == rmf::CMD_AREA_START_ADDRESS)
      {
         return process_command_message(data, size);
      }
      else if (address < rmf::CMD_AREA_START_ADDRESS)
      {
         return process_file_write_message(address, data, size);
      }
      return APX_INVALID_ADDRESS_ERROR;
   }

   error_t FileManager::process_command_message(std::uint8_t const* data, std::size_t size)
   {
      if (size < sizeof(std::uint32_t))
      {
         return APX_INVALID_MSG_ERROR;
      }
      error_t retval = APX_NO_ERROR;
      auto const cmd_type = apx::unpackLE<std::uint32_t>(data);
      std::size_t const cmd_size = size - sizeof(std::uint32_t);
      auto const* next = data + sizeof(std::uint32_t);
      rmf::FileInfo file_info;
      std::size_t decoded_size;
      switch (cmd_type)
      {
      case rmf::CMD_PUBLISH_FILE_MSG:
         decoded_size = rmf::decode_publish_file_cmd(data, size, file_info);
         if (decoded_size > 0u)
         {
            retval = process_remote_file_published(file_info);
         }
         else
         {
            retval = APX_INVALID_MSG_ERROR;
         }
         break;
      case rmf::CMD_REVOKE_FILE_MSG:
         break;
      case rmf::CMD_OPEN_FILE_MSG:
         if (cmd_size == rmf::FILE_OPEN_CMD_SIZE)
         {
            std::uint32_t const address = apx::unpackLE<std::uint32_t>(next);
            retval = process_open_file_request(address);
         }
         else
         {
            retval = APX_INVALID_MSG_ERROR;
         }
         break;
      case rmf::CMD_CLOSE_FILE_MSG:
         if (cmd_size == rmf::FILE_CLOSE_CMD_SIZE)
         {
            std::uint32_t const address = apx::unpackLE<std::uint32_t>(next);
            retval = process_close_file_request(address);
         }
         else
         {
            retval = APX_INVALID_MSG_ERROR;
         }
         break;
      default:
         retval = APX_UNSUPPORTED_ERROR;
      }
      return retval;
   }

   error_t FileManager::process_file_write_message(std::uint32_t address, std::uint8_t const* data, std::size_t size)
   {
      auto* file = m_shared.find_file_by_address(address | rmf::HIGH_ADDR_BIT);
      if (file == nullptr)
      {
         return APX_INVALID_WRITE_ERROR;
      }
      if (!file->is_open())
      {
         //Ignore writes on closed files
         return APX_NO_ERROR;
      }
      if (m_shared.connection() == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      std::uint32_t const start_address = file->get_address_without_flags();
      assert(start_address <= address);
      std::uint32_t const offset = address - start_address;
      return m_shared.connection()->remote_file_write_notification(file, offset, data, size);
   }

   error_t FileManager::process_open_file_request(std::uint32_t start_address)
   {
      auto* file = m_shared.find_file_by_address(start_address);
      if (file == nullptr)
      {
         return APX_FILE_NOT_FOUND_ERROR;
      }
      file->open();
      file->open_notify();
      return APX_NO_ERROR;
   }

   error_t FileManager::process_close_file_request(std::uint32_t start_address)
   {
      (void)start_address;
      return APX_NOT_IMPLEMENTED_ERROR;
   }

   error_t FileManager::process_remote_file_published(rmf::FileInfo const& file_info)
   {
      auto *file = m_shared.create_remote_file(file_info);
      if (file == nullptr)
      {
         return APX_FILE_CREATE_ERROR;
      }
      return m_shared.connection()->remote_file_published_notification(file);
   }
}