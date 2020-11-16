#include "pch.h"
#include <cassert>
#include "cpp-apx/client_connection.h"

namespace apx
{
   void ClientConnection::greeting_header_accepted()
   {
      m_file_manager.connected();
   }

   void ClientConnection::connected()
   {

   }

   void ClientConnection::disconnected()
   {

   }

   void ClientConnection::attach_node_manager(NodeManager* node_manager)
   {
      m_node_manager = node_manager;
   }

   error_t ClientConnection::build_node(char const* definition_text)
   {
      if (definition_text == nullptr)
      {
         return APX_INVALID_ARGUMENT_ERROR;
      }
      if (m_node_manager == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      auto retval = m_node_manager->build_node(definition_text);
      if (retval == APX_NO_ERROR)
      {
         auto* node_instance = m_node_manager->get_last_attached();
         assert(node_instance != nullptr);
         attach_node_instance(node_instance);
      }
      return retval;
   }

   error_t ClientConnection::attach_node_instance(NodeInstance* node_instance)
   {
      return node_instance->attach_to_file_manager(&m_file_manager);
   }
}