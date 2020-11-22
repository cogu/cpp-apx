/*****************************************************************************
* \file      file_manager_worker.h
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
#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "cpp-apx/types.h"
#include "cpp-apx/error.h"
#include "cpp-apx/file_info.h"
#include "cpp-apx/command.h"
#include "cpp-apx/file_manager_shared.h"

namespace apx
{
   class FileManagerWorker
   {
   public:
      FileManagerWorker() = delete;
      FileManagerWorker(FileManagerShared& shared) :m_shared{ shared } {}
      void prepare_publish_local_file(rmf::FileInfo* file_info);
      void prepare_send_local_const_data(std::uint32_t address, std::uint8_t const* data, std::uint32_t size);
      void prepare_send_local_data(std::uint32_t address, std::uint8_t* data, std::uint32_t size);
      void prepare_send_open_file_request(std::uint32_t address);

#ifdef UNIT_TEST
      bool run();
      std::size_t num_pending_commands() const { return m_queue.size(); }
#else
      void start();
      void stop();
#endif

   protected:
      std::queue<apx::Command> m_queue;
      std::condition_variable m_cond;
      std::mutex m_mutex;
      std::thread m_worker_thread;
      FileManagerShared& m_shared;

      bool process_single_command(apx::Command const& cmd);
      error_t run_publish_local_file(rmf::FileInfo* file);
      error_t run_send_local_const_data(std::uint32_t address, std::uint8_t const* data, std::uint32_t size);
      error_t run_send_local_data(std::uint32_t address, std::uint8_t* data, std::uint32_t size);
      error_t run_open_remote_file(std::uint32_t address);
      void worker_main();

   };
}