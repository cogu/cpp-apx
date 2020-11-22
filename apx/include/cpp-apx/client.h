#pragma once

#include <memory>
#include "cpp-apx/socket_client_connection.h"

namespace apx
{
   class Client
   {
   public:
      error_t build_node(char const* apx_text);
      error_t build_node(std::string const& apx_text);
      error_t connect_tcp(char const* address, std::uint16_t port);
      error_t connect_tcp(std::string const& address, std::uint16_t port);
      error_t connect_unix(char const* path);
      error_t connect_unix(std::string const& path);
      
   protected:
      NodeManager m_node_manager;
      std::unique_ptr<SocketClientConnection> m_connection{ nullptr };
   };
}
