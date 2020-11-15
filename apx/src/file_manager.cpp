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

#include <iostream>
#include "cpp-apx/file_manager.h"

namespace apx
{
   void FileManager::start()
   {
   }
   void FileManager::stop()
   {
   }
   void FileManager::connected()
   {
      publish_local_files();
   }
   void FileManager::disconnected()
   {
   }
   error_t FileManager::create_local_file(rmf::FileInfo const& file_info)
   {
      File* file = m_shared.create_local_file(file_info);
      if (file == nullptr)
      {
         return APX_FILE_CREATE_ERROR;
      }
      return APX_NO_ERROR;
   }

   error_t FileManager::message_received(uint8_t const* msg_data, std::size_t msg_len)
   {
      return error_t();
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
      std::cout << local_file_list.size() << std::endl;
      for (auto& file_info : local_file_list)
      {
         m_worker.publish_local_file(file_info);
      }
   }
}