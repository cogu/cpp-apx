#include "pch.h"
#include "cpp-apx/file_manager_shared.h"

using namespace apx;
using namespace std::string_literals;
namespace apx_test
{
   TEST(FileManagerShared, FindLocalFileByName)
   {
      FileManagerShared shared;
      auto *file1 = shared.create_local_file(rmf::FileInfo("TestNode.apx", 200u));
      ASSERT_TRUE(file1);
      EXPECT_EQ(file1->get_address(), DEFINITION_ADDRESS_START);
      auto* file2 = shared.create_local_file(rmf::FileInfo("TestNode.out", 4u));
      ASSERT_TRUE(file2);
      EXPECT_EQ(file2->get_address(), 0u);
      auto* file = shared.find_local_file_by_name("TestNode.apx");
      ASSERT_TRUE(file);
      EXPECT_EQ(file, file1);
   }

   TEST(FileManagerShared, FindRemoteFileByName)
   {
      FileManagerShared shared;
      auto* file1 = shared.create_remote_file(rmf::FileInfo("TestNode1.in", 6u, 0u));
      ASSERT_TRUE(file1);
      EXPECT_EQ(file1->get_address(), rmf::REMOTE_ADDRESS_BIT);
      auto* file2 = shared.create_remote_file(rmf::FileInfo("TestNode2.in", 4u, PORT_DATA_ADDRESS_ALIGNMENT));
      ASSERT_TRUE(file2);
      EXPECT_EQ(file2->get_address(), rmf::REMOTE_ADDRESS_BIT | PORT_DATA_ADDRESS_ALIGNMENT);
      auto* file = shared.find_remote_file_by_name("TestNode2.in");
      ASSERT_TRUE(file);
      EXPECT_EQ(file, file2);
   }

   TEST(FileManagerShared, FindLocalFileByAddress)
   {
      FileManagerShared shared;
      auto* file1 = shared.create_local_file(rmf::FileInfo("TestNode.apx", 200u));
      ASSERT_TRUE(file1);
      EXPECT_EQ(file1->get_address(), DEFINITION_ADDRESS_START);
      auto* file2 = shared.create_local_file(rmf::FileInfo("TestNode.out", 4u));
      ASSERT_TRUE(file2);
      EXPECT_EQ(file2->get_address(), 0u);
      auto* file = shared.find_file_by_address(0u);
      ASSERT_TRUE(file);
      EXPECT_EQ(file, file2);
      file = shared.find_file_by_address(2u);
      ASSERT_TRUE(file);
      EXPECT_EQ(file, file2);
   }

   TEST(FileManagerShared, FindRemoteFileByAddress)
   {
      FileManagerShared shared;
      auto* file1 = shared.create_remote_file(rmf::FileInfo("TestNode1.in", 6u, 0u));
      ASSERT_TRUE(file1);
      EXPECT_EQ(file1->get_address(), rmf::REMOTE_ADDRESS_BIT);
      auto* file2 = shared.create_remote_file(rmf::FileInfo("TestNode2.in", 4u, PORT_DATA_ADDRESS_ALIGNMENT));
      ASSERT_TRUE(file2);
      EXPECT_EQ(file2->get_address(), rmf::REMOTE_ADDRESS_BIT | PORT_DATA_ADDRESS_ALIGNMENT);
      auto* file = shared.find_file_by_address(rmf::REMOTE_ADDRESS_BIT);
      ASSERT_TRUE(file);
      EXPECT_EQ(file, file1);
      file = shared.find_file_by_address(rmf::REMOTE_ADDRESS_BIT | 5u);
      ASSERT_TRUE(file);
      EXPECT_EQ(file, file1);
      file = shared.find_file_by_address(rmf::REMOTE_ADDRESS_BIT | PORT_DATA_ADDRESS_ALIGNMENT);
      ASSERT_TRUE(file);
      EXPECT_EQ(file, file2);
      file = shared.find_file_by_address(rmf::REMOTE_ADDRESS_BIT | (PORT_DATA_ADDRESS_ALIGNMENT+3) );
      ASSERT_TRUE(file);
      EXPECT_EQ(file, file2);
   }

}
