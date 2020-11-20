#include "pch.h"
#include <array>
#include <cassert>
#include <cstring>

#include "testsocket_spy.h"
#include "cpp-apx/socket_client_connection.h"
#include "cpp-apx/remotefile.h"
#include "cpp-apx/sha256.h"
#include "cpp-apx/numheader.h"
#include "cpp-apx/pack.h"

using namespace apx;
namespace apx_test
{
   static void testsocket_helper_send_acknowledge(testsocket_t* sock);
   TEST(SocketClientConnection, SendGreetingOnConnect)
   {
      uint32_t len;
      std::string expected_greeting{ "RMFP/1.0\nNumHeader-Format:32\n\n" };
      testsocket_spy_create();
      testsocket_t* sock = testsocket_client_spy();
      ASSERT_TRUE(sock);
      SocketClientConnection connection(sock);
      EXPECT_EQ(testsocket_spy_getServerConnectedCount(), 0);
      EXPECT_EQ(connection.connect(), APX_NO_ERROR);
      connection.run();
      EXPECT_EQ(testsocket_spy_getServerConnectedCount(), 1);
      const char* data = (const char*)testsocket_spy_getReceivedData(&len);
      ASSERT_EQ(len, 31u);
      EXPECT_EQ(30, data[0]);
      std::string actual_greeting{ &data[1], 30 };
      EXPECT_EQ(expected_greeting.data(), actual_greeting);
      testsocket_spy_destroy();
   }

   TEST(SocketClientConnection, SendFileInfoAfterGreetingAcknowledge)
   {
      char const* apx_text = "APX/1.2\n"
         "N\"TestNode1\"\n"
         "P\"ProvidePort1\"C(0,3)\n"
         "P\"ProvidePort2\"C(0,7)\n";

      NodeManager node_manager;
      EXPECT_EQ(node_manager.build_node(apx_text), APX_NO_ERROR);
      testsocket_spy_create();
      testsocket_t* sock = testsocket_client_spy();
      ASSERT_TRUE(sock);
      SocketClientConnection connection(sock);
      connection.attach_node_manager(&node_manager);
      EXPECT_EQ(testsocket_spy_getServerConnectedCount(), 0);
      EXPECT_EQ(connection.connect(), APX_NO_ERROR);
      connection.run();
      EXPECT_EQ(testsocket_spy_getServerConnectedCount(), 1);
      EXPECT_EQ(testsocket_spy_getServerBytesReceived(), 31u);
      testsocket_spy_clearReceivedData();
      uint32_t received_len;
      const char* received_data = (const char*)testsocket_spy_getReceivedData(&received_len);
      ASSERT_EQ(received_len, 0u);
      testsocket_helper_send_acknowledge(sock);
      connection.run();
      received_data = (const char*)testsocket_spy_getReceivedData(&received_len);
      ASSERT_EQ(received_len, 67*2u);
      std::array<std::uint8_t, 67> expected = {
         66, //(NumHeader short)
         //RemoteFile Header (High Address)
         0xBFu,
         0xFFu,
         0xFCu,
         0x00u,
         //CmdType (U32LE)
         static_cast<std::uint8_t>(rmf::CMD_PUBLISH_FILE_MSG),
         0u,
         0u,
         0u,
         //StartAddress (U32LE)
         0u,
         0u,
         0u,
         0u,
         //FileSize (U32LE)
         2u,
         0u,
         0u,
         0u,
         //FileType (U16LE)
         static_cast<std::uint8_t>(rmf::FileType::Fixed),
         0u,
         //DigestType (U16LE)
         static_cast<std::uint8_t>(rmf::DigestType::None),
         0u,
         //DigestData U8[32]
         0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
         0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
         //FileName (Null-terminated string)
         'T', 'e', 's', 't', 'N','o','d','e', '1', '.','o','u','t','\0'
      };
      std::array<std::uint8_t, expected.size()> actual;
      //Check first message
      std::memcpy(actual.data(), received_data, actual.size());
      EXPECT_EQ(expected, actual);

      //Check second message
      expected[12] = 0x04; //StartAddress[0]
      expected[13] = static_cast<std::uint8_t>(strlen(apx_text)); //FileSize[0]
      expected[19] = static_cast<std::uint8_t>(rmf::DigestType::SHA256);
      sha256::calc(&expected[21], rmf::SHA256_SIZE, reinterpret_cast<std::uint8_t const*>(apx_text), strlen(apx_text));
      expected[63] = 'a'; //FileName extension
      expected[64] = 'p'; //FileName extension
      expected[65] = 'x'; //FileName extension

      std::memcpy(actual.data(), received_data + actual.size(), actual.size());
      EXPECT_EQ(expected, actual);
      testsocket_spy_destroy();
   }

   static void testsocket_helper_send_acknowledge(testsocket_t* sock)
   {
      std::array<std::uint8_t, 1 + 8> buffer;

      std::size_t data_size = rmf::address_encode(&buffer[1], 8, rmf::CMD_AREA_START_ADDRESS, false);
      data_size += rmf::encode_acknowledge_cmd(&buffer[1 + data_size], sizeof(std::uint32_t));
      assert(data_size == 8);
      buffer[0] = static_cast<std::uint8_t>(data_size);
      testsocket_serverSend(sock, buffer.data(), static_cast<std::uint32_t>(buffer.size()));
   }

}