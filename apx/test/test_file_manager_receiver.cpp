#include "pch.h"
#include <array>
#include "cpp-apx/file_manager_receiver.h"

using namespace apx;
using namespace std::string_literals;
namespace apx_test
{
   TEST(FileManagerReceiver, CommandAreaSizeOnCreation)
   {
      FileManagerReceiver receiver;
      EXPECT_EQ(receiver.buffer_size(), rmf::CMD_AREA_SIZE);
   }

   TEST(FileManagerReceiver, SmallSizeWrite)
   {
      FileManagerReceiver receiver;
      std::array<std::uint8_t, 4> msg{ 0x12, 0x034, 0x56, 0x78 };
      auto [result, is_complete, address, data, size] = receiver.write(0u, msg.data(), msg.size(), false);
      EXPECT_EQ(result, APX_NO_ERROR);
      EXPECT_TRUE(is_complete);
      EXPECT_EQ(address, 0u);
      EXPECT_TRUE(data);
      EXPECT_EQ(size, msg.size());
      std::array<std::uint8_t, msg.size()> buf;
      std::memcpy(buf.data(), data, buf.size());
      EXPECT_EQ(msg, buf);
   }

   TEST(FileManagerReceiver, MediumSizeWrite)
   {
      FileManagerReceiver receiver;
      std::uint32_t const write_address = 0x10000;
      std::uint32_t const write_offset = 0u;
      std::array<std::uint8_t, 64> msg;
      for (std::size_t i = 0u; i < msg.size(); i++)
      {
         msg[i] = static_cast<std::uint8_t>(i);
      }
      auto [result, is_complete, address, data, size] = receiver.write(write_address + write_offset, msg.data() + write_offset, msg.size(), false);
      EXPECT_EQ(result, APX_NO_ERROR);
      EXPECT_TRUE(is_complete);
      EXPECT_EQ(address, write_address);
      EXPECT_TRUE(data);
      EXPECT_EQ(size, msg.size());
      std::array<std::uint8_t, msg.size()> buf;
      std::memcpy(buf.data(), data, buf.size());
      EXPECT_EQ(msg, buf);
   }

   TEST(FileManagerReceiver, OneByteFragmentedWrite)
   {
      FileManagerReceiver receiver;
      std::uint32_t const write_address = 0x10000;
      std::uint32_t write_offset = 0u;
      std::array<std::uint8_t, 128> msg;
      for (std::size_t i = 0u; i < msg.size(); i++)
      {
         msg[i] = static_cast<std::uint8_t>(i);
      }
      for (std::uint32_t i = 0u; i < 127; i++)
      {
         auto [result, is_complete, address, data, size] = receiver.write(write_address + write_offset, msg.data() + write_offset, sizeof(std::uint8_t), true);
         EXPECT_EQ(result, APX_NO_ERROR);
         EXPECT_FALSE(is_complete);
         write_offset++;
      }
      auto [result, is_complete, address, data, size] = receiver.write(write_address + write_offset, msg.data() + write_offset, sizeof(std::uint8_t), false);
      EXPECT_EQ(result, APX_NO_ERROR);
      EXPECT_TRUE(is_complete);
      EXPECT_EQ(address, write_address);
      EXPECT_TRUE(data);
      EXPECT_EQ(size, msg.size());
      std::array<std::uint8_t, msg.size()> buf;
      std::memcpy(buf.data(), data, buf.size());
      EXPECT_EQ(msg, buf);
   }

   TEST(FileManagerReceiver, ThreePieceMessageFollowedByTwoPieceMessage)
   {
      FileManagerReceiver receiver;
      std::uint32_t write_address = 0x10000;
      std::uint32_t write_offset = 0u;
      constexpr std::size_t write_size1 = 17;
      constexpr std::size_t write_size2 = 33;
      constexpr std::size_t write_size3 = 14;

      std::array<std::uint8_t, write_size1 + write_size2 + write_size3> msg;
      for (std::size_t i = 0u; i < msg.size(); i++)
      {
         msg[i] = static_cast<std::uint8_t>(i);
      }
      auto result = receiver.write(write_address + write_offset, msg.data() + write_offset, write_size1, true);
      EXPECT_EQ(result.error, APX_NO_ERROR);
      EXPECT_FALSE(result.is_complete);
      write_offset += write_size1;
      result = receiver.write(write_address + write_offset, msg.data() + write_offset, write_size2, true);
      EXPECT_EQ(result.error, APX_NO_ERROR);
      EXPECT_FALSE(result.is_complete);
      write_offset += write_size2;
      result = receiver.write(write_address + write_offset, msg.data() + write_offset, write_size3, false);
      EXPECT_EQ(result.error, APX_NO_ERROR);
      EXPECT_TRUE(result.is_complete);
      EXPECT_EQ(result.address, write_address);
      EXPECT_TRUE(result.data);
      EXPECT_EQ(result.size, msg.size());
      std::array<std::uint8_t, msg.size()> buf;
      std::memcpy(buf.data(), result.data, buf.size());
      EXPECT_EQ(msg, buf);

      constexpr std::size_t write_size4 = 17;
      constexpr std::size_t write_size5 = 33;
      write_address = 0x20000;
      write_offset = 0u;
      std::array<std::uint8_t, write_size4 + write_size5> msg2;
      for (std::size_t i = 0u; i < msg2.size(); i++)
      {
         msg2[i] = static_cast<std::uint8_t>(i);
      }
      result = receiver.write(write_address + write_offset, msg2.data() + write_offset, write_size4, true);
      EXPECT_EQ(result.error, APX_NO_ERROR);
      EXPECT_FALSE(result.is_complete);
      write_offset += write_size4;
      result = receiver.write(write_address + write_offset, msg2.data() + write_offset, write_size5, false);
      EXPECT_EQ(result.error, APX_NO_ERROR);
      EXPECT_TRUE(result.is_complete);
      EXPECT_EQ(result.address, write_address);
      EXPECT_TRUE(result.data);
      EXPECT_EQ(result.size, msg2.size());
      std::array<std::uint8_t, msg2.size()> buf2;
      std::memcpy(buf2.data(), result.data, buf2.size());
      EXPECT_EQ(msg2, buf2);
   }

   TEST(FileManagerReceiver, FragmentedWriteAtWrongAddress)
   {
      FileManagerReceiver receiver;
      constexpr std::size_t write_size1 = 15;
      constexpr std::size_t write_size2 = 15;
      std::uint32_t write_address = 0x10000;
      std::array<std::uint8_t, write_size1 + write_size2> msg;
      for (std::size_t i = 0u; i < msg.size(); i++)
      {
         msg[i] = static_cast<std::uint8_t>(i);
      }
      auto result = receiver.write(write_address, msg.data(), write_size1, true);
      EXPECT_EQ(result.error, APX_NO_ERROR);
      EXPECT_FALSE(result.is_complete);
      result = receiver.write(write_address, msg.data() + write_size1, write_size2, false);
      EXPECT_EQ(result.error, APX_INVALID_ADDRESS_ERROR);
   }
}
