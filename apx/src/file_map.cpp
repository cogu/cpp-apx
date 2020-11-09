/*****************************************************************************
* \file      file_map.cpp
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

#include <algorithm>
#include <cassert>
#include "cpp-apx/file_map.h"

namespace apx
{
   FileMap::~FileMap()
   {
      for (auto& it : m_list)
      {
         delete it;
      }
   }

   File* FileMap::create_file(rmf::FileInfo const& file_info)
   {
      auto* file = new File(file_info);
      std::list<apx::File*>::const_iterator iterator_left = m_list.cend();
      if (file->get_address_without_flags() == rmf::INVALID_ADDRESS)
      {
         iterator_left = auto_assign_address(file);
         if (iterator_left == m_list.cend())
         {
            iterator_left = find_next_available_position(file);
         }
      }
      else
      {
         iterator_left = find_next_available_position(file);
      }
      if (m_is_remote_map)
      {
         std::uint32_t tmp = file->get_address();
         file->set_address(tmp | rmf::REMOTE_ADDRESS_BIT);
      }
      auto result = insert_item(file, iterator_left);
      if (!result)
      {
         delete file;
         file = nullptr;
      }
      return file;
   }

   File* FileMap::find_by_address(std::uint32_t address) const
   {
      (void)address;
      return nullptr;
   }

   std::list<apx::File*>::const_iterator FileMap::auto_assign_address(File* file)
   {
      std::uint32_t start_address = 0u;
      std::uint32_t alignment = 0u;
      auto const file_type = file->get_apx_file_type();
      switch (file_type)
      {
      case FileType::Definition:
         start_address = DEFINITION_ADDRESS_START;
         alignment = DEFINITION_ADDRESS_ALIGNMENT;
         break;
      case FileType::ProvidePortData:
      case FileType::RequirePortData:
         alignment = PORT_DATA_ADDRESS_ALIGNMENT;
         break;
      case FileType::ProvidePortCount:
      case FileType::RequirePortCount:
         start_address = PORT_COUNT_ADDRESS_START;
         alignment = PORT_COUNT_ADDRESS_ALIGNMENT;
         break;
      default:
         start_address = USER_DEFINED_ADDRESS_START;
         alignment = USER_DEFINED_ADDRESS_ALIGNMENT;
      }
      auto const iterator_left = find_last_element_of_type(file_type);
      if (iterator_left == m_list.end())
      {
         file->set_address(start_address);
      }
      else
      {
         assert(alignment != 0);
         std::uint32_t const end_address = (*iterator_left)->get_end_address_without_flags();
         std::uint32_t address = start_address;
         while (address < end_address)
         {
            address += alignment;
         }
         file->set_address(address);
      }
      return iterator_left;
   }

   std::list<apx::File*>::const_iterator FileMap::find_last_element_of_type(FileType file_type)
   {
      if (m_list.empty())
      {
         return m_list.cend();
      }
      auto next = m_list.cbegin();
      auto candidate = m_list.cend();
      //Find first candidate
      while (next != m_list.cend())
      {
         if ((*next)->get_apx_file_type() == file_type)
         {
            candidate = next++;
            break;
         }
         next++;
      }
      if (candidate != m_list.cend())
      {
         //Find last candidate
         while (next != m_list.cend())
         {
            if ((*next)->get_apx_file_type() == file_type)
            {
               candidate = next++;
            }
            else
            {
               break;
            }
         }
      }
      return candidate;
   }

   std::list<apx::File*>::const_iterator FileMap::find_next_available_position(File const* file)
   {
      if (m_list.empty())
      {
         return m_list.end();
      }
      std::list<apx::File*>::const_iterator iterator_next = m_list.cbegin();
      std::list<apx::File*>::const_iterator iterator_prev = m_list.cend();
      std::uint32_t const address = file->get_address();
      while (iterator_next != m_list.cend())
      {
         if ((*iterator_next)->get_address() < address)
         {
            iterator_prev = iterator_next++;
         }
         else
         {
            break;
         }
      }
      return iterator_prev;
   }

   //Inserts file between iterator_left and iterator_right (which is the next item in list)
   bool FileMap::insert_item(File* file, std::list<apx::File*>::const_iterator iterator_left)
   {
      if (m_list.empty())
      {
         m_list.push_back(file);
      }
      else
      {
         auto const file_start_end_address = file->get_address_without_flags();
         auto const file_end_address = file->get_end_address_without_flags(); //First byte after valid address
         std::list<apx::File*>::const_iterator iterator_right = m_list.cbegin();
         if (iterator_left != m_list.cend())
         {
            assert((*iterator_left)->get_end_address_without_flags() <= file_start_end_address);
            iterator_right = std::next(iterator_left);
         }
         if (iterator_right == m_list.cend())
         {
            m_list.push_back(file);
         }
         else
         {
            File const* right_file = *iterator_right;
            if ((file_end_address > right_file->get_address_without_flags()) ||
               (file_end_address > rmf::CMD_AREA_START_ADDRESS))
            {
               return false; //This file would overlap with another file (or with the command area)
            }
            m_list.insert(iterator_right, file);
         }
      }
      return true;
   }
}