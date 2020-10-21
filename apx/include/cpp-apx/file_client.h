/*****************************************************************************
* \file      file_client.h
* \author    Conny Gustafsson
* \date      2020-10-21
* \brief     An APX client that operates on files in local file system 
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

#include <filesystem>
#include <memory>
#include <vector>
#include "cpp-apx/node_manager.h"
#include "cpp-apx/error.h"
#include "cpp-apx/vm.h"

namespace apx
{
   class FileClient
   {
   public:
      FileClient();
      FileClient(std::filesystem::path const& directory):
         m_directory{directory}{}
      apx::error_t build_node(char const* apx_definition);
      apx::error_t save_all();
      apx::error_t save_if_new();
      apx::error_t save_provide_port_data();
      apx::error_t save_require_port_data();
      bool has_provide_port_data();
      bool has_require_port_data();
      NodeData const* get_const_node_data() { return m_node != nullptr ? m_node->get_const_node_data() : nullptr; }
      apx::error_t write_port(apx::port_id_t port_id, dtl::ScalarValue sv);
      apx::error_t read_port(apx::port_id_t port_id, dtl::ScalarValue &sv);
   protected:
      std::filesystem::path m_directory;
      NodeInstance* m_node{ nullptr };
      NodeManager m_node_manager;
      VirtualMachine m_vm;
      std::vector<std::uint8_t> m_buffer;

      void save_buffer_to_file(std::filesystem::path file_path, std::uint8_t const* buffer, std::size_t size);
      apx::error_t read_data_from_filesystem();
      apx::error_t read_require_port_data_from_file();
      apx::error_t read_provide_port_data_from_file();
   };
}
