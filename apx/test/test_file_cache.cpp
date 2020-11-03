#include "pch.h"
#include <array>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstring>
#include "cpp-apx/file_cache.h"
#include "cpp-apx/vmdefs.h"
#include "cpp-apx/node_manager.h"
#include "cpp-apx/sha256.h"


using namespace apx;
namespace fs = std::filesystem;

namespace apx_test
{

   static std::unique_ptr<std::uint8_t[]> read_buffer_from_file(fs::path const& path_to_file, std::size_t& size)
   {

      std::unique_ptr<std::uint8_t[]> retval;

      std::ifstream file(path_to_file, std::ios::in | std::ios::binary | std::ios::ate);
      if (file.is_open())
      {
         std::streampos file_size = file.tellg();
         if (file_size > 0u)
         {
            size = static_cast<std::size_t>(file_size);
            retval.reset(new std::uint8_t[size]);
            file.seekg(0, std::ios::beg);
            file.read(reinterpret_cast<char*>(retval.get()), size);
            file.close();
         }
         else
         {
            //TODO: What to do when file is not open?
         }
      }
      return std::move(retval);
   }

   class FileCacheTest : public ::testing::Test
   {
   protected:
      fs::path m_test_dir;

      FileCacheTest()
      {
         m_test_dir = fs::temp_directory_path() / "apx_test";
         fs::create_directory(m_test_dir);
      }

      ~FileCacheTest()
      {
         for (auto it = fs::directory_iterator(m_test_dir); it != fs::directory_iterator(); ++it)
         {
            fs::remove(*it);
         }
      }
   };

   TEST_F(FileCacheTest, NodeWithoutPorts)
   {
      std::size_t const expected_definition_size = 21u;
      constexpr std::uint8_t expected_number_of_data_elements = 0u;
      constexpr std::uint8_t expected_number_of_computation_lists = 0u;
      constexpr std::uint8_t expected_number_of_provide_ports = 0u;
      constexpr std::uint8_t expected_number_of_require_ports = 0u;
      constexpr std::size_t node_name_size = 10u; //Includes null-terminator char
      std::array<std::uint8_t, NODE_VERSION_HEADER_SIZE + node_name_size + vm::UINT8_SIZE + SHA256_HASH_SIZE + vm::UINT8_SIZE*4> expected = {
         'A', 'P', 'X', NODE_HEADER_MAJOR_VERSION, NODE_HEADER_MINOR_VERSION, 'V','M', vm::MAJOR_VERSION, vm::MINOR_VERSION,
         'T', 'e', 's', 't', 'N', 'o', 'd', 'e', '0','\0', static_cast<std::uint8_t>(expected_definition_size),
         0x43,0xbb,0x81,0xdc,0xb1,0x7c,0xec,0x4b,0x63,0x55,0xfd,0x4b,0x7b,0x88,0x40,0x9b,
         0x4c,0x1e,0x1d,0x7e,0xff,0xa0,0x52,0xc1,0x8e,0x09,0x50,0x28,0xae,0x83,0x67,0x80,
         expected_number_of_data_elements, expected_number_of_computation_lists,
         expected_number_of_provide_ports, expected_number_of_require_ports
      };

      const char* apx_text =
         "APX/1.2\n"
         "N\"TestNode0\"\n";
      apx::NodeManager manager;
      EXPECT_EQ(manager.build_node(apx_text), APX_NO_ERROR);
      EXPECT_EQ(manager.size(), 1u);
      auto* node = manager.get_last_attached();
      EXPECT_NE(node, nullptr);

      FileCache cache;
      cache.set_directory(m_test_dir);
      EXPECT_EQ(cache.store(node), APX_NO_ERROR);
      fs::path const path_to_node_file = m_test_dir / "TestNode0-ae836780.apxnode";
      EXPECT_TRUE(fs::exists(path_to_node_file));
      std::size_t read_size{ 0u };
      auto ptr = read_buffer_from_file(path_to_node_file, read_size);
      std::uint8_t* buf = ptr.get();
      ASSERT_NE(buf, nullptr);
      std::array<std::uint8_t, expected.size()> read_array;
      std::memset(read_array.data(), 0u, read_array.size());
      ASSERT_EQ(read_size, read_array.size());
      std::memcpy(read_array.data(), buf, read_size);
      ASSERT_EQ(expected, read_array);

      fs::path const path_to_apx_file = m_test_dir / "TestNode0-ae836780.apx";
      EXPECT_TRUE(fs::exists(path_to_apx_file));
      ptr = read_buffer_from_file(path_to_apx_file, read_size);
      buf = ptr.get();
      ASSERT_NE(buf, nullptr);
      ASSERT_EQ(read_size, expected_definition_size);
      std::array<char, expected_definition_size + 1> definition_chars;
      std::memset(definition_chars.data(), 0, definition_chars.size());
      std::memcpy(definition_chars.data(), buf, expected_definition_size); //This is 1 byte shorter than the line above, ensuring that we end string with a null-terminator.
      EXPECT_STREQ(apx_text, definition_chars.data());
   }

   TEST_F(FileCacheTest, NodeWithUint8ProvidePort)
   {
      std::size_t const expected_definition_size = 43u;
      constexpr std::uint8_t expected_number_of_data_elements = 1u;
      constexpr std::uint8_t expected_number_of_computation_lists = 0u;
      constexpr std::uint8_t expected_number_of_provide_ports = 1u;
      constexpr std::uint8_t expected_number_of_require_ports = 0u;
      constexpr std::size_t node_name_size = 10u; //Includes null-terminator char
      constexpr std::size_t data_element_size = 8u;
      constexpr std::uint8_t provide_port_data_size = 14u;
      std::array<std::uint8_t, NODE_VERSION_HEADER_SIZE + node_name_size + vm::UINT8_SIZE +
         SHA256_HASH_SIZE + vm::UINT8_SIZE*4 + data_element_size+provide_port_data_size> expected = {
         'A', 'P', 'X', NODE_HEADER_MAJOR_VERSION, NODE_HEADER_MINOR_VERSION, 'V','M', vm::MAJOR_VERSION, vm::MINOR_VERSION,
         'T', 'e', 's', 't', 'N', 'o', 'd', 'e', '1','\0', static_cast<std::uint8_t>(expected_definition_size),
         0xdc,0xa2,0xf6,0x4b,0x5b,0xaa,0xad,0x06,0xf4,0xd7,0x4a,0x13,0xbe,0xb6,0x58,0x53,
         0x1c,0xa1,0xe2,0xbb,0xda,0x70,0x12,0xf1,0x38,0x0c,0x6f,0x2d,0x64,0xe3,0xc4,0x8f,
         expected_number_of_data_elements, expected_number_of_computation_lists,
         expected_number_of_provide_ports, expected_number_of_require_ports,
         'D', 'C', '(', '0', ',' , '3', ')', '\0',
         'P', 0x01, 0x03, //PROVIDE-PORT INIT-DATA
         'U', 'I', 'n', 't', '8', 'P', 'o', 'r', 't', '\0', 0x00
      };

      const char* apx_text =
         "APX/1.2\n"
         "N\"TestNode1\"\n"
         "P\"UInt8Port\"C(0,3):=3\n";
      apx::NodeManager manager;
      EXPECT_EQ(manager.build_node(apx_text), APX_NO_ERROR);
      EXPECT_EQ(manager.size(), 1u);
      auto* node = manager.get_last_attached();
      EXPECT_NE(node, nullptr);

      FileCache cache;
      std::size_t written_file_size{ 0u };
      cache.set_directory(m_test_dir);
      EXPECT_EQ(cache.store(node), APX_NO_ERROR);
      fs::path const path_to_apx_file = m_test_dir / "TestNode1-64e3c48f.apx";
      EXPECT_TRUE(fs::exists(path_to_apx_file));
      auto ptr = read_buffer_from_file(path_to_apx_file, written_file_size);
      std::uint8_t* buf = ptr.get();
      ASSERT_NE(buf, nullptr);
      ASSERT_EQ(written_file_size, expected_definition_size);
      std::array<char, expected_definition_size + 1> definition_chars;
      std::memset(definition_chars.data(), 0, definition_chars.size());
      std::memcpy(definition_chars.data(), buf, expected_definition_size);
      EXPECT_STREQ(apx_text, definition_chars.data());

      fs::path const path_to_node_file = m_test_dir / "TestNode1-64e3c48f.apxnode";
      EXPECT_TRUE(fs::exists(path_to_node_file));

      ptr = read_buffer_from_file(path_to_node_file, written_file_size);
      buf = ptr.get();
      ASSERT_NE(buf, nullptr);
      std::array<std::uint8_t, expected.size()> read_array;
      std::memset(read_array.data(), 0u, read_array.size());
      ASSERT_EQ(written_file_size, read_array.size());
      std::memcpy(read_array.data(), buf, written_file_size);
      ASSERT_EQ(expected, read_array);
   }
}
