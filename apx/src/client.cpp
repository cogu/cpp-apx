#include "cpp-apx/client.h"

namespace apx
{
   error_t Client::build_node(char const* apx_text)
   {
      return m_node_manager.build_node(apx_text);
   }

   error_t Client::build_node(std::string const& apx_text)
   {
      return m_node_manager.build_node(apx_text);
   }

   error_t Client::connect_tcp(char const* address, std::uint16_t port)
   {
      auto* msocket = msocket_new(AF_INET);
      if (msocket == nullptr)
      {
         return APX_MEM_ERROR;
      }
      m_connection = std::make_unique<SocketClientConnection>(msocket);
      m_connection->start();
      m_connection->attach_node_manager(&m_node_manager);
      return m_connection->connect_tcp(address, port);
   }

   error_t Client::connect_tcp(std::string const& address, std::uint16_t port)
   {
      return connect_tcp(address.data(), port);
   }

   error_t Client::connect_unix(char const* path)
   {
      (void)path;
      return APX_UNSUPPORTED_ERROR;
   }

   error_t Client::connect_unix(std::string const& path)
   {
      (void)path;
      return APX_UNSUPPORTED_ERROR;
   }
}