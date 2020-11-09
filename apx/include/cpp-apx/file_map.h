/*****************************************************************************
* \file      file_map.h
* \author    Conny Gustafsson
* \date      2020-11-05
* \brief     File container
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

#include <list>
#include "cpp-apx/file.h"
#include "cpp-apx/types.h"

namespace apx
{
   class FileMap
   {
   public:
      FileMap() = delete;
      FileMap(bool is_remote) : m_is_remote_map{ is_remote }{}
      ~FileMap();
      bool is_remote() const { return m_is_remote_map; }
      File* create_file(rmf::FileInfo const& file_info);
      File* find_by_address(std::uint32_t address) const;
      std::list<apx::File*>& list() { return m_list; }
   protected:
      std::list<apx::File*>::const_iterator auto_assign_address(File* file);
      std::list<apx::File*>::const_iterator find_last_element_of_type(FileType file_type);
      std::list<apx::File*>::const_iterator find_next_available_position(File const* file);
      bool insert_item(File* file, std::list<apx::File*>::const_iterator iterator_left);


      std::list<apx::File*> m_list;
      bool m_is_remote_map;

   };
}
