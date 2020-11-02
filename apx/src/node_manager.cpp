/*****************************************************************************
* \file      node_manager.cpp
* \author    Conny Gustafsson
* \date      2020-10-13
* \brief     Factory for node_instance objects
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

#include <cassert>
#include <cstring>
#include "cpp-apx\node_manager.h"
#include <iostream> //DEBUG ONLY

namespace apx
{

   apx::error_t NodeManager::build_node(char const* definition_text)
   {
      std::size_t definition_size = std::strlen(definition_text);
      apx::error_t result = m_parser.parse(definition_text);
      if (result != APX_NO_ERROR)
      {
         return result;
      }
      auto node{ m_parser.take_last_node() };

      result = create_node_instance(node.get(), reinterpret_cast<std::uint8_t const*>(definition_text), definition_size);
      if (result != APX_NO_ERROR)
      {
         return result;
      }
      return APX_NO_ERROR;
   }

   apx::error_t NodeManager::create_node_instance(Node const* node, std::uint8_t const* definition_data, std::size_t definition_size)
   {
      if ( (node == nullptr) || (definition_data == nullptr) || (definition_size == 0u) )
      {
         return APX_INVALID_ARGUMENT_ERROR;
      }
      apx::error_t result = APX_NO_ERROR;
      auto node_instance_ptr = std::make_unique<apx::NodeInstance>(node->get_name());
      std::size_t expected_provide_port_data_size{ 0u };
      std::size_t expected_require_port_data_size{ 0u };
      auto* node_instance = node_instance_ptr.get();
      result = create_ports_on_node_instance(node_instance, node, expected_provide_port_data_size, expected_require_port_data_size);
      if (result != APX_NO_ERROR)
      {
         return result;
      }
      result = create_init_data_on_node_instance(node_instance, node, expected_provide_port_data_size, expected_require_port_data_size);
      if (result != APX_NO_ERROR)
      {
         return result;
      }
      result = node_instance->init_node_data(definition_data, definition_size);
      if (result != APX_NO_ERROR)
      {
         return result;
      }
      attach_node(node_instance_ptr.release());
      return APX_NO_ERROR;
   }

   void NodeManager::attach_node(apx::NodeInstance* node_instance)
   {
      m_instance_map.insert(std::make_pair(node_instance->get_name(), std::unique_ptr<apx::NodeInstance>(node_instance)));
      m_last_attached = node_instance;
   }

   apx::error_t NodeManager::create_ports_on_node_instance(apx::NodeInstance* node_instance, Node const* node,
      std::size_t& expected_provide_port_data_size, std::size_t& expected_require_port_data_size)
   {
      apx::error_t result = APX_NO_ERROR;
      auto const num_provide_ports = static_cast<port_id_t>(node->get_num_provide_ports());
      auto const num_require_ports = static_cast<port_id_t>(node->get_num_require_ports());

      node_instance->alloc_port_instance_memory(num_provide_ports, num_require_ports);
      std::uint32_t data_offset{ 0u };
      for (port_id_t port_id = 0u; port_id < num_provide_ports; port_id++)
      {
         auto port = node->get_provide_port(port_id);
         assert(port != nullptr);
         auto pack_program = m_compiler.compile_port(port, ProgramType::Pack, result);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         std::uint32_t data_size{ 0 };
         result = node_instance->create_provide_port(port_id, port->name, std::move(pack_program), data_offset, data_size);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         data_offset += data_size;
         auto* port_instance = node_instance->get_provide_port(port_id);
         assert(port_instance != nullptr);
         result = finalize_port_instance(port_instance, port);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
      }
      expected_provide_port_data_size = static_cast<std::size_t>(data_offset);
      data_offset = 0u;
      for (port_id_t port_id = 0u; port_id < num_require_ports; port_id++)
      {
         auto port = node->get_require_port(port_id);
         assert(port != nullptr);
         auto pack_program = m_compiler.compile_port(port, ProgramType::Pack, result);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         auto unpack_program = m_compiler.compile_port(port, ProgramType::Unpack, result);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         std::uint32_t data_size{ 0 };
         result = node_instance->create_require_port(port_id, port->name, std::move(pack_program), std::move(unpack_program), data_offset, data_size);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         data_offset += data_size;
         auto* port_instance = node_instance->get_require_port(port_id);
         assert(port_instance != nullptr);
         result = finalize_port_instance(port_instance, port);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
      }
      expected_require_port_data_size = static_cast<std::size_t>(data_offset);
      return APX_NO_ERROR;
   }

   apx::error_t NodeManager::create_init_data_on_node_instance(apx::NodeInstance* node_instance, Node const* node,
      std::size_t expected_provide_port_data_size, std::size_t expected_require_port_data_size)
   {
      std::size_t provide_port_data_size{ 0u };
      std::size_t require_port_data_size{ 0u };
      std::uint8_t* provide_port_data{ nullptr };
      std::uint8_t* require_port_data{ nullptr };
      apx::error_t result = node_instance->create_port_init_data_memory(provide_port_data, provide_port_data_size,
         require_port_data, require_port_data_size);
      if (result != APX_NO_ERROR)
      {
         return result;
      }
      if ((provide_port_data_size != expected_provide_port_data_size) ||
         (require_port_data_size != expected_require_port_data_size))
      {
         return APX_LENGTH_ERROR;
      }

      auto const num_provide_ports = static_cast<port_id_t>(node->get_num_provide_ports());
      auto const num_require_ports = static_cast<port_id_t>(node->get_num_require_ports());
      if ((num_provide_ports > 0) && (provide_port_data == nullptr))
      {
         return APX_MEM_ERROR;
      }
      if ((num_require_ports > 0) && (require_port_data == nullptr))
      {
         return APX_MEM_ERROR;
      }
      std::size_t data_offset{ 0u };
      VirtualMachine vm;
      for (port_id_t port_id = 0u; port_id < num_provide_ports; port_id++)
      {
         auto parsed_port = node->get_provide_port(port_id);
         auto port_instance = node_instance->get_provide_port(port_id);
         if ( (parsed_port == nullptr) || (port_instance == nullptr) )
         {
            return APX_NULL_PTR_ERROR;
         }
         auto data_size = port_instance->get_data_size();
         assert(data_size > 0u);
         auto const* proper_init_value = parsed_port->proper_init_value.get();
         if (proper_init_value != nullptr)
         {
            result = create_port_init_data(vm, port_instance, proper_init_value, provide_port_data + data_offset, data_size);
            if (result != APX_NO_ERROR)
            {
               return result;
            }
         }
         data_offset += data_size;
      }
      data_offset = 0u;
      for (port_id_t port_id = 0u; port_id < num_require_ports; port_id++)
      {
         auto parsed_port = node->get_require_port(port_id);
         auto port_instance = node_instance->get_require_port(port_id);
         if ((parsed_port == nullptr) || (port_instance == nullptr))
         {
            return APX_NULL_PTR_ERROR;
         }
         auto data_size = port_instance->get_data_size();
         assert(data_size > 0u);
         auto const* proper_init_value = parsed_port->proper_init_value.get();
         if (proper_init_value != nullptr)
         {
            result = create_port_init_data(vm, port_instance, proper_init_value, require_port_data + data_offset, data_size);
            if (result != APX_NO_ERROR)
            {
               return result;
            }
         }
         data_offset += data_size;
      }
      return APX_NO_ERROR;
   }

   apx::error_t NodeManager::create_port_init_data(apx::VirtualMachine& vm, apx::PortInstance* port_instance, dtl::Value const* value, std::uint8_t* data, std::size_t data_size)
   {
      assert(port_instance != nullptr);
      assert(value != nullptr);
      assert(data != nullptr);
      vm::Program const& pack_program = port_instance->get_pack_program();
      auto result = vm.select_program(pack_program);
      if (result != APX_NO_ERROR)
      {
         return result;
      }
      result = vm.set_write_buffer(data, data_size);
      if (result != APX_NO_ERROR)
      {
         return result;
      }
      return vm.pack_value(value);
   }

   apx::error_t NodeManager::finalize_port_instance(apx::PortInstance* port_instance, apx::Port const* parsed_port)
   {
      auto const* parsed_data_element = parsed_port->get_data_element();
      assert(parsed_data_element != nullptr);
      auto const type_code = parsed_data_element->get_type_code();
      assert(type_code != apx::TypeCode::TypeRefId && type_code != apx::TypeCode::TypeRefName);
      if (type_code == apx::TypeCode::TypeRefPtr)
      {
         auto const* data_type = parsed_data_element->get_typeref_ptr();
         assert(data_type != nullptr);
         if (data_type->has_attributes())
         {
            auto const* attributes = data_type->get_attributes();
            std::vector<std::unique_ptr<Computation>> computations;
            for (auto &it : attributes->computations)
            {
               auto result = port_instance->append_computation(it.get());
               if (result != APX_NO_ERROR)
               {
                  return result;
               }
            }
         }
      }
      auto const* effective_data_element = parsed_port->get_effective_data_element();
      if (effective_data_element == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      auto const effective_signature = effective_data_element->to_string();
      port_instance->set_data_signature(effective_signature);
      return APX_NO_ERROR;
   }
}

