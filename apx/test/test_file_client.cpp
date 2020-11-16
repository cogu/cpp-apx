#include "pch.h"
#include <array>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <cassert>
#include <memory>
#include "cpp-apx/file_client.h"

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

   static bool write_buffer_to_file(fs::path const& path_to_file, std::uint8_t const* data, std::size_t size)
   {
      if ((data != nullptr) && (size > 0u))
      {
         std::ofstream file(path_to_file, std::ios::out | std::ios::binary | std::ios::trunc);
         if (file.is_open())
         {
            file.write(reinterpret_cast<char const*>(data), size);
            return true;
         }
      }
      return false;
   }

   class FileClientTest : public ::testing::Test
   {
   protected:
      fs::path m_test_dir;

      FileClientTest()
      {
         m_test_dir = fs::temp_directory_path() / "apx_test";
         fs::create_directory(m_test_dir);
      }

      ~FileClientTest()
      {
         for (auto it = fs::directory_iterator(m_test_dir); it != fs::directory_iterator(); ++it)
         {
            fs::remove(*it);
         }
      }
   };

   TEST_F(FileClientTest, SaveInitDataToFile_UInt8ProvidePort)
   {
      char const* apx_text = "APX/1.2\n"
         "N\"TestNode\"\n"
         "P\"U8Port\"C(0,7):=7\n";
      apx::FileClient client(m_test_dir);
      ASSERT_EQ(client.build_node(apx_text), APX_NO_ERROR);
      ASSERT_TRUE(client.has_provide_port_data());
      ASSERT_FALSE(client.has_require_port_data());
      ASSERT_EQ(client.save_all(), APX_NO_ERROR);
      auto path_to_file = m_test_dir / "TestNode.out";
      ASSERT_TRUE(fs::exists(path_to_file));
      std::size_t size{ 0u };
      auto ptr = read_buffer_from_file(path_to_file, size);
      std::uint8_t* buf = ptr.get();
      ASSERT_NE(buf, nullptr);
      ASSERT_EQ(size, sizeof(std::uint8_t));
      ASSERT_EQ(buf[0], 7u);
   }

   TEST_F(FileClientTest, SaveInitDataToFile_UInt8RequirePort)
   {
      char const* apx_text = "APX/1.2\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"C(0,7):=7\n";
      apx::FileClient client(m_test_dir);
      ASSERT_EQ(client.build_node(apx_text), APX_NO_ERROR);
      ASSERT_FALSE(client.has_provide_port_data());
      ASSERT_TRUE(client.has_require_port_data());
      ASSERT_EQ(client.save_all(), APX_NO_ERROR);
      auto path_to_file = m_test_dir / "TestNode.in";
      ASSERT_TRUE(fs::exists(path_to_file));
      std::size_t size{ 0u };
      auto ptr = read_buffer_from_file(path_to_file, size);
      std::uint8_t* buf = ptr.get();
      ASSERT_NE(buf, nullptr);
      ASSERT_EQ(size, sizeof(std::uint8_t));
      ASSERT_EQ(buf[0], 7u);
   }

   TEST_F(FileClientTest, LoadRequirePortDataFromFile_UInt8)
   {
      std::array<std::uint8_t, sizeof(std::uint8_t)> stored{ 1u };
      auto path_to_file = m_test_dir / "TestNode.in";
      ASSERT_TRUE(write_buffer_to_file(path_to_file, stored.data(), stored.size()));
      std::array<std::uint8_t, sizeof(std::uint8_t)> buf;
      std::memset(buf.data(), 0, buf.size());

      char const* apx_text = "APX/1.2\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"C(0,7):=7\n";

      apx::FileClient client(m_test_dir);
      ASSERT_EQ(client.build_node(apx_text), APX_NO_ERROR);
      ASSERT_FALSE(client.has_provide_port_data());
      ASSERT_TRUE(client.has_require_port_data());
      auto node_data = client.get_node_data();
      ASSERT_EQ(node_data->require_port_data_size(), buf.size());
      ASSERT_EQ(node_data->read_require_port_data(0u, buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(buf[0], stored[0]);
   }

   TEST_F(FileClientTest, LoadProvidePortDataFromFile_UInt8)
   {
      std::array<std::uint8_t, sizeof(std::uint8_t)> stored{ 1u };
      auto path_to_file = m_test_dir / "TestNode.out";
      ASSERT_TRUE(write_buffer_to_file(path_to_file, stored.data(), stored.size()));
      std::array<std::uint8_t, sizeof(std::uint8_t)> buf;
      std::memset(buf.data(), 0, buf.size());

      char const* apx_text = "APX/1.2\n"
         "N\"TestNode\"\n"
         "P\"U8Port\"C(0,7):=7\n";

      apx::FileClient client(m_test_dir);
      ASSERT_EQ(client.build_node(apx_text), APX_NO_ERROR);
      ASSERT_TRUE(client.has_provide_port_data());
      ASSERT_FALSE(client.has_require_port_data());
      auto node_data = client.get_node_data();
      ASSERT_EQ(node_data->provide_port_data_size(), buf.size());
      ASSERT_EQ(node_data->read_provide_port_data(0u, buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(buf[0], stored[0]);
   }

}
