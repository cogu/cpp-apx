#include "pch.h"
#include <array>
#include <cstring>

#include "cpp-apx/client.h"
#include "client_spy.h"
#include "testsocket_spy.h"

using namespace apx;
using namespace std::string_literals;

namespace apx_test
{
   TEST(Client, ConnectedEventIsTriggered)
   {
      char const* apx_text = "APX/1.2\n"
         "N\"TestNode1\"\n"
         "R\"RequirePort1\"C(0,3):=3\n"
         "R\"RequirePort2\"C(0,7):=7\n";

      testsocket_spy_create();
      {
         Client client;
         ClientSpy spy;
         client.register_event_listener(&spy);
         EXPECT_EQ(client.build_node(apx_text), APX_NO_ERROR);
         EXPECT_EQ(spy.connect_count(), 0);
         auto* socket = testsocket_client_spy();
         client.connect(socket);
         client.run();
         EXPECT_EQ(spy.connect_count(), 1);
      }
      testsocket_spy_destroy();
   }

   TEST(Client, RequirePortWriteEventIsTriggered)
   {
      char const* apx_text = "APX/1.2\n"
         "N\"TestNode1\"\n"
         "R\"RequirePort1\"C(0,3):=3\n"
         "R\"RequirePort2\"C(0,7):=7\n";

      testsocket_spy_create();
      {
         Client client;
         ClientSpy spy;
         //Setup start
         client.register_event_listener(&spy);
         EXPECT_EQ(client.build_node(apx_text), APX_NO_ERROR);
         EXPECT_EQ(spy.connect_count(), 0);
         auto* socket = testsocket_client_spy();
         client.connect(socket);
         client.run();
         client.receive_accepted_cmd();
         client.receive_file_info_cmd(PORT_DATA_ADDRESS_START, "TestNode1.in", 2u);
         client.run();
         //Setup done
         std::array<std::uint8_t, 2> port_data{ 0, 1 };
         EXPECT_EQ(spy.require_port_write_count(), 0);
         client.receive_data_messsage(PORT_DATA_ADDRESS_START, port_data.data(), port_data.size());
         client.run();

         EXPECT_EQ(spy.require_port_write_count(), 2);
         auto port_instance = spy.last_require_port_write();
         EXPECT_EQ(port_instance->port_id(), 1u);
         EXPECT_EQ(port_instance->name(), "RequirePort2"s);
         dtl::ScalarValue sv;
         EXPECT_EQ(client.read_port_value(port_instance, sv), APX_NO_ERROR);
         bool ok{ false };
         EXPECT_EQ(sv->to_u32(ok), 1u);
         EXPECT_TRUE(ok);
      }
      testsocket_spy_destroy();
   }

   TEST(Client, ProvidePortWrite_UInt8)
   {
      char const* apx_text = "APX/1.2\n"
         "N\"TestNode1\"\n"
         "P\"ProvidePort1\"C(0,3):=3\n"
         "P\"ProvidePort2\"C(0,7):=7\n";

      Client client;
      EXPECT_EQ(client.build_node(apx_text), APX_NO_ERROR);
      auto* port_instance = client.get_port("TestNode1", "ProvidePort1");
      ASSERT_TRUE(port_instance);
      dtl::ScalarValue sv = dtl::make_sv<std::uint32_t>( 1u );
      EXPECT_EQ(client.write_port_value(port_instance, sv), APX_NO_ERROR);
      auto* node_data = port_instance->node_instance()->get_node_data();
      ASSERT_TRUE(node_data);
      std::array<std::uint8_t, 2> read_buffer;
      EXPECT_EQ(node_data->read_provide_port_data(0, read_buffer.data(), read_buffer.size()), APX_NO_ERROR);
      EXPECT_EQ(read_buffer[0], 1u);
      EXPECT_EQ(read_buffer[1], 7u);
   }
}