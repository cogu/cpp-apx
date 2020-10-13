#include <cassert>
#include "cpp-apx\node_manager.h"
#include <iostream> //DEBUG ONLY

namespace apx
{

   apx::error_t NodeManager::build_node(char const* definition_text)
   {
      apx::error_t result = m_parser.parse(definition_text);
      if (result != APX_NO_ERROR)
      {
         return result;
      }
      auto node{ m_parser.take_last_node() };

      result = create_node_instance(node.get());
      if (result != APX_NO_ERROR)
      {
         return result;
      }
      return APX_NO_ERROR;
   }

   apx::error_t NodeManager::create_node_instance(Node const* node)
   {
      apx::error_t result = APX_NO_ERROR;
      if (node == nullptr)
      {
         return APX_INVALID_ARGUMENT_ERROR;
      }
      auto node_instance = std::make_unique<apx::NodeInstance>(node->get_name());
      auto const num_provide_ports = static_cast<port_id_t>(node->get_num_provide_ports());
      auto const num_require_ports = static_cast<port_id_t>(node->get_num_require_ports());

      node_instance->init_port_memory(num_provide_ports, num_require_ports);
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
         else
         {
            pack_program.release();
         }
         data_offset += data_size;
      }
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
         std::uint32_t data_size{ 0 };
         result = node_instance->create_require_port(port_id, port->name, std::move(pack_program), std::unique_ptr<apx::vm::Program>() , data_offset, data_size);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         else
         {
            pack_program.release();
         }
         data_offset += data_size;
      }
      attach_node(node_instance.release());
      return APX_NO_ERROR;
   }

   void NodeManager::attach_node(apx::NodeInstance* node_instance)
   {
      m_instance_map.insert(std::make_pair(node_instance->get_name(), std::unique_ptr<apx::NodeInstance>(node_instance)));
      m_last_attached = node_instance;
   }
}

