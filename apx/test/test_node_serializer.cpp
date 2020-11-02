#include "pch.h"
#include <array>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstring>
#include "cpp-apx/node_serializer.h"
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

   class NodeSerializerTest : public ::testing::Test
   {
   protected:
      fs::path m_test_dir;

      NodeSerializerTest()
      {
         m_test_dir = fs::temp_directory_path() / "apx_test";
         fs::create_directory(m_test_dir);
      }

      ~NodeSerializerTest()
      {
         for (auto it = fs::directory_iterator(m_test_dir); it != fs::directory_iterator(); ++it)
         {
            fs::remove(*it);
         }
      }
   };

   TEST_F(NodeSerializerTest, NodeWithoutPorts)
   {
      std::array<std::uint8_t, 18u> expected = { 'A', 'P', 'X', NODE_HEADER_MAJOR_VERSION, NODE_HEADER_MINOR_VERSION,
         'V','M', vm::MAJOR_VERSION, vm::MINOR_VERSION, 
         'T', 'e', 's', 't', 'N', 'o', 'd', 'e', '\0',
      };
      std::array<std::uint8_t, expected.size()> written;
      std::memset(written.data(), 0u, written.size());

      const char* apx_text =
         "APX/1.2\n"
         "N\"TestNode\"\n";
      apx::NodeManager manager;
      EXPECT_EQ(manager.build_node(apx_text), APX_NO_ERROR);
      EXPECT_EQ(manager.size(), 1u);
      auto* node = manager.get_last_attached();
      EXPECT_NE(node, nullptr);

      NodeSerializer serializer;
      serializer.set_directory(m_test_dir);
      EXPECT_EQ(serializer.save_node(node), APX_NO_ERROR);
      fs::path const path_to_file = m_test_dir / "TestNode.apxc";
      EXPECT_TRUE(fs::exists(path_to_file));
      std::size_t written_size{ 0u };
      auto ptr = read_buffer_from_file(path_to_file, written_size);
      std::uint8_t* buf = ptr.get();
      ASSERT_NE(buf, nullptr);
      ASSERT_EQ(written_size, written.size());
      std::memcpy(written.data(), buf, written_size);
      ASSERT_EQ(expected, written);
   }
}
