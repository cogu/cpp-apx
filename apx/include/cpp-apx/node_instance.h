/*****************************************************************************
* \file      node_instance.h
* \author    Conny Gustafsson
* \date      2020-10-13
* \brief     Container for an instantiated APX node
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

#include <vector>
#include <string>
#include <memory>
#include "cpp-apx/types.h"
#include "cpp-apx/port_instance.h"
#include "cpp-apx/error.h"
#include "cpp-apx/vm.h"
#include "cpp-apx/node_data.h"

namespace apx
{
   class NodeInstance
   {
   public:
      NodeInstance(){}
      ~NodeInstance();
      NodeInstance(std::string const& name) : m_name{ name } {}
      void set_name(std::string const& name) { m_name = name; }
      std::string const& get_name() { return m_name; }
      void alloc_port_instance_memory(std::size_t num_provide_ports, std::size_t num_require_ports);
      apx::error_t create_provide_port(port_id_t port_id, std::string const& name, std::unique_ptr<apx::vm::Program> pack_program, std::uint32_t data_offset, std::uint32_t& data_size);
      apx::error_t create_require_port(port_id_t port_id, std::string const& name, std::unique_ptr<apx::vm::Program> pack_program, std::unique_ptr<apx::vm::Program> unpack_program, std::uint32_t data_offset, std::uint32_t& data_size);
      std::size_t get_num_provide_ports(){ return m_num_provide_ports; }
      std::size_t get_num_require_ports(){ return m_num_require_ports; }
      std::size_t get_provide_port_init_data_size() { return m_provide_port_init_data_size; }
      std::size_t get_require_port_init_data_size() { return m_require_port_init_data_size; }
      std::uint8_t const* get_provide_port_init_data() { return m_provide_port_init_data; }
      std::uint8_t const* get_require_port_init_data() { return m_require_port_init_data; }
      apx::error_t create_port_init_data_memory(std::uint8_t*& provide_port_data, std::size_t& provide_port_data_size,
         std::uint8_t*& require_port_data, std::size_t& require_port_data_size);
      PortInstance* get_provide_port(std::size_t port_id);
      PortInstance* get_require_port(std::size_t port_id);
      apx::error_t init_node_data(std::uint8_t const* definition_data, std::size_t definition_size);
      bool has_provide_port_data() { return m_provide_port_init_data != nullptr; }
      bool has_require_port_data() { return m_require_port_init_data != nullptr; }
      NodeData const* get_const_node_data() { return m_node_data.get(); }
      NodeData* get_node_data() { return m_node_data.get(); }

   protected:
      std::string m_name;
      std::size_t m_num_provide_ports{ 0u };
      std::size_t m_num_require_ports{ 0u };
      PortInstance** m_provide_ports{ nullptr };
      PortInstance** m_require_ports{ nullptr };
      std::uint8_t* m_provide_port_init_data{ nullptr };
      std::uint8_t* m_require_port_init_data{ nullptr };
      std::size_t m_provide_port_init_data_size{ 0u };
      std::size_t m_require_port_init_data_size{ 0u };
      std::unique_ptr<NodeData> m_node_data{ nullptr };

      apx::error_t calc_init_data_size(PortInstance **port_list, std::size_t num_ports, std::size_t & total_size);
   };
}
