#include "pch.h"
#include <array>
#include "cpp-apx/pack.h"

namespace apx_test
{
   TEST(Pack, PackU8BE)
   {
      std::array<std::uint8_t, sizeof(std::uint8_t)> buf;
      apx::packBE<std::uint8_t>(buf.data(), 0u);
      ASSERT_EQ(buf[0], 0);
      apx::packBE<std::uint8_t>(buf.data(), 0xff);
      ASSERT_EQ(buf[0], 0xff);
   }

   TEST(Pack, UnpackU8BE)
   {
      uint32_t value;
      std::array<std::uint8_t, sizeof(std::uint8_t)> buf;
      buf[0] = 0;
      value = apx::unpackBE<std::uint8_t>(buf.data());
      ASSERT_EQ(value, 0u);
      buf[0] = 0xff;
      value = apx::unpackBE<std::uint8_t>(buf.data());
      ASSERT_EQ(value, 0xffu);
   }

   TEST(Pack, PackU16BE)
   {
      std::array<std::uint8_t, sizeof(std::uint16_t)> buf;
      apx::packBE<std::uint16_t>(buf.data(), 0x0000);
      ASSERT_EQ(buf[0], 0x00);
      ASSERT_EQ(buf[1], 0x00);
      apx::packBE<std::uint16_t>(buf.data(), 0x1234);
      ASSERT_EQ(buf[0], 0x12);
      ASSERT_EQ(buf[1], 0x34);
      apx::packBE<std::uint16_t>(buf.data(), 0xffff);
      ASSERT_EQ(buf[0], 0xff);
      ASSERT_EQ(buf[1], 0xff);
   }

   TEST(Pack, UnpackU16BE)
   {
      uint32_t value;
      std::array<std::uint8_t, sizeof(std::uint16_t)> buf;
      buf[0] = 0;
      buf[1] = 0;
      value = apx::unpackBE<std::uint16_t>(buf.data());
      ASSERT_EQ(value, 0u);
      buf[0] = 0x12;
      buf[1] = 0x34;
      value = apx::unpackBE<std::uint16_t>(buf.data());
      ASSERT_EQ(value, 0x1234u);
      buf[0] = 0xff;
      buf[1] = 0xff;
      value = apx::unpackBE<std::uint16_t>(buf.data());
      ASSERT_EQ(value, 0xffffu);
   }

   TEST(Pack, PackU32BE)
   {
      std::array<std::uint8_t, sizeof(std::uint32_t)> buf;
      apx::packBE<std::uint32_t>(buf.data(), 0x00000000);
      ASSERT_EQ(buf[0], 0x00);
      ASSERT_EQ(buf[1], 0x00);
      ASSERT_EQ(buf[2], 0x00);
      ASSERT_EQ(buf[3], 0x00);
      apx::packBE<std::uint32_t>(buf.data(), 0x12345678);
      ASSERT_EQ(buf[0], 0x12);
      ASSERT_EQ(buf[1], 0x34);
      ASSERT_EQ(buf[2], 0x56);
      ASSERT_EQ(buf[3], 0x78);
      apx::packBE<std::uint32_t>(buf.data(), 0xffffffff);
      ASSERT_EQ(buf[0], 0xff);
      ASSERT_EQ(buf[1], 0xff);
      ASSERT_EQ(buf[2], 0xff);
      ASSERT_EQ(buf[3], 0xff);
   }

   TEST(Pack, UnpackU32BE)
   {
      uint32_t value;
      std::array<std::uint8_t, sizeof(std::uint32_t)> buf;
      buf[0] = 0;
      buf[1] = 0;
      buf[2] = 0;
      buf[3] = 0;
      value = apx::unpackBE<std::uint32_t>(buf.data());
      ASSERT_EQ(value, 0u);
      buf[0] = 0x12;
      buf[1] = 0x34;
      buf[2] = 0x56;
      buf[3] = 0x78;
      value = apx::unpackBE<std::uint32_t>(buf.data());
      ASSERT_EQ(value, 0x12345678u);
      buf[0] = 0xff;
      buf[1] = 0xff;
      buf[2] = 0xff;
      buf[3] = 0xff;
      value = apx::unpackBE<std::uint32_t>(buf.data());
      ASSERT_EQ(value, 0xffffffffu);
   }

   TEST(Pack, PackU8LE)
   {
      std::array<std::uint8_t, sizeof(std::uint8_t)> buf;
      apx::packLE<std::uint8_t>(buf.data(), 0x00);
      ASSERT_EQ(buf[0], 0x00);
      apx::packLE<std::uint8_t>(buf.data(), 0xff);
      ASSERT_EQ(buf[0], 0xff);
   }

   TEST(Pack, UnpackU8LE)
   {
      std::uint32_t value = 0;
      std::array<std::uint8_t, sizeof(std::uint8_t)> buf;
      buf[0] = 0x00;
      value = apx::unpackLE<std::uint8_t>(buf.data());
      ASSERT_EQ(value, 0x00u);
      buf[0] = 0xff;
      value = apx::unpackLE<std::uint8_t>(buf.data());
      ASSERT_EQ(value, 0xffu);
   }


   TEST(Pack, PackU16LE)
   {
      std::array<std::uint8_t, sizeof(std::uint16_t)> buf;
      apx::packLE<std::uint16_t>(buf.data(), 0x0000);
      ASSERT_EQ(buf[0], 0x00u);
      ASSERT_EQ(buf[1], 0x00u);
      apx::packLE<std::uint16_t>(buf.data(), 0x1234);
      ASSERT_EQ(buf[0], 0x34u);
      ASSERT_EQ(buf[1], 0x12u);
      apx::packLE<std::uint16_t>(buf.data(), 0xffff);
      ASSERT_EQ(buf[0], 0xffu);
      ASSERT_EQ(buf[1], 0xffu);
   }

   TEST(Pack, UnpackU16LE)
   {
      uint32_t value;
      std::array<std::uint8_t, sizeof(std::uint16_t)> buf;
      buf[0] = 0;
      buf[1] = 0;
      value = apx::unpackLE<std::uint16_t>(buf.data());
      ASSERT_EQ(value, 0u);
      buf[0] = 0x34;
      buf[1] = 0x12;
      value = apx::unpackLE<std::uint16_t>(buf.data());
      ASSERT_EQ(value, 0x1234u);
      buf[0] = 0xff;
      buf[1] = 0xff;
      value = apx::unpackLE<std::uint16_t>(buf.data());
      ASSERT_EQ(value, 0xffffu);
   }

   TEST(Pack, PackU32LE)
   {
      std::array<std::uint8_t, sizeof(std::uint32_t)> buf;
      apx::packLE<std::uint32_t>(buf.data(), 0x00000000u);
      ASSERT_EQ(buf[0], 0x00u);
      ASSERT_EQ(buf[1], 0x00u);
      ASSERT_EQ(buf[2], 0x00u);
      ASSERT_EQ(buf[3], 0x00u);
      apx::packLE<std::uint32_t>(buf.data(), 0x12345678u);
      ASSERT_EQ(buf[0], 0x78u);
      ASSERT_EQ(buf[1], 0x56u);
      ASSERT_EQ(buf[2], 0x34u);
      ASSERT_EQ(buf[3], 0x12u);
      apx::packLE<std::uint32_t>(buf.data(), 0xffffffffu);
      ASSERT_EQ(buf[0], 0xffu);
      ASSERT_EQ(buf[1], 0xffu);
      ASSERT_EQ(buf[2], 0xffu);
      ASSERT_EQ(buf[3], 0xffu);
   }

   TEST(Pack, UnpackU32LE)
   {
      uint32_t value;
      std::array<std::uint8_t, sizeof(std::uint32_t)> buf;
      buf[0] = 0u;
      buf[1] = 0u;
      buf[2] = 0u;
      buf[3] = 0u;
      value = apx::unpackLE<std::uint32_t>(buf.data());
      ASSERT_EQ(value, 0x000000u);
      buf[0] = 0x78u;
      buf[1] = 0x56u;
      buf[2] = 0x34u;
      buf[3] = 0x12u;
      value = apx::unpackLE<std::uint32_t>(buf.data());
      ASSERT_EQ(value, 0x12345678u);
      buf[0] = 0xffu;
      buf[1] = 0xffu;
      buf[2] = 0xffu;
      buf[3] = 0xffu;
      value = apx::unpackLE<std::uint32_t>(buf.data());
      ASSERT_EQ(value, 0xffffffffu);
   }
}
