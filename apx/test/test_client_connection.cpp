#include "pch.h"
#include <array>
#include <cstring>

#include "cpp-apx/mock_client_connection.h"
#include "cpp-apx/remotefile.h"
#include "cpp-apx/sha256.h"
#include "cpp-apx/numheader.h"
#include "cpp-apx/pack.h"

using namespace apx;
namespace apx_test
{
   TEST(ClientConnection, LocalFilesArePublishedWhenGreetingIsAccepted)
   {
      char const* apx_text = "APX/1.2\n"
         "N\"TestNode1\"\n"
         "P\"ProvidePort1\"C(0,3)\n"
         "P\"ProvidePort2\"C(0,7)\n";
      MockClientConnection mock_connection;
      EXPECT_EQ(mock_connection.build_node(apx_text), APX_NO_ERROR);
      EXPECT_EQ(mock_connection.log_length(), 0u);
      mock_connection.greeting_header_accepted();
      mock_connection.run();
      EXPECT_EQ(mock_connection.log_length(), 1u);
      auto const& buffer = mock_connection.get_log_packet(0);
      EXPECT_EQ(buffer.size(), 67u * 2u);
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
      std::memcpy(actual.data(), buffer.data(), actual.size());
      EXPECT_EQ(expected, actual);

      //Check second message
      expected[12] = 0x04; //StartAddress[0]
      expected[13] = static_cast<std::uint8_t>(strlen(apx_text)); //FileSize[0]
      expected[19] = static_cast<std::uint8_t>(rmf::DigestType::SHA256);
      sha256::calc(&expected[21], rmf::SHA256_SIZE, reinterpret_cast<std::uint8_t const*>(apx_text), strlen(apx_text));
      expected[63] = 'a'; //FileName extension
      expected[64] = 'p'; //FileName extension
      expected[65] = 'x'; //FileName extension

      std::memcpy(actual.data(), buffer.data() + expected.size(), actual.size());
      EXPECT_EQ(expected, actual);
   }

   TEST(ClientConnection, DefinitionFileIsSentWhenOpenRequested)
   {
      char const* apx_text = "APX/1.2\n"
         "N\"TestNode1\"\n"
         "P\"ProvidePort1\"C(0,3)\n"
         "P\"ProvidePort2\"C(0,7)\n";
      MockClientConnection mock_connection;
      EXPECT_EQ(mock_connection.build_node(apx_text), APX_NO_ERROR);
      EXPECT_EQ(mock_connection.log_length(), 0u);
      mock_connection.greeting_header_accepted();
      mock_connection.run();
      EXPECT_EQ(mock_connection.log_length(), 1u);
      auto const& buffer1 = mock_connection.get_log_packet(0);
      EXPECT_EQ(buffer1.size(), 67u * 2u); //This should be the FileInfo messages
      mock_connection.clear_log();
      EXPECT_EQ(mock_connection.request_open_local_file("TestNode1.apx"), APX_NO_ERROR);
      mock_connection.run();
      EXPECT_EQ(mock_connection.log_length(), 1u);
      auto const& buffer2 = mock_connection.get_log_packet(0);
      EXPECT_EQ(buffer2.size(), numheader::SHORT_SIZE + rmf::HIGH_ADDR_SIZE + std::strlen(apx_text));
      EXPECT_EQ(buffer2[0], 69u);
      std::uint32_t address{ rmf::INVALID_ADDRESS };
      bool more_bit{ false };
      EXPECT_EQ(rmf::address_decode(&buffer2[1], &buffer2[1] + rmf::HIGH_ADDR_SIZE, address, more_bit), rmf::HIGH_ADDR_SIZE);
      EXPECT_EQ(address, DEFINITION_ADDRESS_START);
      EXPECT_FALSE(more_bit);
   }

   TEST(ClientConnection, ProvidePortFileIsSentWhenOpenRequested)
   {
      char const* apx_text = "APX/1.2\n"
         "N\"TestNode1\"\n"
         "P\"ProvidePort1\"C(0,3):=3\n"
         "P\"ProvidePort2\"C(0,7):=7\n";
      MockClientConnection mock_connection;
      EXPECT_EQ(mock_connection.build_node(apx_text), APX_NO_ERROR);
      EXPECT_EQ(mock_connection.log_length(), 0u);
      mock_connection.greeting_header_accepted();
      mock_connection.run();
      EXPECT_EQ(mock_connection.log_length(), 1u);
      auto const& buffer1 = mock_connection.get_log_packet(0);
      EXPECT_EQ(buffer1.size(), 67u * 2u); //This should be the FileInfo messages
      mock_connection.clear_log();
      EXPECT_EQ(mock_connection.request_open_local_file("TestNode1.out"), APX_NO_ERROR);
      mock_connection.run();
      EXPECT_EQ(mock_connection.log_length(), 1u);
      auto const& buffer2 = mock_connection.get_log_packet(0);
      EXPECT_EQ(buffer2.size(), numheader::SHORT_SIZE + rmf::LOW_ADDR_SIZE + sizeof(std::uint8_t)*2);
      EXPECT_EQ(buffer2[0], 4u);
      std::uint32_t address{ rmf::INVALID_ADDRESS };
      bool more_bit{ false };
      EXPECT_EQ(rmf::address_decode(&buffer2[1], &buffer2[1] + rmf::HIGH_ADDR_SIZE, address, more_bit), rmf::LOW_ADDR_SIZE);
      EXPECT_EQ(address, PORT_DATA_ADDRESS_START);
      EXPECT_FALSE(more_bit);
      EXPECT_EQ(buffer2[3], 0x03u);
      EXPECT_EQ(buffer2[4], 0x07u);
   }
}