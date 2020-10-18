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
      ASSERT_EQ(serializer.set_write_buffer(nullptr, buf.size()), APX_INVALID_ARGUMENT_ERROR);
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 0), APX_INVALID_ARGUMENT_ERROR);
   }

   TEST(Serializer, SetBuffer)
   {
      std::array<std::uint8_t, 1> buf;
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
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

   TEST(Serializer, PackUInt8FromInt32Value)
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

   TEST(Serializer, PackUInt8FromUInt32Value)
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

   TEST(Serializer, PackUInt8FromValueOutOfRangeYieldsRangeError)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto sv = dtl::make_sv<std::uint32_t>(6000);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0u, apx::SizeType::None), APX_VALUE_RANGE_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 0u);

   }
   TEST(Serializer, PackUIn8FromNegativeValueYieldsConversionError)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 1), APX_NO_ERROR);
      auto sv = dtl::make_sv<std::int32_t>(-1);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0u, apx::SizeType::None), APX_VALUE_CONVERSION_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 0u);
   }
   TEST(Serializer, PackUInt8Array)
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
      std::array<std::uint8_t, 4> buf = { 0xFF, 0xFF, 0xFF, 0xFF };
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

   TEST(Serializer, PackRecordUInt8)
   {
      std::array<std::uint8_t, 3> buf = { 0xFF, 0xFF, 0xFF };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto hv = dtl::make_hv({ {"Red", dtl::make_sv<std::uint8_t>(0x02)},
                               {"Green", dtl::make_sv<std::uint8_t>(0x12)},
                               {"Blue", dtl::make_sv<std::uint8_t>(0xaa)} });
      ASSERT_EQ(serializer.set_value(hv), APX_NO_ERROR);
      ASSERT_EQ(serializer.record_select("Red", false), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.record_select("Green", false), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.record_select("Blue", false), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 3u);
      ASSERT_EQ(buf[0], 0x02);
      ASSERT_EQ(buf[1], 0x12);
      ASSERT_EQ(buf[2], 0xaa);
   }

   TEST(Serializer, PackRecordUInt8WithRangeCheck)
   {
      std::array<std::uint8_t, 3> buf = { 0xFF, 0xFF, 0xFF };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto hv = dtl::make_hv({ {"First", dtl::make_sv<std::uint8_t>(7)},
                               {"Second", dtl::make_sv<std::uint8_t>(3)},
                               {"Third", dtl::make_sv<std::uint8_t>(3)} });
      ASSERT_EQ(serializer.set_value(hv), APX_NO_ERROR);
      ASSERT_EQ(serializer.record_select("First", false), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_u32(0u, 7u), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.record_select("Second", false), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_u32(0u, 3u), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.record_select("Third", false), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_u32(0u, 3u), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 3u);
      ASSERT_EQ(buf[0], 7u);
      ASSERT_EQ(buf[1], 3u);
      ASSERT_EQ(buf[2], 3u);
   }

   TEST(Serializer, PackUInt16FromInt32Value)
   {
      std::array<std::uint8_t, UINT16_SIZE> buf = { 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ScalarValue sv = dtl::make_sv<std::int32_t>(0x1234);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint16(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), UINT16_SIZE);
      ASSERT_EQ(buf[0], 0x34u);
      ASSERT_EQ(buf[1], 0x12u);
   }

   TEST(Serializer, PackUInt16FromUInt32Value)
   {
      std::array<std::uint8_t, UINT16_SIZE> buf = { 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ScalarValue sv = dtl::make_sv<std::uint32_t>(0xffff);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint16(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), UINT16_SIZE);
      ASSERT_EQ(buf[0], 0xffu);
      ASSERT_EQ(buf[1], 0xffu);
   }

   TEST(Serializer, PackUInt16Array)
   {
      std::array<std::uint8_t, UINT16_SIZE * 5> buf = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto av = dtl::make_av({
         dtl::make_sv<std::uint32_t>(0x11a),
         dtl::make_sv<std::uint32_t>(0x21b),
         dtl::make_sv<std::uint32_t>(0x31c),
         dtl::make_sv<std::uint32_t>(0x41d),
         dtl::make_sv<std::uint32_t>(0x51e),
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint16(5u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), UINT16_SIZE * 5);
      ASSERT_EQ(buf[0], 0x1au);
      ASSERT_EQ(buf[1], 0x01u);
      ASSERT_EQ(buf[2], 0x1bu);
      ASSERT_EQ(buf[3], 0x02u);
      ASSERT_EQ(buf[4], 0x1cu);
      ASSERT_EQ(buf[5], 0x03u);
      ASSERT_EQ(buf[6], 0x1du);
      ASSERT_EQ(buf[7], 0x04u);
      ASSERT_EQ(buf[8], 0x1eu);
      ASSERT_EQ(buf[9], 0x05u);
   }

   TEST(Serializer, PackUInt16ArrayWithRangeCheck)
   {
      std::array<std::uint8_t, UINT16_SIZE * 2> buf = { 0, 0, 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto av = dtl::make_av({
         dtl::make_sv<std::uint32_t>(999),
         dtl::make_sv<std::uint32_t>(999),
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_u32(0, 999), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint16(2u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), UINT16_SIZE * 2);
      ASSERT_EQ(buf[0], 0xe7u);
      ASSERT_EQ(buf[1], 0x03u);
      ASSERT_EQ(buf[2], 0xe7u);
      ASSERT_EQ(buf[3], 0x03u);
   }

   TEST(Serializer, PackUInt32Value)
   {
      std::array<std::uint8_t, UINT32_SIZE> buf = { 0, 0, 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ScalarValue sv = dtl::make_sv<std::uint32_t>(0x12345678);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint32(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), UINT32_SIZE);
      ASSERT_EQ(buf[0], 0x78u);
      ASSERT_EQ(buf[1], 0x56u);
      ASSERT_EQ(buf[2], 0x34u);
      ASSERT_EQ(buf[3], 0x12u);
   }

   TEST(Serializer, PackUInt32Array)
   {
      std::array<std::uint8_t, UINT32_SIZE * 2> buf = { 0, 0, 0, 0, 0, 0, 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto av = dtl::make_av({
         dtl::make_sv<std::uint32_t>(100000),
         dtl::make_sv<std::uint32_t>(200000)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint32(2u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), UINT32_SIZE * 2);
      ASSERT_EQ(buf[0], 0xa0u);
      ASSERT_EQ(buf[1], 0x86u);
      ASSERT_EQ(buf[2], 0x01u);
      ASSERT_EQ(buf[3], 0x00u);
      ASSERT_EQ(buf[4], 0x40u);
      ASSERT_EQ(buf[5], 0x0du);
      ASSERT_EQ(buf[6], 0x03u);
      ASSERT_EQ(buf[7], 0x00u);
   }

   TEST(Serializer, PackUInt32ArrayWithRangeCheck)
   {
      std::array<std::uint8_t, UINT32_SIZE * 2> buf = { 0, 0, 0, 0, 0, 0, 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto av = dtl::make_av({
         dtl::make_sv<std::uint32_t>(99999),
         dtl::make_sv<std::uint32_t>(99998),
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_u32(0, 99999UL), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint32(2u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0x9fu);
      ASSERT_EQ(buf[1], 0x86u);
      ASSERT_EQ(buf[2], 0x01u);
      ASSERT_EQ(buf[3], 0x00u);
      ASSERT_EQ(buf[4], 0x9eu);
      ASSERT_EQ(buf[5], 0x86u);
      ASSERT_EQ(buf[6], 0x01u);
      ASSERT_EQ(buf[7], 0x00u);
   }

   TEST(Serializer, RangeCheckUInt32Value)
   {
      Serializer serializer;
      auto sv = dtl::make_sv<std::uint32_t>(0);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_u32(0u, 7u), APX_NO_ERROR);
      sv->set((uint32_t)7);
      ASSERT_EQ(serializer.check_value_range_u32(0u, 7u), APX_NO_ERROR);
      sv->set((uint32_t)8);
      ASSERT_EQ(serializer.check_value_range_u32(0u, 7u), APX_VALUE_RANGE_ERROR);
   }

   TEST(Serializer, RangeCheckUInt32ArrayWithMixedValueTypes)
   {
      Serializer serializer;
      auto av = dtl::make_av();
      av->push(dtl::make_sv<std::int32_t>(0));
      av->push(dtl::make_sv<std::int32_t>(7));
      av->push(dtl::make_sv<std::uint32_t>(3));
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_u32(0u, 7u), APX_NO_ERROR);
   }

   TEST(Serializer, RangeCheckUInt32ArrayWithOutOfRangeValue)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 1), APX_NO_ERROR);
      auto av = dtl::make_av();
      av->push(dtl::make_sv<std::int32_t>(0));
      av->push(dtl::make_sv<std::int32_t>(8));
      av->push(dtl::make_sv<std::int32_t>(7));
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_u32(0u, 7u), APX_VALUE_RANGE_ERROR);
   }

   TEST(Serializer, RangeCheckUInt8ArrayWithBadValueType)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 1), APX_NO_ERROR);
      auto av = dtl::make_av();
      av->push(dtl::make_sv<std::uint32_t>(0));
      av->push(dtl::make_sv<std::uint32_t>(7));
      av->push(dtl::make_sv<std::int32_t>(-1));
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_u32(0u, 7u), APX_VALUE_CONVERSION_ERROR);
   }

   TEST(Serializer, RangeCheckInt32Value)
   {
      Serializer serializer;
      auto sv = dtl::make_sv<std::int32_t>(11);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_i32(-10, 10), APX_VALUE_RANGE_ERROR);
      sv->set((int32_t)10);
      ASSERT_EQ(serializer.check_value_range_i32(-10, 10), APX_NO_ERROR);
      sv->set((int32_t)0);
      ASSERT_EQ(serializer.check_value_range_i32(-10, 10), APX_NO_ERROR);
      sv->set((int32_t)-10);
      ASSERT_EQ(serializer.check_value_range_i32(-10, 10), APX_NO_ERROR);
      sv->set((int32_t)-11);
      ASSERT_EQ(serializer.check_value_range_i32(-10, 10), APX_VALUE_RANGE_ERROR);
   }

   TEST(Serializer, RangeCheckInt32ArrayWithMixedValueTypes)
   {
      Serializer serializer;
      auto av = dtl::make_av({
         dtl::make_sv<std::int32_t>(-10),
         dtl::make_sv<std::uint32_t>(9),
         dtl::make_sv<std::uint32_t>(3)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_i32(-10, 10), APX_NO_ERROR);
   }

   TEST(Serializer, RangeCheckInt32ArrayWithOutOfRangeValue)
   {
      Serializer serializer;
      auto av = dtl::make_av({
         dtl::make_sv<std::int32_t>(-10),
         dtl::make_sv<std::int32_t>(9),
         dtl::make_sv<std::int32_t>(11)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_i32(-10, 10), APX_VALUE_RANGE_ERROR);
   }

   TEST(Serializer, RangeCheckUInt64Value)
   {
      Serializer serializer;
      auto sv = dtl::make_sv<std::uint64_t>(250000001ULL);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_u64(0u, 250000000u), APX_VALUE_RANGE_ERROR);
      sv->set(250000000ULL);
      ASSERT_EQ(serializer.check_value_range_u64(0u, 250000000u), APX_NO_ERROR);
      sv->set(0);
      ASSERT_EQ(serializer.check_value_range_u64(0u, 250000000u), APX_NO_ERROR);
      sv->set(250000000ULL);
      ASSERT_EQ(serializer.check_value_range_u64(0u, 250000000u), APX_NO_ERROR);
      sv->set(250000001ULL);
      ASSERT_EQ(serializer.check_value_range_u64(0u, 250000000u), APX_VALUE_RANGE_ERROR);
   }

   TEST(Serializer, RangeCheckUInt64ArrayWithMixedValueTypes)
   {
      Serializer serializer;
      auto av = dtl::make_av({
         dtl::make_sv<std::int32_t>(0),
         dtl::make_sv<std::uint64_t>(250000000ULL),
         dtl::make_sv<std::uint32_t>(100000000ULL)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_u64(0u, 250000000u), APX_NO_ERROR);
   }

   TEST(Serializer, RangeCheckUInt64ArrayWithOutOfRangeValue)
   {
      Serializer serializer;
      auto av = dtl::make_av({
         dtl::make_sv<std::uint64_t>(250000000ULL),
         dtl::make_sv<std::uint64_t>(250000001ULL),
         dtl::make_sv<std::uint64_t>(250000000ULL)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_u64(0, 250000000ULL), APX_VALUE_RANGE_ERROR);
   }

   TEST(Serializer, RangeCheckInt64Value)
   {
      Serializer serializer;
      auto sv = dtl::make_sv<std::int64_t>(250000001ULL);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_i64(-250000000, 250000000), APX_VALUE_RANGE_ERROR);
      sv->set(250000000LL);
      ASSERT_EQ(serializer.check_value_range_i64(-250000000, 250000000), APX_NO_ERROR);
      sv->set(0);
      ASSERT_EQ(serializer.check_value_range_i64(-250000000, 250000000), APX_NO_ERROR);
      sv->set(-250000000LL);
      ASSERT_EQ(serializer.check_value_range_i64(-250000000, 250000000), APX_NO_ERROR);
      sv->set(-250000001LL);
      ASSERT_EQ(serializer.check_value_range_i64(-250000000, 250000000), APX_VALUE_RANGE_ERROR);
   }

   TEST(Serializer, RangeCheckInt64ArrayWithMixedValueTypes)
   {
      Serializer serializer;
      auto av = dtl::make_av({
         dtl::make_sv<std::int32_t>(-250000000LL),
         dtl::make_sv<std::uint64_t>(250000000ULL),
         dtl::make_sv<std::uint32_t>(10000)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_i64(-250000000LL, 250000000LL), APX_NO_ERROR);
   }

   TEST(Serializer, RangeCheckInt64ArrayWithOutOfRangeValue)
   {
      Serializer serializer;
      auto av = dtl::make_av({
         dtl::make_sv<std::int64_t>(-250000000LL),
         dtl::make_sv<std::int64_t>(-250000001LL),
         dtl::make_sv<std::int64_t>(250000000LL)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_i64(-250000000LL, 250000000LL), APX_VALUE_RANGE_ERROR);
   }

   TEST(Serializer, PackInt8FromInt32Value)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ScalarValue sv = dtl::make_sv<std::int32_t>(-128);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_int8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0x80);
   }

   TEST(Serializer, PackInt8FromValueOutOfRangeYieldsRangeError)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto sv = dtl::make_sv<std::int32_t>(128);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_int8(0u, apx::SizeType::None), APX_VALUE_RANGE_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 0u);
   }

   TEST(Serializer, PackInt8Array)
   {
      std::array<std::uint8_t, 4> buf = { 0, 0, 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto av = dtl::make_av({
         dtl::make_sv<std::int32_t>(-2),
         dtl::make_sv<std::int32_t>(-1),
         dtl::make_sv<std::int32_t>(0),
         dtl::make_sv<std::int32_t>(1)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_int8(4u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 4u);
      ASSERT_EQ(buf[0], 0xFEu);
      ASSERT_EQ(buf[1], 0xFFu);
      ASSERT_EQ(buf[2], 0u);
      ASSERT_EQ(buf[3], 1u);
   }

   TEST(Serializer, PackInt16FromInt32Value)
   {
      std::array<std::uint8_t, INT16_SIZE> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ScalarValue sv = dtl::make_sv<std::int32_t>(-32768);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_int16(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0x00);
      ASSERT_EQ(buf[1], 0x80);
   }

   TEST(Serializer, PackInt16FromValueOutOfRangeYieldsRangeError)
   {
      std::array<std::uint8_t, INT16_SIZE> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto sv = dtl::make_sv<std::int32_t>(32768);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_int16(0u, apx::SizeType::None), APX_VALUE_RANGE_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 0u);
   }

   TEST(Serializer, PackInt16Array)
   {
      std::array<std::uint8_t, 4 * INT16_SIZE> buf = { 0, 0, 0, 0, 0, 0, 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto av = dtl::make_av({
         dtl::make_sv<std::int32_t>(-32768),
         dtl::make_sv<std::int32_t>(-1),
         dtl::make_sv<std::int32_t>(0),
         dtl::make_sv<std::int32_t>(32767)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_int16(4u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0x00u);
      ASSERT_EQ(buf[1], 0x80u);
      ASSERT_EQ(buf[2], 0xffu);
      ASSERT_EQ(buf[3], 0xffu);
      ASSERT_EQ(buf[4], 0x00u);
      ASSERT_EQ(buf[5], 0x00u);
      ASSERT_EQ(buf[6], 0xffu);
      ASSERT_EQ(buf[7], 0x7fu);
   }

   TEST(Serializer, PackInt32FromInt32Value)
   {
      std::array<std::uint8_t, INT32_SIZE> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ScalarValue sv = dtl::make_sv<std::int32_t>(INT32_MIN);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_int32(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0x00);
      ASSERT_EQ(buf[1], 0x00);
      ASSERT_EQ(buf[2], 0x00);
      ASSERT_EQ(buf[3], 0x80);
   }

   TEST(Serializer, PackInt32FromValueOutOfRangeYieldsConversionError)
   {
      std::array<std::uint8_t, INT32_SIZE> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ScalarValue sv = dtl::make_sv<std::uint32_t>(0x80000000UL);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_int32(0u, apx::SizeType::None), APX_VALUE_CONVERSION_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 0u);
   }

   TEST(Serializer, PackInt32Array)
   {
      std::array<std::uint8_t, 4 * INT32_SIZE> buf = { 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto av = dtl::make_av({
         dtl::make_sv<std::int32_t>(INT32_MIN),
         dtl::make_sv<std::int32_t>(-1),
         dtl::make_sv<std::int32_t>(0),
         dtl::make_sv<std::int32_t>(INT32_MAX)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_int32(4u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0x00);
      ASSERT_EQ(buf[1], 0x00);
      ASSERT_EQ(buf[2], 0x00);
      ASSERT_EQ(buf[3], 0x80);
      ASSERT_EQ(buf[4], 0xffu);
      ASSERT_EQ(buf[5], 0xffu);
      ASSERT_EQ(buf[6], 0xffu);
      ASSERT_EQ(buf[7], 0xffu);
      ASSERT_EQ(buf[8], 0x00u);
      ASSERT_EQ(buf[9], 0x00u);
      ASSERT_EQ(buf[10], 0x00u);
      ASSERT_EQ(buf[11], 0x00u);
      ASSERT_EQ(buf[12], 0xffu);
      ASSERT_EQ(buf[13], 0xffu);
      ASSERT_EQ(buf[14], 0xffu);
      ASSERT_EQ(buf[15], 0x7fu);
   }

   TEST(Serializer, PackInt64FromInt64Value)
   {
      std::array<std::uint8_t, INT64_SIZE> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ScalarValue sv = dtl::make_sv<std::int64_t>(INT64_MIN);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_int64(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0x00);
      ASSERT_EQ(buf[1], 0x00);
      ASSERT_EQ(buf[2], 0x00);
      ASSERT_EQ(buf[3], 0x00);
      ASSERT_EQ(buf[4], 0x00);
      ASSERT_EQ(buf[5], 0x00);
      ASSERT_EQ(buf[6], 0x00);
      ASSERT_EQ(buf[7], 0x80);
   }

   TEST(Serializer, PackInt64FromValueOutOfRangeYieldsConversionError)
   {
      std::array<std::uint8_t, INT64_SIZE> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ScalarValue sv = dtl::make_sv<std::uint64_t>(0x8000000000000000ULL);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_int64(0u, apx::SizeType::None), APX_VALUE_CONVERSION_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 0u);
   }

   TEST(Serializer, PackInt64Array)
   {
      std::array<std::uint8_t, 4 * INT64_SIZE> buf = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto av = dtl::make_av({
         dtl::make_sv<std::int64_t>(INT64_MIN),
         dtl::make_sv<std::int64_t>(-1LL),
         dtl::make_sv<std::int64_t>(0LL),
         dtl::make_sv<std::int64_t>(INT64_MAX)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_int64(4u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0x00);
      ASSERT_EQ(buf[1], 0x00);
      ASSERT_EQ(buf[2], 0x00);
      ASSERT_EQ(buf[3], 0x00);
      ASSERT_EQ(buf[4], 0x00);
      ASSERT_EQ(buf[5], 0x00);
      ASSERT_EQ(buf[6], 0x00);
      ASSERT_EQ(buf[7], 0x80);
      ASSERT_EQ(buf[8], 0xffu);
      ASSERT_EQ(buf[9], 0xffu);
      ASSERT_EQ(buf[10], 0xffu);
      ASSERT_EQ(buf[11], 0xffu);
      ASSERT_EQ(buf[12], 0xffu);
      ASSERT_EQ(buf[13], 0xffu);
      ASSERT_EQ(buf[14], 0xffu);
      ASSERT_EQ(buf[15], 0xffu);
      ASSERT_EQ(buf[16], 0x00u);
      ASSERT_EQ(buf[17], 0x00u);
      ASSERT_EQ(buf[18], 0x00u);
      ASSERT_EQ(buf[19], 0x00u);
      ASSERT_EQ(buf[20], 0x00u);
      ASSERT_EQ(buf[21], 0x00u);
      ASSERT_EQ(buf[22], 0x00u);
      ASSERT_EQ(buf[23], 0x00u);
      ASSERT_EQ(buf[24], 0xffu);
      ASSERT_EQ(buf[25], 0xffu);
      ASSERT_EQ(buf[26], 0xffu);
      ASSERT_EQ(buf[27], 0xffu);
      ASSERT_EQ(buf[28], 0xffu);
      ASSERT_EQ(buf[29], 0xffu);
      ASSERT_EQ(buf[30], 0xffu);
      ASSERT_EQ(buf[31], 0x7fu);
   }

   TEST(Serializer, PackUInt64FromUInt64Value)
   {
      std::array<std::uint8_t, UINT64_SIZE> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ScalarValue sv = dtl::make_sv<std::uint64_t>(UINT64_MAX);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint64(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0xff);
      ASSERT_EQ(buf[1], 0xff);
      ASSERT_EQ(buf[2], 0xff);
      ASSERT_EQ(buf[3], 0xff);
      ASSERT_EQ(buf[4], 0xff);
      ASSERT_EQ(buf[5], 0xff);
      ASSERT_EQ(buf[6], 0xff);
      ASSERT_EQ(buf[7], 0xff);
   }

   TEST(Serializer, PackUInt64FromValueOutOfRangeYieldsConversionError)
   {
      std::array<std::uint8_t, UINT64_SIZE> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ScalarValue sv = dtl::make_sv<std::int64_t>(-1LL);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint64(0u, apx::SizeType::None), APX_VALUE_CONVERSION_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 0u);
   }

   TEST(Serializer, PackUInt64Array)
   {
      std::array<std::uint8_t, 3 * UINT64_SIZE> buf = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                       0, 0, 0, 0, 0, 0, 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto av = dtl::make_av({
         dtl::make_sv<std::uint64_t>(0ULL),
         dtl::make_sv<std::uint64_t>(0x0123456789ABCDEFULL),
         dtl::make_sv<std::uint64_t>(UINT64_MAX)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint64(3u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0x00);
      ASSERT_EQ(buf[1], 0x00);
      ASSERT_EQ(buf[2], 0x00);
      ASSERT_EQ(buf[3], 0x00);
      ASSERT_EQ(buf[4], 0x00);
      ASSERT_EQ(buf[5], 0x00);
      ASSERT_EQ(buf[6], 0x00);
      ASSERT_EQ(buf[7], 0x00);
      ASSERT_EQ(buf[8], 0xEFu);
      ASSERT_EQ(buf[9], 0xCDu);
      ASSERT_EQ(buf[10], 0xABu);
      ASSERT_EQ(buf[11], 0x89u);
      ASSERT_EQ(buf[12], 0x67u);
      ASSERT_EQ(buf[13], 0x45u);
      ASSERT_EQ(buf[14], 0x23u);
      ASSERT_EQ(buf[15], 0x01u);
      ASSERT_EQ(buf[16], 0xffu);
      ASSERT_EQ(buf[17], 0xffu);
      ASSERT_EQ(buf[18], 0xffu);
      ASSERT_EQ(buf[19], 0xffu);
      ASSERT_EQ(buf[20], 0xffu);
      ASSERT_EQ(buf[21], 0xffu);
      ASSERT_EQ(buf[22], 0xffu);
      ASSERT_EQ(buf[23], 0xffu);
   }

   TEST(Serializer, PackBoolFromBoolValue)
   {
      std::array<std::uint8_t, UINT8_SIZE> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ScalarValue sv = dtl::make_sv<bool>(true);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_bool(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0x01u);
      sv->set(false);
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_bool(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0x00u);
   }

   TEST(Serializer, PackBoolFromInt32Value)
   {
      std::array<std::uint8_t, UINT8_SIZE> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ScalarValue sv = dtl::make_sv<std::int32_t>(1);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_bool(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0x01);
      sv->set((std::int32_t) 0);
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_bool(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0x00u);
   }

   TEST(Serializer, PackBoolFromUInt32Value)
   {
      std::array<std::uint8_t, UINT8_SIZE> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ScalarValue sv = dtl::make_sv<std::uint32_t>(1);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_bool(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0x01);
      sv->set((std::uint32_t) 0);
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_bool(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0x00u);
   }

   TEST(Serializer, PackByteArray)
   {
      std::array<std::uint8_t, UINT8_SIZE * 2> buf = { 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ByteArray data{ 0x12u, 0x34u };
      auto sv = dtl::make_sv(data);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_byte_array(data.size(), apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0x12u);
      ASSERT_EQ(buf[1], 0x34u);
   }

   TEST(Serializer, PackSingleByteArray)
   {
      std::array<std::uint8_t, UINT8_SIZE> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ByteArray data{ 0xaaU };
      auto sv = dtl::make_sv(data);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_byte_array(data.size(), apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0xaaU);
   }

   TEST(Serializer, PackEmptyByteArray)
   {
      std::array<std::uint8_t, UINT8_SIZE> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ByteArray data;
      auto sv = dtl::make_sv(data);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_byte_array(data.size(), apx::SizeType::None), APX_INVALID_ARGUMENT_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 0u);
   }

   TEST(Serializer, PackByteArrayWithWrongLength)
   {
      std::array<std::uint8_t, UINT8_SIZE * 2> buf = { 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ByteArray data{ 10u }; //This should have been two bytes
      auto sv = dtl::make_sv(data);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_byte_array(2u, apx::SizeType::None), APX_VALUE_LENGTH_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 0u);
   }

   TEST(Serializer, PackDynamicUInt8ArrayWithUint8Length)
   {
      std::array<std::uint8_t, UINT8_SIZE * 5> buf = { 0, 0, 0, 0, 0 };
      std::size_t const element_size{ UINT8_SIZE };
      std::size_t const max_array_length{ 4 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto av = dtl::make_av({
         dtl::make_sv<std::uint32_t>(10)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 2u);
      ASSERT_EQ(buf[0], 1u);
      ASSERT_EQ(buf[1], 10u);
      av->push(dtl::make_sv<std::uint32_t>(11));
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 3u);
      ASSERT_EQ(buf[0], 2u);
      ASSERT_EQ(buf[1], 10u);
      ASSERT_EQ(buf[2], 11u);
      av->push(dtl::make_sv<std::uint32_t>(12));
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 4u);
      ASSERT_EQ(buf[0], 3u);
      ASSERT_EQ(buf[1], 10u);
      ASSERT_EQ(buf[2], 11u);
      ASSERT_EQ(buf[3], 12u);
      av->push(dtl::make_sv<std::uint32_t>(13));
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 4u);
      ASSERT_EQ(buf[1], 10u);
      ASSERT_EQ(buf[2], 11u);
      ASSERT_EQ(buf[3], 12u);
      ASSERT_EQ(buf[4], 13u);
      av->push(dtl::make_sv<std::uint32_t>(14));
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(max_array_length, apx::SizeType::UInt8), APX_VALUE_LENGTH_ERROR);
   }

   TEST(Serializer, PackDynamicUInt16ArrayWithUint8Length)
   {
      std::size_t const element_size{ UINT16_SIZE };
      std::size_t const max_array_length{ 4 };
      std::array<std::uint8_t, UINT8_SIZE + max_array_length * element_size> buf = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto av = dtl::make_av({
         dtl::make_sv<std::uint32_t>(200),
         dtl::make_sv<std::uint32_t>(400)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint16(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), UINT8_SIZE+element_size*2);
      ASSERT_EQ(buf[0], 2U);
      ASSERT_EQ(buf[1], 0xC8U);
      ASSERT_EQ(buf[2], 0u);
      ASSERT_EQ(buf[3], 0x90U);
      ASSERT_EQ(buf[4], 0x1U);
   }

   TEST(Serializer, PackDynamicUInt32ArrayWithUint8Length)
   {
      std::size_t const element_size{ UINT32_SIZE };
      std::size_t const max_array_length{ 4 };
      std::array<std::uint8_t, UINT8_SIZE + max_array_length * element_size> buf;
      std::memset(buf.data(), 0, buf.size());
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto av = dtl::make_av({
         dtl::make_sv<std::uint32_t>(200000),
         dtl::make_sv<std::uint32_t>(400000)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint32(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), UINT8_SIZE + element_size * 2);
      ASSERT_EQ(buf[0], 2U);
      ASSERT_EQ(buf[1], 0x40U);
      ASSERT_EQ(buf[2], 0x0DU);
      ASSERT_EQ(buf[3], 0x03U);
      ASSERT_EQ(buf[4], 0x00U);
      ASSERT_EQ(buf[5], 0x80U);
      ASSERT_EQ(buf[6], 0x1AU);
      ASSERT_EQ(buf[7], 0x06U);
      ASSERT_EQ(buf[8], 0x00U);
   }

   TEST(Serializer, PackDynamicInt8ArrayWithUint8Length)
   {
      std::size_t const element_size{ INT8_SIZE };
      std::size_t const max_array_length{ 4 };
      std::array<std::uint8_t, UINT8_SIZE + max_array_length * element_size> buf = { 0, 0, 0, 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto av = dtl::make_av({
         dtl::make_sv<std::int32_t>(-100),
         dtl::make_sv<std::int32_t>(100)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_int8(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), UINT8_SIZE + element_size * 2);
      ASSERT_EQ(buf[0], 2U);
      ASSERT_EQ(buf[1], 0x9CU);
      ASSERT_EQ(buf[2], 0x64U);
   }


   TEST(Serializer, PackDynamicInt16ArrayWithUint8Length)
   {
      std::size_t const element_size{ INT16_SIZE };
      std::size_t const max_array_length{ 4 };
      std::array<std::uint8_t, UINT8_SIZE + max_array_length * element_size> buf = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto av = dtl::make_av({
         dtl::make_sv<std::int32_t>(-1000),
         dtl::make_sv<std::int32_t>(1000)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_int16(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), UINT8_SIZE + element_size * 2);
      ASSERT_EQ(buf[0], 2U);
      ASSERT_EQ(buf[1], 0x18U);
      ASSERT_EQ(buf[2], 0xFCU);
      ASSERT_EQ(buf[3], 0xE8U);
      ASSERT_EQ(buf[4], 0x03U);
   }

   TEST(Serializer, PackDynamicInt32ArrayWithUint8Length)
   {
      std::size_t const element_size{ INT32_SIZE };
      std::size_t const max_array_length{ 4 };
      std::array<std::uint8_t, UINT8_SIZE + max_array_length * element_size> buf;
      std::memset(buf.data(), 0, buf.size());
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto av = dtl::make_av({
         dtl::make_sv<std::int32_t>(-200000),
         dtl::make_sv<std::int32_t>(200000)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_int32(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), UINT8_SIZE + element_size * 2);
      ASSERT_EQ(buf[0], 2U);
      ASSERT_EQ(buf[1], 0xC0U);
      ASSERT_EQ(buf[2], 0xF2U);
      ASSERT_EQ(buf[3], 0xFCU);
      ASSERT_EQ(buf[4], 0xFFU);
      ASSERT_EQ(buf[5], 0x40U);
      ASSERT_EQ(buf[6], 0x0DU);
      ASSERT_EQ(buf[7], 0x03U);
      ASSERT_EQ(buf[8], 0x00U);
   }

   TEST(Serializer, PackDynamicBoolArrayWithUint8Length)
   {
      std::size_t const element_size{ INT8_SIZE };
      std::size_t const max_array_length{ 4 };
      std::array<std::uint8_t, UINT8_SIZE + max_array_length * element_size> buf = { 0, 0, 0, 0, 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto av = dtl::make_av({
         dtl::make_sv<bool>(false),
         dtl::make_sv<bool>(true),
         dtl::make_sv<bool>(true)
         });
      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_bool(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), UINT8_SIZE + element_size * 3);
      ASSERT_EQ(buf[0], 3U);
      ASSERT_EQ(buf[1], 0u);
      ASSERT_EQ(buf[2], 1u);
      ASSERT_EQ(buf[3], 1u);
   }

   TEST(Serializer, PackDynamicByteArrayWithUint8Length)
   {
      std::size_t const element_size{ BYTE_SIZE };
      std::size_t const max_array_length{ 4 };
      std::array<std::uint8_t, UINT8_SIZE + max_array_length * element_size> buf;
      std::memset(buf.data(), 0, buf.size());
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ByteArray data{ 1u, 2u, 3u, 4u };
      auto sv = dtl::make_sv(data);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_byte_array(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), UINT8_SIZE + element_size * 4);
      ASSERT_EQ(buf[0], 4U);
      ASSERT_EQ(buf[1], 1u);
      ASSERT_EQ(buf[2], 2u);
      ASSERT_EQ(buf[3], 3u);
      ASSERT_EQ(buf[4], 4u);
   }

   TEST(Serializer, PackDynamicByteArrayWithUint16Length)
   {
      std::size_t const element_size{ BYTE_SIZE };
      std::size_t const max_array_length{ 1023 };
      std::array<std::uint8_t, UINT16_SIZE + max_array_length * element_size> buf;
      std::memset(buf.data(), 0, buf.size());
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ByteArray data(max_array_length);
      std::uint8_t value = 0u;
      for (std::size_t i = 0; i < max_array_length; i++)
      {
         data[i] = value++;
      }
      ASSERT_EQ(data.size(), max_array_length);
      ASSERT_EQ(data[max_array_length - 1], (max_array_length % 256) - 1);
      auto sv = dtl::make_sv(data);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_byte_array(max_array_length, apx::SizeType::UInt16), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 0xFFU);
      ASSERT_EQ(buf[1], 0x03u);
      value = 0u;
      for (std::size_t i = 0; i < max_array_length; i++)
      {
         ASSERT_EQ(buf[2+i], value++);
      }
   }

   TEST(Serializer, PackDynamicStringWithUint8Length)
   {
      std::size_t const element_size{ CHAR_SIZE };
      std::size_t const max_array_length{ 32 };
      std::array<std::uint8_t, UINT8_SIZE + max_array_length * element_size> buf;
      std::memset(buf.data(), 0, buf.size());
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto sv = dtl::make_sv<std::string>("Name");
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_char(max_array_length, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), UINT8_SIZE + element_size*4);
      ASSERT_EQ(buf[0], 0x04U);
      ASSERT_EQ(buf[1], 'N');
      ASSERT_EQ(buf[2], 'a');
      ASSERT_EQ(buf[3], 'm');
      ASSERT_EQ(buf[4], 'e');
   }

   TEST(Serializer, PackStringsInRecord)
   {
      std::size_t const element_size{ CHAR_SIZE };
      std::array<std::uint8_t, (10+10) * element_size> buf;
      std::memset(buf.data(), 0, buf.size());
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto hv = dtl::make_hv({
         std::make_pair("First", dtl::make_sv("Hello")),
         std::make_pair("Second", dtl::make_sv("APX"))
         });
      ASSERT_EQ(serializer.set_value(hv), APX_NO_ERROR);
      ASSERT_EQ(serializer.record_select("First", false), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_char(10, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.record_select("Second", false), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_char(10, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
   }

   TEST(Serializer, PackDynamicStringsInRecord)
   {
      std::array<std::uint8_t, UINT8_SIZE + 10 + UINT8_SIZE + 10> buf;
      std::memset(buf.data(), 0, buf.size());
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto hv = dtl::make_hv({
         std::make_pair("First", dtl::make_sv("Hello")),
         std::make_pair("Second", dtl::make_sv("APX"))
         });
      ASSERT_EQ(serializer.set_value(hv), APX_NO_ERROR);
      ASSERT_EQ(serializer.record_select("First", false), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_char(10, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(serializer.record_select("Second", false), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_char(10, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 1+10+1+3);
      ASSERT_EQ(buf[0], 5u);
      ASSERT_EQ(buf[1], 'H');
      ASSERT_EQ(buf[2], 'e');
      ASSERT_EQ(buf[3], 'l');
      ASSERT_EQ(buf[4], 'l');
      ASSERT_EQ(buf[5], 'o');
      ASSERT_EQ(buf[6], 0);
      ASSERT_EQ(buf[7], 0);
      ASSERT_EQ(buf[8], 0);
      ASSERT_EQ(buf[9], 0);
      ASSERT_EQ(buf[10], 0);
      ASSERT_EQ(buf[11], 3u);
      ASSERT_EQ(buf[12], 'A');
      ASSERT_EQ(buf[13], 'P');
      ASSERT_EQ(buf[14], 'X');
      ASSERT_EQ(buf[15], 0);
      ASSERT_EQ(buf[16], 0);
      ASSERT_EQ(buf[17], 0);
      ASSERT_EQ(buf[18], 0);
      ASSERT_EQ(buf[19], 0);
      ASSERT_EQ(buf[20], 0);
      ASSERT_EQ(buf[21], 0);
   }

   TEST(Serializer, PackDynamicUint16ArrayInRecord)
   {
      //DATA SIGNATURE: {"First"S[5*]"Second"C}
      std::array<std::uint8_t, UINT8_SIZE + UINT16_SIZE*5 + UINT8_SIZE> buf;
      std::memset(buf.data(), 0, buf.size());
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto hv = dtl::make_hv({
         std::make_pair("First", dtl::make_av_dv({
            dtl::make_sv<std::uint32_t>(1000),
            dtl::make_sv<std::uint32_t>(2000),
         })),
         std::make_pair("Second", dtl::make_sv_dv<std::uint32_t>(255))
         });
      ASSERT_EQ(serializer.set_value(hv), APX_NO_ERROR);
      ASSERT_EQ(serializer.record_select("First", false), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint16(5, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(serializer.record_select("Second", true), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), buf.size());
      ASSERT_EQ(buf[0], 2u);
      ASSERT_EQ(buf[1], 0xE8U);
      ASSERT_EQ(buf[2], 0x03U);
      ASSERT_EQ(buf[3], 0xD0);
      ASSERT_EQ(buf[4], 0x07);
      ASSERT_EQ(buf[5], 0);
      ASSERT_EQ(buf[6], 0);
      ASSERT_EQ(buf[7], 0);
      ASSERT_EQ(buf[8], 0);
      ASSERT_EQ(buf[9], 0);
      ASSERT_EQ(buf[10], 0);
      ASSERT_EQ(buf[11], 0xFF);
   }

   TEST(Serializer, PackUInt8QueuedElement)
   {
      //DATA SIGNATURE: C:Q[10]
      std::array<std::uint8_t, UINT8_SIZE + UINT8_SIZE * 5> buf;
      std::memset(buf.data(), 0, buf.size());
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(serializer.queued_write_begin(UINT8_SIZE, 10, true), APX_NO_ERROR);
      auto sv = dtl::make_sv<std::uint32_t>(7);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(serializer.queued_write_end(), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), UINT8_SIZE*2);
      ASSERT_EQ(buf[0], 1u);
      ASSERT_EQ(buf[1], 7u);
   }

   TEST(Serializer, PackMultipleUInt8QueuedElements)
   {
      //DATA SIGNATURE: C:Q[10]
      std::array<std::uint8_t, UINT8_SIZE + UINT8_SIZE * 5> buf;
      std::memset(buf.data(), 0, buf.size());
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(serializer.queued_write_begin(UINT8_SIZE, 5, true), APX_NO_ERROR);
      auto sv = dtl::make_sv<std::uint32_t>(7);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0, apx::SizeType::UInt8), APX_NO_ERROR);
      sv->set((std::uint32_t) 8);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0, apx::SizeType::UInt8), APX_NO_ERROR);
      sv->set((std::uint32_t) 9);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0, apx::SizeType::UInt8), APX_NO_ERROR);
      ASSERT_EQ(serializer.queued_write_end(), APX_NO_ERROR);
      ASSERT_EQ(serializer.bytes_written(), UINT8_SIZE * 4);
      ASSERT_EQ(buf[0], 3u);
      ASSERT_EQ(buf[1], 7u);
      ASSERT_EQ(buf[2], 8u);
      ASSERT_EQ(buf[3], 9u);
   }

}