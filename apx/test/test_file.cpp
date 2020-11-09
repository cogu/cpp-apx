#include "pch.h"

#include <array>
#include <string>
#include <vector>
#include <algorithm>
#include "cpp-apx/file.h"

using namespace std::string_literals;
using namespace apx;

namespace apx_test
{
   TEST(File, FileTypeToExtension)
   {
      EXPECT_EQ(file_type_to_extension(apx::FileType::Definition), ".apx"s);
      EXPECT_EQ(file_type_to_extension(apx::FileType::ProvidePortData), ".out"s);
      EXPECT_EQ(file_type_to_extension(apx::FileType::RequirePortData), ".in"s);
      EXPECT_EQ(file_type_to_extension(apx::FileType::ProvidePortCount), ".cout"s);
      EXPECT_EQ(file_type_to_extension(apx::FileType::RequirePortCount), ".cin"s);
   }

   TEST(File, AutoDetectFileTypeFromName)
   {
      File file1(rmf::FileInfo("TestNode.apx", 40u));
      EXPECT_EQ(file1.get_apx_file_type(), FileType::Definition);
      File file2(rmf::FileInfo("TestNode.out", 1u));
      EXPECT_EQ(file2.get_apx_file_type(), FileType::ProvidePortData);
      File file3(rmf::FileInfo("TestNode.in", 2u));
      EXPECT_EQ(file3.get_apx_file_type(), FileType::RequirePortData);
      File file4(rmf::FileInfo("TestNode.cout", 1u));
      EXPECT_EQ(file4.get_apx_file_type(), FileType::ProvidePortCount);
      File file5(rmf::FileInfo("TestNode.cin", 1u));
      EXPECT_EQ(file5.get_apx_file_type(), FileType::RequirePortCount);
   }

   TEST(File, LocalOrRemoteFile)
   {
      File file1(rmf::FileInfo("TestNode.out", 1));
      EXPECT_FALSE(file1.is_local());
      EXPECT_FALSE(file1.is_remote());
      EXPECT_FALSE(file1.has_valid_address());

      File file2(rmf::FileInfo("TestNode.out", 1, 0x1000u));
      EXPECT_TRUE(file2.is_local());
      EXPECT_FALSE(file2.is_remote());
      EXPECT_TRUE(file2.has_valid_address());

      File file3(rmf::FileInfo("TestNode.in", 1, 0x1000u | rmf::REMOTE_ADDRESS_BIT));
      EXPECT_FALSE(file3.is_local());
      EXPECT_TRUE(file3.is_remote());
      EXPECT_TRUE(file3.has_valid_address());
   }

   TEST(File, DigestDataIsCopiedBetweenFiles)
   {
      std::array<std::uint8_t, rmf::SHA256_SIZE> digest;
      for (std::size_t i = 0; i < digest.size(); i++)
      {
         digest[i] = static_cast<std::uint8_t>(i);
      }
      rmf::FileInfo info("TestNode.apx", 40, 1000u, rmf::DigestType::SHA256, digest.data());
      File file1(info);
      auto const* digest_data = file1.get_digest_data();
      EXPECT_EQ(digest_data[1], 1u);
      EXPECT_EQ(digest_data[15], 15u);
      EXPECT_EQ(digest_data[31], 31u);

      File file2(file1);
      digest_data = file2.get_digest_data();
      EXPECT_EQ(digest_data[1], 1u);
      EXPECT_EQ(digest_data[15], 15u);
      EXPECT_EQ(digest_data[31], 31u);
   }

   TEST(File, SortFiles)
   {
      File file1(rmf::FileInfo("File1.apx", 100u, 0x1000));
      File file2(rmf::FileInfo("File2.apx", 100u, 0x800));
      File file3(rmf::FileInfo("File3.apx", 100u, 0x2000));
      File file4(rmf::FileInfo("File4.apx", 100u, 0x400));

      std::vector<File> files;
      files.push_back(file1);
      files.push_back(file2);
      files.push_back(file3);
      files.push_back(file4);
      std::sort(files.begin(), files.end(), File::less_than);

      EXPECT_EQ(files[0].get_name(), "File4.apx"s);
      EXPECT_EQ(files[1].get_name(), "File2.apx"s);
      EXPECT_EQ(files[2].get_name(), "File1.apx"s);
      EXPECT_EQ(files[3].get_name(), "File3.apx"s);
   }
}