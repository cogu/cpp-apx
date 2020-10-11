#include "pch.h"
#include "cpp-apx/serializer.h"
#include <array>

using namespace apx::vm;

namespace apx_test
{
   TEST(SerializerState, SetNoneTypeValue)
   {
      Serializer::State state;
      dtl::DynamicValue dv = dtl::make_dv();
      auto tmp = dv.get();
      state.set_value(tmp);
      ASSERT_EQ(state.value_type, dtl::ValueType::NoneType);
      ASSERT_EQ(state.value.dv, nullptr);
   }

   TEST(Serializer, SetInvalidBuffer)
   {
      std::array<std::uint8_t, 1> buf;
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(nullptr, 1), APX_INVALID_ARGUMENT_ERROR);
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 0), APX_INVALID_ARGUMENT_ERROR);
   }

   TEST(Serializer, SetBuffer)
   {
      std::array<std::uint8_t, 1> buf;
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 1), APX_NO_ERROR);
   }

   TEST(Serializer, SetScalar)
   {
      Serializer serializer;
      dtl::ScalarValue sv = dtl::make_sv<std::int32_t>(18);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
   }

   TEST(Serializer, SetArray)
   {
      Serializer serializer;
      dtl::ArrayValue av = dtl::make_av();
      av->push(dtl::make_sv<std::int32_t>(1));
      av->push(dtl::make_sv<std::int32_t>(2));
      av->push(dtl::make_sv<std::int32_t>(3));

      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
   }

   TEST(Serializer, SetHash)
   {
      Serializer serializer;
      auto hv = dtl::make_hv();
      hv->set("First", dtl::make_sv<std::int32_t>(1));
      hv->set("Second", dtl::make_sv<std::int32_t>(2));
      hv->set("Third", dtl::make_sv<std::int32_t>(3));

      ASSERT_EQ(serializer.set_value(hv), APX_NO_ERROR);
   }

   TEST(Serializer, PackUInt8FromInt8Value)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 1), APX_NO_ERROR);
      dtl::ScalarValue sv = dtl::make_sv<std::int32_t>(7);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 1u);
      ASSERT_EQ(buf[0], 7);
   }

   TEST(Serializer, PackUInt8FromUInt8Value)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 1), APX_NO_ERROR);
      dtl::ScalarValue sv = dtl::make_sv<std::uint32_t>(255);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 1u);
      ASSERT_EQ(buf[0], 255);
   }

   TEST(Serializer, PackUInt8FromValueOutOfRange)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 1), APX_NO_ERROR);
      auto sv = dtl::make_sv<std::uint32_t>(6000);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0u, apx::SizeType::None), APX_VALUE_RANGE_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 0u);

   }
   TEST(Serializer, PackUIn8FromNegativeValue)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 1), APX_NO_ERROR);
      auto sv = dtl::make_sv<std::int32_t>(-1);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0u, apx::SizeType::None), APX_VALUE_CONVERSION_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 0u);
   }

   TEST(Serializer, RangeCheckUInt32ValueOutOfRange)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 1), APX_NO_ERROR);
      auto sv = dtl::make_sv<std::uint32_t>(15);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_uint32(0u, 7u), APX_VALUE_RANGE_ERROR);
   }

   TEST(Serializer, RangeCheckUInt8Value)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 1), APX_NO_ERROR);
      auto sv = dtl::make_sv<std::uint32_t>(0);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_uint32(0u, 7u), APX_NO_ERROR);
      sv->set((uint32_t)7);
      ASSERT_EQ(serializer.check_value_range_uint32(0u, 7u), APX_NO_ERROR);
      sv->set((uint32_t)8);
      ASSERT_EQ(serializer.check_value_range_uint32(0u, 7u), APX_VALUE_RANGE_ERROR);
   }

   TEST(Serializer, RangeCheckUInt8Array)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 1), APX_NO_ERROR);
      auto av = dtl::make_av();
      av->push(dtl::make_sv<std::int32_t>(0));
      av->push(dtl::make_sv<std::uint32_t>(7));
      av->push(dtl::make_sv<std::uint32_t>(3));
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_uint32(0u, 7u), APX_NO_ERROR);
   }

   TEST(Serializer, RangeCheckUInt8ArrayWithOutOfRangeValue)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 1), APX_NO_ERROR);
      auto av = dtl::make_av();
      av->push(dtl::make_sv<std::int32_t>(0));
      av->push(dtl::make_sv<std::int32_t>(8));
      av->push(dtl::make_sv<std::int32_t>(7));
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_uint32(0u, 7u), APX_VALUE_RANGE_ERROR);
   }

   TEST(Serializer, RangeCheckUInt8ArrayWithBadValue)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 1), APX_NO_ERROR);
      auto av = dtl::make_av();
      av->push(dtl::make_sv<std::int32_t>(0));
      av->push(dtl::make_sv<std::int32_t>(7));
      av->push(dtl::make_sv<std::int32_t>(-1));
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_uint32(0u, 7u), APX_VALUE_CONVERSION_ERROR);
   }

   TEST(Serializer, PackUInt8ArrayWithDefaultLimitCheck)
   {
      std::array<std::uint8_t, 4> buf = { 0, 0, 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto av = dtl::make_av();
      av->push(dtl::make_sv<std::int32_t>(1));
      av->push(dtl::make_sv<std::int32_t>(2));
      av->push(dtl::make_sv<std::int32_t>(3));
      av->push(dtl::make_sv<std::int32_t>(4));
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(4u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 4u);
      ASSERT_EQ(buf[0], 1);
      ASSERT_EQ(buf[1], 2);
      ASSERT_EQ(buf[2], 3);
      ASSERT_EQ(buf[3], 4);
   }

   TEST(Serializer, PackCharString)
   {
      std::array<std::uint8_t, 4> buf = { 0xFF, 0xFF, 0xFF, 0xFF };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto sv = dtl::make_sv("Test");
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_char(4u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 4u);
      ASSERT_EQ(buf[0], 'T');
      ASSERT_EQ(buf[1], 'e');
      ASSERT_EQ(buf[2], 's');
      ASSERT_EQ(buf[3], 't');
   }

   TEST(Serializer, PackShorterCharString)
   {
      std::array<std::uint8_t, 6> buf = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto sv = dtl::make_sv("Tst");
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_char(6u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 6u);
      ASSERT_EQ(buf[0], 'T');
      ASSERT_EQ(buf[1], 's');
      ASSERT_EQ(buf[2], 't');
      ASSERT_EQ(buf[3], 0u);
      ASSERT_EQ(buf[4], 0u);
      ASSERT_EQ(buf[5], 0u);
   }

   TEST(Serializer, PackTooLargeCharString)
   {
      std::array<std::uint8_t, 4> buf = { 0xFF, 0xFF, 0xFF, 0xFF};
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto sv = dtl::make_sv("Short");
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_char(4u, apx::SizeType::None), APX_BUFFER_BOUNDARY_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 0);
   }

   TEST(Serializer, PackChar8String)
   {
      std::array<std::uint8_t, 4> buf = { 0xFF, 0xFF, 0xFF, 0xFF };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto sv = dtl::make_sv("Test");
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_char8(4u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 4u);
      ASSERT_EQ(buf[0], 'T');
      ASSERT_EQ(buf[1], 'e');
      ASSERT_EQ(buf[2], 's');
      ASSERT_EQ(buf[3], 't');
   }


}