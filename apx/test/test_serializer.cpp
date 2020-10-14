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
      ASSERT_EQ(serializer.check_value_range_uint32(0u, 7u), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.record_select("Second", false), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_uint32(0u, 3u), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0u, apx::SizeType::None), APX_NO_ERROR);
      ASSERT_EQ(serializer.record_select("Third", false), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_uint32(0u, 3u), APX_NO_ERROR);
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
      std::array<std::uint8_t, UINT16_SIZE*5> buf = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
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
      ASSERT_EQ(serializer.bytes_written(), UINT16_SIZE*5);
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
      ASSERT_EQ(serializer.check_value_range_uint32(0, 999), APX_NO_ERROR);
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
      ASSERT_EQ(serializer.check_value_range_uint32(0, 99999UL), APX_NO_ERROR);
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

}