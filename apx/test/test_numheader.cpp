#include "pch.h"
#include <cstring>
#include <array>
#include "cpp-apx/numheader.h"

namespace apx_test
{
   TEST(NumHeader16, ShortLowerLimit)
   {
      std::array<std::uint8_t, sizeof(uint8_t)> buf;
      std::array < std::uint8_t, buf.size()> expected = { 0u };
      std::uint16_t const value = 0u;
      EXPECT_EQ(numheader::encode16(buf.data(), buf.data() + buf.size(), value), numheader::SHORT_SIZE);
      EXPECT_EQ(buf, expected);
   }

   TEST(NumHeader16, ShortUpperLimit)
   {
      std::array<std::uint8_t, sizeof(uint8_t)> buf;
      std::array < std::uint8_t, buf.size()> expected = { 127u };
      std::uint16_t const value = 127u;
      EXPECT_EQ(numheader::encode16(buf.data(), buf.data() + buf.size(), value), numheader::SHORT_SIZE);
      EXPECT_EQ(buf, expected);
   }

   TEST(NumHeader16, LongLowerLimit)
   {
      std::array<std::uint8_t, sizeof(uint16_t)> buf;
      std::array < std::uint8_t, buf.size()> expected = { 0x80u, 0x00u };
      std::uint16_t const value = 128u;
      EXPECT_EQ(numheader::encode16(buf.data(), buf.data() + buf.size(), value), numheader::LONG16_SIZE);
      EXPECT_EQ(buf, expected);
   }

   TEST(NumHeader16, LongUpperLimit)
   {
      std::array<std::uint8_t, sizeof(uint32_t)> buf;
      std::array < std::uint8_t, buf.size()> expected = { 0xffu, 0xff };
      std::uint16_t const value = numheader::LONG16_MAX;
      EXPECT_EQ(numheader::encode16(buf.data(), buf.data() + buf.size(), value), numheader::LONG16_SIZE);
      EXPECT_EQ(buf, expected);
   }
   TEST(NumHeader32, ShortLowerLimit)
   {
      std::array<std::uint8_t, sizeof(uint8_t)> buf;
      std::array < std::uint8_t, buf.size()> expected = { 0u };
      std::uint32_t const value = 0u;
      EXPECT_EQ(numheader::encode32(buf.data(), buf.data() + buf.size(), value), numheader::SHORT_SIZE);
      EXPECT_EQ(buf, expected);
   }

   TEST(NumHeader32, ShortUpperLimit)
   {
      std::array<std::uint8_t, sizeof(uint8_t)> buf;
      std::array < std::uint8_t, buf.size()> expected = { 127u };
      std::uint32_t const value = 127u;
      EXPECT_EQ(numheader::encode32(buf.data(), buf.data() + buf.size(), value), numheader::SHORT_SIZE);
      EXPECT_EQ(buf, expected);
   }

   TEST(NumHeader32, LongLowerLimit)
   {
      std::array<std::uint8_t, sizeof(uint32_t)> buf;
      std::array < std::uint8_t, buf.size()> expected = { 0x80u, 0x00u, 0x00u, 0x80u };
      std::uint32_t const value = 128u;
      EXPECT_EQ(numheader::encode32(buf.data(), buf.data() + buf.size(), value), numheader::LONG32_SIZE);
      EXPECT_EQ(buf, expected);
   }

   TEST(NumHeader32, LongUpperLimit)
   {
      std::array<std::uint8_t, sizeof(uint32_t)> buf;
      std::array < std::uint8_t, buf.size()> expected = { 0xffu, 0xff, 0xff, 0xff };
      std::uint32_t const value = INT32_MAX;
      EXPECT_EQ(numheader::encode32(buf.data(), buf.data() + buf.size(), value), numheader::LONG32_SIZE);
      EXPECT_EQ(buf, expected);
   }

}