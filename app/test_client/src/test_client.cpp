#include <iostream>
#include <chrono>
#include <thread>
#include <cassert>
#include "cpp-apx/client.h"

#ifdef _WIN32
static int init_wsa(void);
#endif

int main()
{
   char const* apx_text = "APX/1.2\n"
      "N\"TestNode1\"\n"
      "P\"ProvidePort1\"C(0,3):=3\n"
      "P\"ProvidePort2\"C(0,7):=7\n"
      "R\"RequirePort1\"C(0,15):=15\n";

#ifdef _WIN32
   init_wsa();
#endif
   apx::Client client;
   std::cout << "Building node" << std::endl;
   auto result = client.build_node(apx_text);
   if (result != APX_NO_ERROR)
   {
      return -1;
   }   
   std::cout << "Connecting to server" << std::endl;
   result = client.connect_tcp("127.0.0.1", 5000);
   if (result != APX_NO_ERROR)
   {
      return -1;
   }
   std::cout << "Running event loop" << std::endl;
   for (;;)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(2000));
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