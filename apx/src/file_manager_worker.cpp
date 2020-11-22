/*****************************************************************************
* \file      file_manager_worker.cpp
* \author    Conny Gustafsson
* \date      2020-11-11
* \brief     An active object that executes commands from a command queue
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
#include <array>
#include <memory>
#if APX_DEBUG_ENABLE
#include <iostream>
#endif
#include "cpp-apx/file_manager_worker.h"

namespace apx
{
   void FileManagerWorker::prepare_publish_local_file(rmf::FileInfo* file_info)
   {
      std::scoped_lock lock{ m_mutex } ;
      Command cmd{CmdType::PublishLocalFile, 0u, 0u, reinterpret_cast<void*>(file_info), nullptr };
#if APX_DEBUG_ENABLE
      std::cout << "Preparing to publish local files" << std::endl;
#endif
      m_queue.push(cmd);
#ifndef UNIT_TEST
      m_cond.notify_one();
#endif
   }

   void FileManagerWorker::prepare_send_local_const_data(std::uint32_t address, std::uint8_t const* data, std::uint32_t size)
   {
      std::scoped_lock lock{ m_mutex };
      Command cmd{ CmdType::SendLocalConstData, address, size, reinterpret_cast<void*>(const_cast<std::uint8_t*>(data)), nullptr };
      m_queue.push(cmd);
#ifndef UNIT_TEST
      m_cond.notify_one();
#endif
   }

   void FileManagerWorker::prepare_send_local_data(std::uint32_t address, std::uint8_t* data, std::uint32_t size)
   {
      std::scoped_lock lock{ m_mutex };
      Command cmd{ CmdType::SendLocalConstData, address, size, reinterpret_cast<void*>(const_cast<std::uint8_t*>(data)), nullptr };
      m_queue.push(cmd);
#ifndef UNIT_TEST
      m_cond.notify_one();
#endif
   }

   void FileManagerWorker::prepare_send_open_file_request(std::uint32_t address)
   {
      std::scoped_lock lock{ m_mutex };
      Command cmd{ CmdType::OpenRemoteFile, address, 0u, (void*) nullptr, nullptr };
      m_queue.push(cmd);
#ifndef UNIT_TEST
      m_cond.notify_one();
#endif
   }

#ifdef UNIT_TEST
   bool FileManagerWorker::run()
   {
      auto* connection = m_shared.connection();
      if (connection != nullptr)
      {
         connection->transmit_begin();
      }
      while (m_queue.size() > 0u)
      {
         auto const& cmd = m_queue.front();
         auto result = process_single_command(cmd);
         m_queue.pop();
         if (!result)
         {
            if (connection != nullptr)
            {
               connection->transmit_end();
            }
            return result;
         }
      }
      if (connection != nullptr)
      {
         connection->transmit_end();
      }
      return true;
   }
#else
   void FileManagerWorker::start()
   {
      m_worker_thread = std::thread([this] {worker_main(); });
   }

   void FileManagerWorker::stop()
   {
      //TODO: Trigger worker exit event
      m_worker_thread.join();
   }

#endif

   bool FileManagerWorker::process_single_command(apx::Command const& cmd)
   {
      error_t result;
#if APX_DEBUG_ENABLE
      std::cout << "Processing command" << std::endl;
#endif
      switch (cmd.cmd_type)
      {
      case CmdType::PublishLocalFile:
         result = run_publish_local_file(reinterpret_cast<rmf::FileInfo*>(cmd.data3.ptr));
         break;
      case CmdType::OpenRemoteFile:
         result = run_open_remote_file(cmd.data1);
         break;
      case CmdType::SendLocalConstData:
         result = run_send_local_const_data(cmd.data1, reinterpret_cast<std::uint8_t const*>(cmd.data3.ptr), cmd.data2);
         break;
      case CmdType::SendLocalData:
         result = run_send_local_data(cmd.data1, reinterpret_cast<std::uint8_t*>(cmd.data3.ptr), cmd.data2);
         break;
      default:
         return false;
      }
      return true;
   }

   error_t FileManagerWorker::run_publish_local_file(rmf::FileInfo* file_info)
   {
      std::array<std::uint8_t, rmf::FILE_INFO_HEADER_SIZE + rmf::FILE_NAME_MAX_SIZE + 1> buffer; //add 1 byte for null-terminator
      std::unique_ptr<rmf::FileInfo> ptr{ file_info }; //automatically dispose object at end of scope
      std::size_t const encoded_size = rmf::encode_publish_file_cmd(buffer.data(), buffer.size(), file_info);
      if (encoded_size == 0u)
      {
         return APX_BUFFER_TOO_SMALL_ERROR;
      }
      auto* connection = m_shared.connection();
      if (connection != nullptr)
      {
         std::int32_t bytes_available{ 0 };
         auto rc = connection->transmit_data_message(rmf::CMD_AREA_START_ADDRESS, false, buffer.data(), static_cast<std::int32_t>(encoded_size), bytes_available);
         if (rc < 0)
         {
            return APX_TRANSMIT_ERROR;
         }
      }
      else
      {
         return APX_NOT_CONNECTED_ERROR;
      }
      return APX_NO_ERROR;
   }

   error_t FileManagerWorker::run_send_local_const_data(std::uint32_t address, std::uint8_t const* data, std::uint32_t size)
   {
      auto* connection = m_shared.connection();
      if (connection != nullptr)
      {
         std::int32_t bytes_available{ 0 };
         auto rc = connection->transmit_data_message(address, false, data, static_cast<std::int32_t>(size), bytes_available);
         if (rc < 0)
         {
            return APX_TRANSMIT_ERROR;
         }
      }
      else
      {
         return APX_NOT_CONNECTED_ERROR;
      }
      return APX_NO_ERROR;
   }

   error_t FileManagerWorker::run_send_local_data(std::uint32_t address, std::uint8_t* data, std::uint32_t size)
   {
      std::unique_ptr<std::uint8_t[]> ptr{ data }; //automatically dispose data on return
      auto* connection = m_shared.connection();
      if (connection != nullptr)
      {
         std::int32_t bytes_available{ 0 };
         auto rc = connection->transmit_data_message(address, false, ptr.get(), static_cast<std::int32_t>(size), bytes_available);
         if (rc < 0)
         {
            return APX_TRANSMIT_ERROR;
         }
      }
      else
      {
         return APX_NOT_CONNECTED_ERROR;
      }
      return APX_NO_ERROR;
   }

   error_t FileManagerWorker::run_open_remote_file(std::uint32_t address)
   {
      std::array<std::uint8_t, rmf::CMD_TYPE_SIZE + rmf::FILE_OPEN_CMD_SIZE> buffer; //add 1 byte for null-terminator
      std::size_t const encoded_size = rmf::encode_open_file_cmd(buffer.data(), buffer.size(), address);
      if (encoded_size == 0u)
      {
         return APX_BUFFER_TOO_SMALL_ERROR;
      }
      auto* connection = m_shared.connection();
      if (connection != nullptr)
      {
         std::int32_t bytes_available{ 0 };
         auto rc = connection->transmit_data_message(rmf::CMD_AREA_START_ADDRESS, false, buffer.data(), static_cast<std::int32_t>(encoded_size), bytes_available);
         if (rc < 0)
         {
            return APX_TRANSMIT_ERROR;
         }
      }
      else
      {
         return APX_NOT_CONNECTED_ERROR;
      }
      return APX_NO_ERROR;
   }

   void FileManagerWorker::worker_main()
   {
      std::unique_lock lock{ m_mutex };
      for (;;)
      {
         m_cond.wait(lock, [this] {return !m_queue.empty(); });
         auto* connection = m_shared.connection();
         if (connection != nullptr)
         {
            connection->transmit_begin();
         }
         while (m_queue.size() > 0u)
         {
            auto const& cmd = m_queue.front();
            bool success = process_single_command(cmd);
            m_queue.pop();
            if (!success)
            {
               if (connection != nullptr)
               {
                  connection->transmit_end();
               }
               lock.unlock();
               return;
            }
         }
         if (connection != nullptr)
         {
            connection->transmit_end();
         }
      }
   }
}