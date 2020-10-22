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

   TEST(Deserializer, RangeCheckUInt8)
   {
      std::array<std::uint8_t, UINT8_SIZE> buf = { 7u };
      Deserializer deserializer;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_uint32(0u, 7u), APX_NO_ERROR);
      buf[0]++;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_uint32(0u, 7u), APX_VALUE_RANGE_ERROR);
   }

   TEST(Deserializer, RangeCheckUInt16)
   {
      std::array<std::uint8_t, UINT16_SIZE> buf = { 0x10, 0x27 }; //10000
      Deserializer deserializer;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint16(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_uint32(0u, 10000u), APX_NO_ERROR);
      buf[0]++;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint16(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_uint32(0u, 10000u), APX_VALUE_RANGE_ERROR);
   }

   TEST(Deserializer, RangeCheckUInt32)
   {
      std::array<std::uint8_t, UINT32_SIZE> buf = { 0xA0, 0x86, 0x01 }; //100000
      Deserializer deserializer;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint32(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_uint32(0u, 100000u), APX_NO_ERROR);
      buf[0]++;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint32(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_uint32(0u, 100000u), APX_VALUE_RANGE_ERROR);
   }

   TEST(Deserializer, UnpackInt8)
   {
      std::array<std::uint8_t, INT8_SIZE> buf = { 0 };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      auto sv = deserializer.take_sv();
      EXPECT_NE(sv.get(), nullptr);
      EXPECT_EQ(sv->to_i32(ok), 0);
      EXPECT_TRUE(ok);

      buf[0] = 0x80;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      sv = deserializer.take_sv();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_i32(ok), INT8_MIN);
      EXPECT_TRUE(ok);

      buf[0] = 0x7f;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      sv = deserializer.take_sv();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_i32(ok), INT8_MAX);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackInt16)
   {
      std::array<std::uint8_t, UINT16_SIZE> buf = { 0u, 0u };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int16(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      auto sv = deserializer.take_sv();
      EXPECT_NE(sv.get(), nullptr);
      EXPECT_EQ(sv->to_i32(ok), 0);
      EXPECT_TRUE(ok);

      buf[0] = 0x00;
      buf[1] = 0x80;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int16(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      sv = deserializer.take_sv();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_i32(ok), INT16_MIN);
      EXPECT_TRUE(ok);

      buf[0] = 0xff;
      buf[1] = 0x7f;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int16(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      sv = deserializer.take_sv();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_i32(ok), INT16_MAX);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackInt32)
   {
      std::array<std::uint8_t, INT32_SIZE> buf = { 0u, 0u, 0u, 0u };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int32(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      auto sv = deserializer.take_sv();
      EXPECT_NE(sv.get(), nullptr);
      EXPECT_EQ(sv->to_i32(ok), 0);
      EXPECT_TRUE(ok);

      buf[0] = 0x00;
      buf[1] = 0x00;
      buf[2] = 0x00;
      buf[3] = 0x80;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int32(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      sv = deserializer.take_sv();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_i32(ok), INT32_MIN);
      EXPECT_TRUE(ok);

      buf[0] = 0xff;
      buf[1] = 0xff;
      buf[2] = 0xff;
      buf[3] = 0x7f;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int32(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      sv = deserializer.take_sv();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_i32(ok), INT32_MAX);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackUInt8Array)
   {
      constexpr std::size_t array_length = 3u;
      std::array<std::uint8_t, UINT8_SIZE* array_length> buf = { 0xaau, 0xbbu, 0xccu };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint8(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_u32(ok), 0xaau);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_u32(ok), 0xbbu);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(2));
      EXPECT_EQ(sv->to_u32(ok), 0xccu);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackUInt8ArrayWithTooShortReadBuffer)
   {
      constexpr std::size_t array_length = 3u;
      std::array<std::uint8_t, UINT8_SIZE* array_length> buf = { 0xaau, 0xbbu, 0xccu };
      Deserializer deserializer;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint8(array_length+1, apx::SizeType::None), APX_BUFFER_BOUNDARY_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
   }

   TEST(Deserializer, UnpackUInt8ArrayWithRangeCheck)
   {
      constexpr std::size_t array_length = 3u;
      std::array<std::uint8_t, UINT8_SIZE* array_length> buf = { 0x07u, 0x03u, 0x07u };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint8(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.check_value_range_uint32(0u, 7u), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_u32(ok), 0x07u);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_u32(ok), 0x03u);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(2));
      EXPECT_EQ(sv->to_u32(ok), 0x07u);
      EXPECT_TRUE(ok);

      buf[1] = 0x08; //Value outside range
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint8(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_uint32(0u, 7u), APX_VALUE_RANGE_ERROR);
   }

   TEST(Deserializer, UnpackUInt8DynamicArray)
   {
      constexpr std::uint8_t current_array_length = 2u;
      constexpr std::size_t max_array_length = 3u;
      std::array<std::uint8_t, UINT8_SIZE * max_array_length> buf = { current_array_length, 0x10u, 0x11u };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint8(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), current_array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_u32(ok), 0x10u);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_u32(ok), 0x11u);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackUInt8DynamicArrayWithRangeCheck)
   {
      constexpr std::uint8_t current_array_length = 2u;
      constexpr std::size_t max_array_length = 3u;
      std::array<std::uint8_t, UINT8_SIZE* max_array_length> buf = { current_array_length, 0x07u, 0x06u };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint8(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(deserializer.check_value_range_uint32(0u, 7u), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), current_array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_u32(ok), 0x07u);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_u32(ok), 0x06u);
      EXPECT_TRUE(ok);
   }

}