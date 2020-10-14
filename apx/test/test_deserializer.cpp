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
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), 1), APX_NO_ERROR);
   }

   TEST(Deserializer, UnpackU8)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Deserializer deserializer;
      dtl::ValueType value_type{ dtl::ValueType::NoneType };
      auto ok = false;
      dtl::Scalar const* sv{ nullptr };

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), 1), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint8(0u, apx::SizeType::None, value_type), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), 1u);
      ASSERT_EQ(value_type, dtl::ValueType::Scalar);
      sv = deserializer.get_scalar();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_u32(ok), 0u);
      EXPECT_TRUE(ok);

      buf[0] = 0x12;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), 1), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint8(0u, apx::SizeType::None, value_type), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), 1u);
      ASSERT_EQ(value_type, dtl::ValueType::Scalar);
      sv = deserializer.get_scalar();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_u32(ok), 0x12u);
      EXPECT_TRUE(ok);

      buf[0] = 0xff;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), 1), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint8(0u, apx::SizeType::None, value_type), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), 1u);
      ASSERT_EQ(value_type, dtl::ValueType::Scalar);
      sv = deserializer.get_scalar();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_u32(ok), 0xffu);
      EXPECT_TRUE(ok);

   }
}