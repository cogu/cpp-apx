/*****************************************************************************
* \file      file_manager.h
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
#pragma once

#include "cpp-apx/file_manager_shared.h"
#include "cpp-apx/file_manager_worker.h"
#include "cpp-apx/file_manager_receiver.h"
#include "cpp-apx/error.h"

namespace apx
{
   class FileManager
   {
   public:
      FileManager() = delete;
      FileManager(ConnectionInterface* parent_connection) : m_shared(parent_connection),
         m_worker{ m_shared }{}
      void start();
      void stop();
      void connected();
      void disconnected();
      File* create_local_file(rmf::FileInfo const& file_info);
      File* find_file_by_address(std::uint32_t address) { return m_shared.find_file_by_address(address); }
      File* find_local_file_by_name(char const* name) { return m_shared.find_local_file_by_name(name); }
      error_t message_received(uint8_t const* msg_data, std::size_t msg_len);
      error_t send_local_const_data(std::uint32_t address, std::uint8_t const* data, std::size_t size);
      error_t send_local_data(std::uint32_t address, std::uint8_t* data, std::size_t size);

#ifdef UNIT_TEST
      bool run();
      std::size_t num_pending_commands() { return 0u; }
#endif
   protected:
      void publish_local_files();
      error_t process_message(std::uint32_t address, std::uint8_t const* data, std::size_t size);
      error_t process_command_message(std::uint8_t const* data, std::size_t size);
      error_t process_file_write_message(std::uint32_t address, std::uint8_t const* data, std::size_t size);
      error_t process_open_file_request(std::uint32_t start_address);
      error_t process_close_file_request(std::uint32_t start_address);

      FileManagerReceiver m_receiver;
      FileManagerShared m_shared;
      FileManagerWorker m_worker;
   };

}