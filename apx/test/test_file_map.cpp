#include "pch.h"
#include "cpp-apx/file_map.h"

using namespace apx;
using namespace std::string_literals;
namespace apx_test
{
   TEST(FileMap, AutoAssignLocalAddress)
   {
      FileMap fmap{ false };
      auto const* file1 = fmap.create_file(rmf::FileInfo("TestNode1.out", 10u));
      ASSERT_TRUE(file1);
      EXPECT_EQ(file1->get_address(), 0u);
      auto const* file2 = fmap.create_file(rmf::FileInfo("TestNode2.out", 10u));
      ASSERT_TRUE(file2);
      EXPECT_EQ(file2->get_address(), PORT_DATA_ADDRESS_ALIGNMENT);
      auto const* file3 = fmap.create_file(rmf::FileInfo("TestNode3.out", 10u));
      ASSERT_TRUE(file3);
      EXPECT_EQ(file3->get_address(), PORT_DATA_ADDRESS_ALIGNMENT * 2);
      auto const* file4 = fmap.create_file(rmf::FileInfo("TestNode1.apx", 100u));
      ASSERT_TRUE(file4);
      EXPECT_EQ(file4->get_address(), DEFINITION_ADDRESS_START);
      auto const* file5 = fmap.create_file(rmf::FileInfo("TestNode2.apx", 100u));
      ASSERT_TRUE(file5);
      EXPECT_EQ(file5->get_address(), DEFINITION_ADDRESS_START + DEFINITION_ADDRESS_ALIGNMENT);
      auto const* file6 = fmap.create_file(rmf::FileInfo("TestNode4.out", 10u));
      ASSERT_TRUE(file6);
      EXPECT_EQ(file6->get_address(), PORT_DATA_ADDRESS_ALIGNMENT * 3);
   }

   TEST(FileMap, CreatePortFileAfterDefinitionFile)
   {
      FileMap fmap{ false };
      auto const* file1 = fmap.create_file(rmf::FileInfo("TestNode1.apx", 100u));
      ASSERT_TRUE(file1);
      auto const* file2 = fmap.create_file(rmf::FileInfo("TestNode1.out", 1u));
      ASSERT_TRUE(file2);
      auto& list = fmap.list();
      auto it = list.begin();
      EXPECT_EQ(*it, file2);
      it++;
      EXPECT_EQ(*it, file1);
   }

   TEST(FileMap, AddTooManyFilesOfSameType)
   {
      FileMap fmap{ false };
      auto const* file1 = fmap.create_file(rmf::FileInfo("TestNode1.apx", 10000u));
      ASSERT_TRUE(file1);
      EXPECT_EQ(file1->get_address(), DEFINITION_ADDRESS_START);
      std::size_t const file_size = 1024 * 1024;
      std::uint32_t expected_address = 0u;
      for (int i = 0; i < 64; i++)
      {
         auto const* file = fmap.create_file(rmf::FileInfo("TestNode" + std::to_string(i) + ".out", file_size));
         ASSERT_TRUE(file);
         EXPECT_EQ(file->get_address(), expected_address);
         expected_address += file_size;
      }
      auto const* file2 = fmap.create_file(rmf::FileInfo("TestNode64.out", 1u));
      ASSERT_FALSE(file2);
   }

   TEST(FileMap, FindFileByAddress)
   {
      FileMap fmap{ false };
      auto const* file1 = fmap.create_file(rmf::FileInfo("TestNode1.out", 10u));
      ASSERT_TRUE(file1);
      ASSERT_EQ(file1->get_address(), 0u);
      auto const* file2 = fmap.create_file(rmf::FileInfo("TestNode2.out", 10u));
      ASSERT_TRUE(file2);
      ASSERT_EQ(file2->get_address(), 1024u);
      auto const* file3 = fmap.create_file(rmf::FileInfo("TestNode3.out", 10u));
      ASSERT_TRUE(file3);
      ASSERT_EQ(file3->get_address(), 2048u);
      auto const* file4 = fmap.create_file(rmf::FileInfo("TestNode1.apx", 100u));
      ASSERT_TRUE(file4);
      auto const* file5 = fmap.create_file(rmf::FileInfo("TestNode2.apx", 100u));
      ASSERT_TRUE(file5);
      auto* file = fmap.find_by_address(2048);
      EXPECT_EQ(file, file3);
      file = fmap.find_by_address(2048); //This should used cached value
      EXPECT_EQ(file, file3);
      file = fmap.find_by_address(1024);
      EXPECT_EQ(file, file2);
   }

   TEST(FileMap, FindRemoteFileByLocalAddress)
   {
      FileMap fmap{ true };
      auto const* file1 = fmap.create_file(rmf::FileInfo("TestNode1.out", 10u));
      ASSERT_TRUE(file1);
      ASSERT_EQ(file1->get_address(), rmf::REMOTE_ADDRESS_BIT);
      auto const* file2 = fmap.create_file(rmf::FileInfo("TestNode2.out", 10u));
      ASSERT_TRUE(file2);
      ASSERT_EQ(file2->get_address(), rmf::REMOTE_ADDRESS_BIT | 1024u);
      auto const* file3 = fmap.create_file(rmf::FileInfo("TestNode3.out", 10u));
      ASSERT_TRUE(file3);
      ASSERT_EQ(file3->get_address(), rmf::REMOTE_ADDRESS_BIT | 2048u);
      auto const* file4 = fmap.create_file(rmf::FileInfo("TestNode1.apx", 100u));
      ASSERT_TRUE(file4);
      auto const* file5 = fmap.create_file(rmf::FileInfo("TestNode2.apx", 100u));
      ASSERT_TRUE(file5);
      auto* file = fmap.find_by_address(2048);
      EXPECT_EQ(file, file3);
      file = fmap.find_by_address(1024);
      EXPECT_EQ(file, file2);
   }

   TEST(FileMap, FindFileByName)
   {
      FileMap fmap{ false };
      auto const* file1 = fmap.create_file(rmf::FileInfo("TestNode1.out", 10u));
      ASSERT_TRUE(file1);
      ASSERT_EQ(file1->get_address(), 0u);
      auto const* file2 = fmap.create_file(rmf::FileInfo("TestNode2.out", 10u));
      ASSERT_TRUE(file2);
      ASSERT_EQ(file2->get_address(), 1024u);
      auto const* file3 = fmap.create_file(rmf::FileInfo("TestNode3.out", 10u));
      ASSERT_TRUE(file3);
      ASSERT_EQ(file3->get_address(), 2048u);
      auto const* file4 = fmap.create_file(rmf::FileInfo("TestNode1.apx", 100u));
      ASSERT_TRUE(file4);
      auto const* file5 = fmap.create_file(rmf::FileInfo("TestNode2.apx", 100u));
      ASSERT_TRUE(file5);
      auto* file = fmap.find_by_name("TestNode1.apx");
      EXPECT_EQ(file, file4);
      file = fmap.find_by_name("TestNode1.apx"); //Use cached value
      EXPECT_EQ(file, file4);
      file = fmap.find_by_name("TestNode1.out"); //Use cached value
      EXPECT_EQ(file, file1);
   }

}