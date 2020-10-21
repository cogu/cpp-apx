#include "pch.h"
#include "cpp-apx/deserializer.h"
#include <array>

using namespace apx::vm;

namespace apx_test
{
   TEST(Deserializer, SetInvalidBuffer)
   {
      std::array<std::uint8_t, 1> buf;
      Deserializer deserializer;
      ASSERT_EQ(deserializer.set_read_buffer(nullptr, 1), APX_INVALID_ARGUMENT_ERROR);
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), 0), APX_INVALID_ARGUMENT_ERROR);
   }

   TEST(Deserializer, SetValidBuffer)
   {
      std::array<std::uint8_t, 1> buf;
      Deserializer deserializer;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
   }

   TEST(Deserializer, UnpackUInt8)
   {
      std::array<std::uint8_t, UINT8_SIZE> buf = { 0 };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type() , dtl::ValueType::Scalar);
      auto sv = deserializer.take_sv();
      EXPECT_NE(sv.get(), nullptr);
      EXPECT_EQ(sv->to_u32(ok), 0u);
      EXPECT_TRUE(ok);

      buf[0] = 0x12;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      sv = deserializer.take_sv();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_u32(ok), 0x12u);
      EXPECT_TRUE(ok);

      buf[0] = 0xff;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      sv = deserializer.take_sv();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_u32(ok), 0xffu);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackUint16)
   {
      std::array<std::uint8_t, UINT16_SIZE> buf = { 0u, 0u };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint16(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      auto sv = deserializer.take_sv();
      EXPECT_NE(sv.get(), nullptr);
      EXPECT_EQ(sv->to_u32(ok), 0u);
      EXPECT_TRUE(ok);

      buf[0] = 0x34;
      buf[1] = 0x12;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint16(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      sv = deserializer.take_sv();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_u32(ok), 0x1234u);
      EXPECT_TRUE(ok);

      buf[0] = 0xff;
      buf[1] = 0xff;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint16(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      sv = deserializer.take_sv();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_u32(ok), 0xffffu);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackUint32)
   {
      std::array<std::uint8_t, UINT32_SIZE> buf = { 0u, 0u, 0u, 0u };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint32(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      auto sv = deserializer.take_sv();
      EXPECT_NE(sv.get(), nullptr);
      EXPECT_EQ(sv->to_u32(ok), 0u);
      EXPECT_TRUE(ok);

      buf[0] = 0x78;
      buf[1] = 0x56;
      buf[2] = 0x34;
      buf[3] = 0x12;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint32(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      sv = deserializer.take_sv();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_u32(ok), 0x12345678ul);
      EXPECT_TRUE(ok);

      buf[0] = 0xff;
      buf[1] = 0xff;
      buf[2] = 0xff;
      buf[3] = 0xff;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint32(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      sv = deserializer.take_sv();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_u32(ok), 0xfffffffful);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, RangeCheckUInt8Value)
   {
      std::array<std::uint8_t, UINT8_SIZE> buf = { 7u };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_uint32(0u, 7u), APX_NO_ERROR);
      auto sv = deserializer.take_sv();
      EXPECT_NE(sv.get(), nullptr);
      EXPECT_EQ(sv->to_u32(ok), 7u);
      EXPECT_TRUE(ok);

   }

}