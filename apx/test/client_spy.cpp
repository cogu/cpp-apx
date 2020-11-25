#include "client_spy.h"

namespace apx
{
   void ClientSpy::connected1(ClientConnection* connection)
   {
      (void)connection;
      m_connect_count++;
   }

   void ClientSpy::disconnected1(ClientConnection* connection)
   {
      (void)connection;
      m_disconnect_count++;
   }

   void ClientSpy::require_port_written1(PortInstance* port_instance)
   {
      m_require_port_write_count++;
      m_last_require_port_write = port_instance;
   }
}
