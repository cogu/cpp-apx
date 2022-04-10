#include <iostream>
#include <chrono>
#include <thread>
#include <cassert>
#include "cpp-apx/client.h"

#ifdef _WIN32
static int init_wsa(void);
#endif

constexpr int DemoSignal3_ID = 0;

class EventListener : public apx::ClientEventListener
{
public:
   EventListener(apx::Client& client):m_client{client}{}
   void connected1(apx::ClientConnection* connection)
   {
      std::cout << "[EVENT] Connected" << std::endl;
   }

   void disconnected1(apx::ClientConnection* connection)
   {
      std::cout << "[EVENT] Disconnected" << std::endl;
   }

   void require_port_written1(apx::PortInstance* port_instance)
   {
      bool ok{ false };
      dtl::ScalarValue sv;
      switch (port_instance->port_id())
      {
      case DemoSignal3_ID:
         (void) m_client.read_port_value(port_instance, sv);
         std::cout << "[EVENT] " << port_instance->name() << ": " << sv->to_u32(ok) << std::endl;
      }
   }
protected:
   apx::Client& m_client;
};

int main()
{

   char const* apx_text = "APX/1.3\n"
      "N\"CppTestNode\"\n"
      "P\"DemoSignal1\"C(0,3):=3\n"
      "P\"DemoSignal2\"C(0,7):=7\n"
      "R\"DemoSignal3\"C(0,15):=15\n";

   std::uint8_t DemoSignal1{ 0u };

#ifdef _WIN32
   init_wsa();
#endif
   apx::Client client;
   EventListener listener{ client };
   client.register_event_listener(&listener);
   std::cout << "Building node" << std::endl;
   auto result = client.build_node(apx_text);
   if (result != APX_NO_ERROR)
   {
      return -1;
   }
   //apx::PortInstance* TestPort1Instance{ client.get_port("CppTestNode", "DemoSignal1") };
   std::cout << "Connecting to server" << std::endl;
   result = client.connect_tcp("127.0.0.1", 5000);
   if (result != APX_NO_ERROR)
   {
      return -1;
   }
   std::cout << "Running event loop" << std::endl;
   for (;;)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
   }
   return 0;
}

#ifdef _WIN32
static int init_wsa(void)
{
   WORD wVersionRequested;
   WSADATA wsaData;
   int err;
   wVersionRequested = MAKEWORD(2, 2);
   err = WSAStartup(wVersionRequested, &wsaData);
   return err;
}
#endif