/*****************************************************************************
* \file      file_manager_shared.cpp
* \author    Conny Gustafsson
* \date      2020-11-10
* \brief     Shared objects used by file manager sub-components
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
#include "cpp-apx/file_manager_shared.h"

namespace apx
{
    File* FileManagerShared::create_local_file(rmf::FileInfo const& file_info)
   {
      std::lock_guard lock(m_mutex);
      return m_local_file_map.create_file(file_info);
   }

   File* FileManagerShared::create_remote_file(rmf::FileInfo const& file_info)
   {
      std::lock_guard lock(m_mutex);
      return m_remote_file_map.create_file(file_info);
   }

   File* FileManagerShared::find_local_file_by_name(char const* name)
   {
      std::lock_guard lock(m_mutex);
      return m_local_file_map.find_by_name(name);
   }

   File* FileManagerShared::find_local_file_by_name(std::string const& name)
   {
      std::lock_guard lock(m_mutex);
      return m_local_file_map.find_by_name(name);
   }

   File* FileManagerShared::find_remote_file_by_name(char const* name)
   {
      std::lock_guard lock(m_mutex);
      return m_remote_file_map.find_by_name(name);
   }

   File* FileManagerShared::find_remote_file_by_name(std::string const& name)
   {
      std::lock_guard lock(m_mutex);
      return m_remote_file_map.find_by_name(name);
   }

   File* FileManagerShared::find_file_by_address(std::uint32_t address)
   {
      std::lock_guard lock(m_mutex);
      return (address & rmf::REMOTE_ADDRESS_BIT)? m_remote_file_map.find_by_address(address) : m_local_file_map.find_by_address(address);
   }

   void FileManagerShared::connected()
   {
      std::lock_guard lock(m_mutex);
      m_is_connected = true;
   }

   void FileManagerShared::disconnected()
   {
      std::lock_guard lock(m_mutex);
      m_is_connected = false;
   }

   bool FileManagerShared::is_connected()
   {
      std::lock_guard lock(m_mutex);
      return m_is_connected;
   }

   void FileManagerShared::copy_local_file_info(std::vector<rmf::FileInfo*>& dest)
   {
      std::lock_guard lock(m_mutex);
      for (auto& file : m_local_file_map.list())
      {
         dest.push_back(new rmf::FileInfo(file->get_file_info()));
      }
   }


}