#include "pch.h"
#include <string>
#include "cpp-apx/file_info.h"

using namespace rmf;
using namespace std::string_literals;
namespace apx_test
{
   TEST(FileInfo, CreateLocalFile)
   {
      FileInfo file("TestNode.apx", 40);
      EXPECT_EQ(file.name, "TestNode.apx"s);
      EXPECT_EQ(file.rmf_file_type, rmf::FileType::Fixed);
      EXPECT_EQ(file.size, 40u);
      EXPECT_EQ(file.address, rmf::INVALID_ADDRESS);
      EXPECT_EQ(file.digest_type, rmf::DigestType::None);
   }

   TEST(FileInfo, NameEndsWith)
   {
      FileInfo file1("TestNode.apx", 40);
      EXPECT_TRUE(file1.name_ends_with(".apx"));
      EXPECT_FALSE(file1.name_ends_with(".out"));
      EXPECT_FALSE(file1.name_ends_with(".in"));

      FileInfo file2("TestNode.out", 1);
      EXPECT_FALSE(file2.name_ends_with(".apx"));
      EXPECT_TRUE(file2.name_ends_with(".out"));
      EXPECT_FALSE(file2.name_ends_with(".in"));

      FileInfo file3("TestNode.in", 1);
      EXPECT_FALSE(file3.name_ends_with(".apx"));
      EXPECT_FALSE(file3.name_ends_with(".out"));
      EXPECT_TRUE(file3.name_ends_with(".in"));
   }

   TEST(FileInfo, BaseName)
   {
      FileInfo file1("TestNode.apx", 40);
      FileInfo file2("TestNode.out", 1);
      FileInfo file3("TestNode.in", 1);
      EXPECT_EQ(file1.base_name(), "TestNode"s);
   }
}