/*****************************************************************************
* \file      file.cpp
* \author    Conny Gustafsson
* \date      2020-11-06
* \brief     APX file class
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
#include "cpp-apx/file.h"

namespace apx
{
   static std::string const s_unknown_ext;
   static std::string const s_definition_ext{ ".apx" };
   static std::string const s_provide_port_data_ext{ ".out" };
   static std::string const s_require_port_data_ext{ ".in" };
   static std::string const s_provide_port_count_ext{ ".cout" };
   static std::string const s_require_port_count_ext{ ".cin" };

   std::string const& file_type_to_extension(FileType file_type)
   {
      switch (file_type)
      {
      case FileType::Definition:
         return s_definition_ext;
      case FileType::ProvidePortData:
         return s_provide_port_data_ext;
      case FileType::RequirePortData:
         return s_require_port_data_ext;
      case FileType::ProvidePortCount:
         return s_provide_port_count_ext;
      case FileType::RequirePortCount:
         return s_require_port_count_ext;
      }
      return s_unknown_ext;
   }

   File::File(rmf::FileInfo const& file_info):m_file_info{file_info}
   {
      if (m_file_info.name_ends_with(s_definition_ext))
      {
         m_apx_file_type = FileType::Definition;
      }
      else if (m_file_info.name_ends_with(s_provide_port_data_ext))
      {
         m_apx_file_type = FileType::ProvidePortData;
      }
      else if (m_file_info.name_ends_with(s_require_port_data_ext))
      {
         m_apx_file_type = FileType::RequirePortData;
      }
      else if (m_file_info.name_ends_with(s_provide_port_count_ext))
      {
         m_apx_file_type = FileType::ProvidePortCount;
      }
      else if (m_file_info.name_ends_with(s_require_port_count_ext))
      {
         m_apx_file_type = FileType::RequirePortCount;
      }
      else
      {
         //m_apx_file_type is already set to Unknown in declaration
      }
   }
   bool File::less_than(File const& a, File const& b)
   {
      return a.get_address_without_flags() < b.get_address_without_flags();
   }
   bool File::ptr_less_than(File const*& a, File const*& b)
   {
      return a->get_address_without_flags() < b->get_address_without_flags();
   }

   void File::open_notify()
   {
      if (m_notification_handler != nullptr)
      {
         m_notification_handler->file_open_notify(this);
      }
   }

   void File::close_notify()
   {
      if (m_notification_handler != nullptr)
      {
         m_notification_handler->file_close_notify(this);
      }
   }
}