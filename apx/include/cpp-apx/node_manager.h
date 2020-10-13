#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include "cpp-apx/types.h"
#include "cpp-apx/node_instance.h"
#include "cpp-apx/parser.h"
#include "cpp-apx/compiler.h"
#include "cpp-apx/error.h"

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
      std::unordered_map<std::string, std::unique_ptr<apx::NodeInstance>> m_instance_map;
      apx::NodeInstance* m_last_attached{ nullptr };

      apx::error_t create_node_instance(Node const* node);
      void attach_node(apx::NodeInstance* node_instance);
   };
}