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

   TEST(Deserializer, UnpackUInt16)
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

   TEST(Deserializer, UnpackUInt32)
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

   TEST(Deserializer, UnpackUInt64)
   {
      std::array<std::uint8_t, UINT64_SIZE> buf = { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint64(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      auto sv = deserializer.take_sv();
      EXPECT_NE(sv.get(), nullptr);
      EXPECT_EQ(sv->to_u32(ok), 0u);
      EXPECT_TRUE(ok);

      buf[0] = 0xef;
      buf[1] = 0xcd;
      buf[2] = 0xab;
      buf[3] = 0x89;
      buf[4] = 0x67;
      buf[5] = 0x45;
      buf[6] = 0x23;
      buf[7] = 0x01;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint64(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      sv = deserializer.take_sv();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_u64(ok), 0x0123456789abcdefull);
      EXPECT_TRUE(ok);

      buf[0] = 0xff;
      buf[1] = 0xff;
      buf[2] = 0xff;
      buf[3] = 0xff;
      buf[4] = 0xff;
      buf[5] = 0xff;
      buf[6] = 0xff;
      buf[7] = 0xff;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint64(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      sv = deserializer.take_sv();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_u64(ok), 0xffffffffffffffffull);
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

   TEST(Deserializer, RangeCheckUInt64)
   {
      std::array<std::uint8_t, UINT64_SIZE> buf = { 0x00, 0xba, 0x1d, 0xd2, 0x05, 0x00, 0x00, 0x00 }; //25*10^9
      Deserializer deserializer;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint64(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_uint64(0u, 25000000000ull), APX_NO_ERROR);
      buf[0]++;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint64(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_uint64(0u, 25000000000ull), APX_VALUE_RANGE_ERROR);
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

   TEST(Deserializer, UnpackInt64)
   {
      std::array<std::uint8_t, INT64_SIZE> buf = { 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int64(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      auto sv = deserializer.take_sv();
      EXPECT_NE(sv.get(), nullptr);
      EXPECT_EQ(sv->to_i64(ok), 0);
      EXPECT_TRUE(ok);

      buf[0] = 0x00;
      buf[1] = 0x00;
      buf[2] = 0x00;
      buf[3] = 0x00;
      buf[4] = 0x00;
      buf[5] = 0x00;
      buf[6] = 0x00;
      buf[7] = 0x80;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int64(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      sv = deserializer.take_sv();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_i64(ok), INT64_MIN);
      EXPECT_TRUE(ok);

      buf[0] = 0xff;
      buf[1] = 0xff;
      buf[2] = 0xff;
      buf[3] = 0xff;
      buf[4] = 0xff;
      buf[5] = 0xff;
      buf[6] = 0xff;
      buf[7] = 0x7f;
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int64(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Scalar);
      sv = deserializer.take_sv();
      EXPECT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_i64(ok), INT64_MAX);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackUInt8Array)
   {
      constexpr std::size_t array_length = 3u;
      std::array<std::uint8_t, UINT8_SIZE* array_length> buf = { 0x00u, 0x12u, 0xffu };
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
      EXPECT_EQ(sv->to_u32(ok), 0x00u);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_u32(ok), 0x12u);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(2));
      EXPECT_EQ(sv->to_u32(ok), 0xffu);
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

   TEST(Deserializer, UnpackUInt8DynamicArrayWithRangeCheck)
   {
      constexpr std::uint8_t current_array_length = 2u;
      constexpr std::size_t max_array_length = 3u;
      std::array<std::uint8_t, UINT8_SIZE + UINT8_SIZE* current_array_length> buf = { current_array_length, 0x07u, 0x06u };
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

   TEST(Deserializer, UnpackUInt16Array)
   {
      constexpr std::size_t array_length = 3u;
      std::array<std::uint8_t, UINT16_SIZE* array_length> buf = { 0x00, 0x00, 0x34, 0x12, 0xff, 0xff};
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint16(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_u32(ok), 0x0000u);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_u32(ok), 0x1234u);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(2));
      EXPECT_EQ(sv->to_u32(ok), 0xffffu);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackUInt16ArrayWithRangeCheck)
   {
      constexpr std::size_t array_length = 3u;
      std::array<std::uint8_t, UINT16_SIZE* array_length> buf = { 0x00, 0x00, 0xe8, 0x03u,  0x10u, 0x027u }; //{0, 1000, 10000)
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint16(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_uint32(0, 10000), APX_NO_ERROR);
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_u32(ok), 0u);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_u32(ok), 1000u);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(2));
      EXPECT_EQ(sv->to_u32(ok), 10000u);
      EXPECT_TRUE(ok);

      buf[4]++; //force value to 10001 (outside range)
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint16(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_int32(0, 10000), APX_VALUE_RANGE_ERROR);
   }

   TEST(Deserializer, UnpackUInt16DynamicArrayWithRangeCheck)
   {
      constexpr std::uint8_t current_array_length = 2u;
      constexpr std::size_t max_array_length = 3u;
      std::array<std::uint8_t, UINT8_SIZE + UINT16_SIZE* current_array_length> buf = { current_array_length, 0x00, 0x00, 0x10u, 0x027u }; //{0, 10000)
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint16(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_uint32(0, 10000), APX_NO_ERROR);
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), current_array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_u32(ok), 0u);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_u32(ok), 10000u);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackUInt32Array)
   {
      constexpr std::size_t array_length = 3u;
      std::array<std::uint8_t, UINT32_SIZE* array_length> buf = { 0x00, 0x00, 0x00, 0x00, 0x78, 0x56, 0x34, 0x12, 0xff, 0xff, 0xff, 0xff };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint32(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_u32(ok), 0x00000000ul);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_u32(ok), 0x12345678ul);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(2));
      EXPECT_EQ(sv->to_u32(ok), 0xfffffffful);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackUInt32ArrayWithRangeCheck)
   {
      constexpr std::size_t array_length = 2u;
      std::array<std::uint8_t, UINT32_SIZE* array_length> buf = { 0x00u, 0x00u, 0x00u, 0x00u, 0xa0u, 0x86u, 0x01u, 0x00u };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint32(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_uint32(0u, 100000), APX_NO_ERROR);
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_u32(ok), 0u);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_u32(ok), 100000u);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackUInt32DynamicArrayWithRangeCheck)
   {
      constexpr std::uint8_t current_array_length = 2u;
      constexpr std::size_t max_array_length = 3u;
      std::array<std::uint8_t, UINT8_SIZE + UINT32_SIZE* max_array_length> buf = { current_array_length, 0x00u, 0x00u, 0x00u, 0x00u, 0xa0u, 0x86u, 0x01u, 0x00u, 0xD, 0xE, 0xA, 0xD };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint32(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), UINT8_SIZE + UINT32_SIZE * current_array_length);
      ASSERT_EQ(deserializer.check_value_range_uint32(0u, 100000), APX_NO_ERROR);
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), current_array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_u32(ok), 0u);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_u32(ok), 100000u);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackUInt64Array)
   {
      constexpr std::size_t array_length = 2u;
      std::array<std::uint8_t, UINT64_SIZE* array_length> buf = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint64(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_u64(ok), 0x00000000ull);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_u64(ok), UINT64_MAX);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackUInt64ArrayWithRangeCheck)
   {
      constexpr std::size_t array_length = 2u;
      std::array<std::uint8_t, UINT64_SIZE* array_length> buf = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0xba, 0x1d, 0xd2, 0x05, 0x00, 0x00, 0x00 };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint64(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_uint64(0ull, 25000000000ull), APX_NO_ERROR);
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_u64(ok), 0x00000000ull);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_u64(ok), 25000000000ull);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackUInt64DynamicArrayWithRangeCheck)
   {
      constexpr std::uint8_t current_array_length = 2u;
      constexpr std::size_t max_array_length = 3u;
      std::array<std::uint8_t, UINT8_SIZE + UINT64_SIZE* current_array_length> buf = { current_array_length, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0xba, 0x1d, 0xd2, 0x05, 0x00, 0x00, 0x00 };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint64(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_uint64(0ull, 25000000000ull), APX_NO_ERROR);
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), current_array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_u64(ok), 0x00000000ull);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_u64(ok), 25000000000ull);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackInt8Array)
   {
      constexpr std::size_t array_length = 4u;
      std::array<std::uint8_t, INT8_SIZE* array_length> buf = { 0x80, 0xffu, 0x00, 0x7fu };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int8(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_i32(ok), -128);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_i32(ok), -1);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(2));
      EXPECT_EQ(sv->to_i32(ok), 0);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(3));
      EXPECT_EQ(sv->to_i32(ok), 127);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackInt8ArrayWithRangeCheck)
   {
      constexpr std::size_t array_length = 2u;
      std::array<std::uint8_t, INT8_SIZE* array_length> buf = { 0xf6u, 0x0au }; //{-10, 10}
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int8(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_int32(-10, 10), APX_NO_ERROR);
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_i32(ok), -10);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_i32(ok), 10);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackInt8DynamicArrayWithRangeCheck)
   {
      constexpr std::uint8_t current_array_length = 2u;
      constexpr std::size_t max_array_length = 4u;
      std::array<std::uint8_t, UINT8_SIZE + INT8_SIZE * max_array_length> buf = { current_array_length, 0xf6u, 0x0au, 0xee, 0xee };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int8(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), UINT8_SIZE + INT8_SIZE * current_array_length);
      ASSERT_EQ(deserializer.check_value_range_int32(-10, 10), APX_NO_ERROR);
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), current_array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_i32(ok), -10);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_i32(ok), 10);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackInt16Array)
   {
      constexpr std::size_t array_length = 4u;
      std::array<std::uint8_t, INT16_SIZE* array_length> buf = { 0x00, 0x80, 0xff, 0xffu, 0x00, 0x00, 0xff, 0x7fu };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int16(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_i32(ok), INT16_MIN);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_i32(ok), -1);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(2));
      EXPECT_EQ(sv->to_i32(ok), 0);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(3));
      EXPECT_EQ(sv->to_i32(ok), INT16_MAX);
      EXPECT_TRUE(ok);
   }
   TEST(Deserializer, UnpackInt16ArrayWithRangeCheck)
   {
      constexpr std::size_t array_length = 2u;
      std::array<std::uint8_t, INT16_SIZE* array_length> buf = { 0x18, 0xFC, 0xE8, 0x03u}; //{-1000, 1000)
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int16(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_int32(-1000, 1000), APX_NO_ERROR);
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_i32(ok), -1000);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_i32(ok), 1000);
      EXPECT_TRUE(ok);

      buf[2]++; //force value to 1001 (outside range)
      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_uint16(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_int32(-1000, 1000), APX_VALUE_RANGE_ERROR);
   }

   TEST(Deserializer, UnpackInt16DynamicArrayWithRangeCheck)
   {
      constexpr std::uint8_t current_array_length = 2u;
      constexpr std::size_t max_array_length = 4u;
      std::array<std::uint8_t, UINT8_SIZE + INT16_SIZE* max_array_length> buf = { current_array_length, 0x18, 0xFC, 0xE8, 0x03u, 0xee, 0xee, 0xee, 0xee };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int16(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), UINT8_SIZE + INT16_SIZE* current_array_length);
      ASSERT_EQ(deserializer.check_value_range_int32(-1000, 1000), APX_NO_ERROR);
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), current_array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_i32(ok), -1000);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_i32(ok), 1000);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackInt32Array)
   {
      constexpr std::size_t array_length = 4u;
      std::array<std::uint8_t, INT32_SIZE* array_length> buf = { 0x00, 0x00, 0x00, 0x80, 0xff, 0xffu, 0xff, 0xffu, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x7fu };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int32(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_i32(ok), INT32_MIN);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_i32(ok), -1);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(2));
      EXPECT_EQ(sv->to_i32(ok), 0);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(3));
      EXPECT_EQ(sv->to_i32(ok), INT32_MAX);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackInt32ArrayWithRangeCheck)
   {
      constexpr std::size_t array_length = 2u;
      std::array<std::uint8_t, INT32_SIZE* array_length> buf = { 0x60, 0x79, 0xFE, 0xFF, 0xa0, 0x86, 0x01, 0x00}; //{-100000, 100000}
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int32(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_int32(-100000, 100000), APX_NO_ERROR);
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_i32(ok), -100000);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_i32(ok), 100000);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackInt32DynamicArrayWithRangeCheck)
   {
      constexpr std::uint8_t current_array_length = 2u;
      constexpr std::size_t max_array_length = 3u;
      std::array<std::uint8_t, UINT8_SIZE + INT32_SIZE* max_array_length> buf = { current_array_length, 0x60, 0x79, 0xFE, 0xFF, 0xa0, 0x86, 0x01, 0x00, 0xee, 0xee, 0xee, 0xee };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int32(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), UINT8_SIZE + INT32_SIZE * current_array_length);
      ASSERT_EQ(deserializer.check_value_range_int32(-100000, 100000), APX_NO_ERROR);
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), current_array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_i32(ok), -100000);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_i32(ok), 100000);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackInt64Array)
   {
      constexpr std::size_t array_length = 4u;
      std::array<std::uint8_t, INT64_SIZE* array_length> buf = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
         0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu,
         0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
         0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0x7fu };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int64(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_i64(ok), INT64_MIN);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_i64(ok), -1);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(2));
      EXPECT_EQ(sv->to_i64(ok), 0);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(3));
      EXPECT_EQ(sv->to_i64(ok), INT64_MAX);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackInt64ArrayWithRangeCheck)
   {
      constexpr std::size_t array_length = 2u;
      std::array<std::uint8_t, INT64_SIZE* array_length> buf = { 0x00u, 0x46u, 0xe2u, 0x2du, 0xfau, 0xffu, 0xffu, 0xffu, //-25E9
         0x00u, 0xbau, 0x1du, 0xd2u, 0x05u, 0x00, 0x00, 0x00}; //+25E9
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int64(array_length, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), buf.size());
      ASSERT_EQ(deserializer.check_value_range_int64(-25000000000ll, 25000000000ll), APX_NO_ERROR);
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_i64(ok), -25000000000ll);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_i64(ok), 25000000000ll);
      EXPECT_TRUE(ok);
   }

   TEST(Deserializer, UnpackInt64DynamicArrayWithRangeCheck)
   {
      constexpr std::uint8_t current_array_length = 2u;
      constexpr std::size_t max_array_length = 3u;
      std::array<std::uint8_t, UINT8_SIZE + INT64_SIZE* current_array_length> buf = { current_array_length, 0x00u, 0x46u, 0xe2u, 0x2du, 0xfau, 0xffu, 0xffu, 0xffu,
         0x00u, 0xbau, 0x1du, 0xd2u, 0x05u, 0x00, 0x00, 0x00 };
      Deserializer deserializer;
      auto ok = false;

      ASSERT_EQ(deserializer.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(deserializer.unpack_int64(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(deserializer.bytes_read(), UINT8_SIZE + INT64_SIZE * current_array_length);
      ASSERT_EQ(deserializer.check_value_range_int64(-25000000000ll, 25000000000ll), APX_NO_ERROR);
      ASSERT_EQ(deserializer.value_type(), dtl::ValueType::Array);
      auto av = deserializer.take_av();
      EXPECT_NE(av.get(), nullptr);
      EXPECT_EQ(av->length(), current_array_length);
      auto sv = dtl::sv_cast(av->at(0));
      EXPECT_EQ(sv->to_i64(ok), -25000000000ll);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(av->at(1));
      EXPECT_EQ(sv->to_i64(ok), 25000000000ll);
      EXPECT_TRUE(ok);
   }

}