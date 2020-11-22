/*****************************************************************************
* \file      file_manager_shared.h
* \author    Conny Gustafsson
* \date      2020-11-05
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
#pragma once

#include <mutex>
#include <vector>
#include "cpp-apx/file_map.h"
#include "cpp-apx/connection_interface.h"

namespace apx
{
   class FileManagerShared
   {
   public:
      FileManagerShared() :m_local_file_map{ false }, m_remote_file_map{ true }, m_parent_connection{ nullptr }, m_is_connected{ false }{}
      FileManagerShared(ConnectionInterface* parent_connection) :m_local_file_map{ false }, m_remote_file_map{ true },
         m_parent_connection{ parent_connection }, m_is_connected{ false }{}

      File* create_local_file(rmf::FileInfo const& file_info);
      File* create_remote_file(rmf::FileInfo const& file_info);
      File* find_local_file_by_name(char const* name);
      File* find_local_file_by_name(std::string const& name);
      File* find_remote_file_by_name(char const* name);
      File* find_remote_file_by_name(std::string const& name);
      File* find_file_by_address(std::uint32_t address);
      void connected();
      void disconnected();
      bool is_connected();
      void copy_local_file_info(std::vector<rmf::FileInfo*>& dest);
      ConnectionInterface* connection() const { return m_parent_connection; }

   protected:
      FileMap m_local_file_map;
      FileMap m_remote_file_map;
      std::mutex m_mutex;
      ConnectionInterface* m_parent_connection;
      bool m_is_connected;
   };
}