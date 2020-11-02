/*****************************************************************************
* \file      node_serializer.h
* \author    Conny Gustafsson
* \date      2020-11-01
* \brief     Binary serializer for node_instance objects
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
#include <string>
#include <fstream>
#include <array>
#include "cpp-apx/node_serializer.h"
#include "cpp-apx/vmdefs.h"

namespace fs = std::filesystem;

using namespace std::string_literals;

namespace apx
{
   NodeSerializer::NodeSerializer()
   {
      m_directory = fs::current_path() / ".apx"s;
   }

   void NodeSerializer::set_directory(std::filesystem::path const& directory)
   {
      m_directory = directory;
   }

   apx::error_t NodeSerializer::save_node(NodeInstance const* node_instance)
   {
      fs::path path_to_file = m_directory / (node_instance->get_name() + m_file_extension);
      std::ofstream file(path_to_file, std::ios::out | std::ios::binary | std::ios::trunc);
      if (file.is_open())
      {
         write_file_header(&file);
         write_node_name(&file, node_instance);
         file.flush();
         file.close();
      }
      else
      {
         return APX_FILE_NOT_OPEN_ERROR;
      }
      return APX_NO_ERROR;
   }

   void NodeSerializer::write_file_header(std::ostream* stream)
   {
      std::array<char, 9u> header = { 'A', 'P', 'X', NODE_HEADER_MAJOR_VERSION, NODE_HEADER_MINOR_VERSION,
                                              'V', 'M', vm::MAJOR_VERSION, vm::MINOR_VERSION };
      stream->write(header.data(), header.size());
   }

   void NodeSerializer::write_node_name(std::ostream* stream, NodeInstance const* node_instance)
   {
      std::string const& name = node_instance->get_name();
      stream->write(name.data(), name.size());
      stream->put('\0');
   }

}