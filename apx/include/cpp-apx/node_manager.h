#pragma once

#include <memory>
#include <unordered_map>
#include <string>
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
      apx::NodeInstance* get_last_attached() { return m_last_attached; }
      std::size_t size() { return m_instance_map.size(); }
   protected:
      apx::Parser m_parser;
      apx::Compiler m_compiler;
      apx::VirtualMachine m_vm;
      std::unordered_map<std::string, std::unique_ptr<apx::NodeInstance>> m_instance_map;
      apx::NodeInstance* m_last_attached{ nullptr };

      apx::error_t create_node_instance(Node const* node);
      void attach_node(apx::NodeInstance* node_instance);
      apx::error_t create_ports_on_node_instance(apx::NodeInstance* node_instance, Node const* node,
         std::size_t &expected_provide_port_data_size, std::size_t& expected_require_port_data_size);
      apx::error_t create_init_data_on_node_instance(apx::NodeInstance* node_instance, Node const* node,
         std::size_t expected_provide_port_data_size, std::size_t expected_require_port_data_size);
      apx::error_t create_port_init_data(apx::PortInstance* port_instance, dtl::Value const* value, std::uint8_t* data, std::size_t data_size);
   };
}