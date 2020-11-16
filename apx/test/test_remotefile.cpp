#include "pch.h"
#include <array>
#include "cpp-apx/remotefile.h"

namespace apx_test
{
   TEST(RemoteFileProtocol, LowAddressEncode)
   {
      std::array<std::uint8_t, sizeof(std::uint16_t)> buffer;
      std::array<std::uint8_t, sizeof(std::uint16_t)> expected = { 0, 0 };
      EXPECT_EQ(rmf::address_encode(buffer.data(), buffer.size(), 0u, false), buffer.size());
      EXPECT_EQ(buffer, expected);
      EXPECT_EQ(rmf::address_encode(buffer.data(), buffer.size(), 0u, true), buffer.size());
      expected[0] = 0x40;
      EXPECT_EQ(buffer, expected);
      EXPECT_EQ(rmf::address_encode(buffer.data(), buffer.size(), rmf::LOW_ADDR_MAX, false), buffer.size());
      expected[0] = 0x3F;
      expected[1] = 0xFF;
      EXPECT_EQ(buffer, expected);
      EXPECT_EQ(rmf::address_encode(buffer.data(), buffer.size(), rmf::LOW_ADDR_MAX, true), buffer.size());
      expected[0] = 0x7F;
      EXPECT_EQ(buffer, expected);
   }

   TEST(RemoteFileProtocol, LowAddressDecode)
   {
      std::array<std::uint8_t, sizeof(std::uint16_t)> buffer = { 0, 0 };
      std::uint32_t address{ 0u };
      bool more_bit{ false };
      buffer[0] = 0u;
      EXPECT_EQ(rmf::address_decode(buffer.data(), buffer.data() + buffer.size(), address, more_bit), buffer.size());
      EXPECT_EQ(address, 0u);
      EXPECT_EQ(more_bit, false);
      buffer[0] = 0x40;
      EXPECT_EQ(rmf::address_decode(buffer.data(), buffer.data() + buffer.size(), address, more_bit), buffer.size());
      EXPECT_EQ(address, 0u);
      EXPECT_EQ(more_bit, true);
      buffer[0] = 0x3F;
      buffer[1] = 0xFF;
      EXPECT_EQ(rmf::address_decode(buffer.data(), buffer.data() + buffer.size(), address, more_bit), buffer.size());
      EXPECT_EQ(address, rmf::LOW_ADDR_MAX);
      EXPECT_EQ(more_bit, false);
      buffer[0] = 0x7F;
      EXPECT_EQ(rmf::address_decode(buffer.data(), buffer.data() + buffer.size(), address, more_bit), buffer.size());
      EXPECT_EQ(address, rmf::LOW_ADDR_MAX);
      EXPECT_EQ(more_bit, true);
   }

   TEST(RemoteFileProtocol, HighAddressEncode)
   {
      std::array<std::uint8_t, sizeof(std::uint32_t)> buffer;
      std::array<std::uint8_t, sizeof(std::uint32_t)> expected = { 0x80u, 0x0u, 0x40u , 0x0u };
      EXPECT_EQ(rmf::address_encode(buffer.data(), buffer.size(), rmf::HIGH_ADDR_MIN, false), buffer.size());
      EXPECT_EQ(buffer, expected);
      EXPECT_EQ(rmf::address_encode(buffer.data(), buffer.size(), rmf::HIGH_ADDR_MIN, true), buffer.size());
      expected[0] = 0xC0;
      EXPECT_EQ(buffer, expected);
      EXPECT_EQ(rmf::address_encode(buffer.data(), buffer.size(), rmf::HIGH_ADDR_MAX, false), buffer.size());
      expected[0] = 0xBF;
      expected[1] = 0xFF;
      expected[2] = 0xFF;
      expected[3] = 0xFF;
      EXPECT_EQ(buffer, expected);
      EXPECT_EQ(rmf::address_encode(buffer.data(), buffer.size(), rmf::HIGH_ADDR_MAX, true), buffer.size());
      expected[0] = 0xFF;
      EXPECT_EQ(buffer, expected);
   }

   TEST(RemoteFileProtocol, HighAddressDecode)
   {
      std::array<std::uint8_t, sizeof(std::uint32_t)> buffer = { 0x80u, 0x0u, 0x40u , 0x0u };
      std::uint32_t address{ 0u };
      bool more_bit{ false };
      EXPECT_EQ(rmf::address_decode(buffer.data(), buffer.data() + buffer.size(), address, more_bit), buffer.size());
      EXPECT_EQ(address, rmf::HIGH_ADDR_MIN);
      EXPECT_EQ(more_bit, false);
      buffer[0] = 0xC0;
      EXPECT_EQ(rmf::address_decode(buffer.data(), buffer.data() + buffer.size(), address, more_bit), buffer.size());
      EXPECT_EQ(address, rmf::HIGH_ADDR_MIN);
      EXPECT_EQ(more_bit, true);
      buffer[0] = 0xBF;
      buffer[1] = 0xFF;
      buffer[2] = 0xFF;
      buffer[3] = 0xFF;
      EXPECT_EQ(rmf::address_decode(buffer.data(), buffer.data() + buffer.size(), address, more_bit), buffer.size());
      EXPECT_EQ(address, rmf::HIGH_ADDR_MAX);
      EXPECT_EQ(more_bit, false);
      buffer[0] = 0xFF;
      EXPECT_EQ(rmf::address_decode(buffer.data(), buffer.data() + buffer.size(), address, more_bit), buffer.size());
      EXPECT_EQ(address, rmf::HIGH_ADDR_MAX);
      EXPECT_EQ(more_bit, true);
   }

}