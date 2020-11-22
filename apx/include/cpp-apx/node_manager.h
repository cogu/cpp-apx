/*****************************************************************************
* \file      node_manager.h
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
#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include <map>
#include "cpp-apx/types.h"
#include "cpp-apx/node_instance.h"
#include "cpp-apx/parser.h"
#include "cpp-apx/compiler.h"
#include "cpp-apx/error.h"
#include "cpp-apx/vm.h"
#include "dtl/dtl.hpp"

namespace apx
{
   class NodeManager
   {
   public:
      apx::error_t build_node(char const* definition_text);
      apx::error_t build_node(std::string const& definition_text);
      apx::NodeInstance* get_last_attached() { return m_last_attached; }
      std::size_t size() { return m_instance_map.size(); }
      std::vector<apx::NodeInstance*> get_nodes();
      apx::NodeInstance* find(char const* name);
      apx::NodeInstance* find(std::string const& name);
   protected:
      apx::Parser m_parser;
      apx::Compiler m_compiler;
      std::unordered_map<std::string, std::unique_ptr<apx::NodeInstance>> m_instance_map;
      apx::NodeInstance* m_last_attached{ nullptr };
      using DataElementMap = std::map<std::string, DataElement const*>;
      using ComputationListMap = std::map<std::string, ComputationList const*>;
      using DataElementList = std::vector<std::unique_ptr<apx::DataElement>>;
      using ComputationListOfLists = std::vector<std::unique_ptr<apx::ComputationList>>;

      void reset();
      apx::error_t create_node_instance(Node const* node, std::uint8_t const* definition_data, std::size_t definition_size);
      void attach_node(apx::NodeInstance* node_instance);
      apx::error_t create_ports_on_node_instance(apx::NodeInstance* node_instance, Node const* node,
         std::size_t &expected_provide_port_data_size, std::size_t& expected_require_port_data_size);
      apx::error_t create_init_data_on_node_instance(apx::NodeInstance* node_instance, Node const* node,
         std::size_t expected_provide_port_data_size, std::size_t expected_require_port_data_size);
      apx::error_t create_port_init_data(apx::VirtualMachine &vm, apx::PortInstance* port_instance, dtl::Value const* value, std::uint8_t* data, std::size_t data_size);
      apx::error_t create_data_element_list_on_node_instance(apx::NodeInstance* node_instance, Node const* node);
      apx::error_t update_data_element_list_on_port(DataElementList &list, DataElementMap& map, apx::PortInstance* port_instance, apx::Port const* parsed_port);
      apx::error_t create_computation_list_on_node_instance(apx::NodeInstance* node_instance, Node const* node);
      apx::error_t update_computation_list_on_port(ComputationListOfLists& list, ComputationListMap& map, apx::PortInstance* port_instance, apx::Port const* parsed_port);
   };
}