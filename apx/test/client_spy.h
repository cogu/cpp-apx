#pragma once
#include "cpp-apx/event_listener.h"

namespace apx
{
   class ClientSpy : public ClientEventListener
   {
   public:
      void connected1(ClientConnection* connection) override;
      void disconnected1(ClientConnection* connection) override;
      void require_port_written1(PortInstance* port_instance) override;

      int connect_count() const { return m_connect_count; }
      int disconnect_count() const { return m_disconnect_count; }
      int require_port_write_count() const { return m_require_port_write_count; }
      PortInstance* last_require_port_write() const { return m_last_require_port_write; }
   protected:
      int m_connect_count{ 0 };
      int m_disconnect_count{ 0 };
      int m_require_port_write_count{ 0 };
      PortInstance* m_last_require_port_write{ nullptr };
   };

}

