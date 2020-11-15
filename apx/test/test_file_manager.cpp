#include "pch.h"
#include <array>
#include <cstring>

#include "cpp-apx/mock_client_connection.h"
#include "cpp-apx/remotefile.h"

using namespace apx;
namespace apx_test
{
   TEST(FileManager, LocalFilesAreAnnouncedWhenConnected)
   {
      MockClientConnection mock_connection;
      auto& file_manager = mock_connection.get_file_manager();
      file_manager.create_local_file(rmf::FileInfo("TestNode1.apx", 40u));
      file_manager.create_local_file(rmf::FileInfo("TestNode1.out", 2u));
      EXPECT_EQ(mock_connection.log_length(), 0u);
      mock_connection.greeting_header_accepted();
      mock_connection.run();
      EXPECT_EQ(mock_connection.log_length(), 1u);
      auto const& buffer = mock_connection.get_log_packet(0);
      EXPECT_EQ(buffer.size(), 67u*2u);
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
      expected[13] = 40u; //FileSize[0]
      expected[63] = 'a'; //FileName extension
      expected[64] = 'p'; //FileName extension
      expected[65] = 'x'; //FileName extension

      std::memcpy(actual.data(), buffer.data() + expected.size(), actual.size());
      EXPECT_EQ(expected, actual);

   }
}