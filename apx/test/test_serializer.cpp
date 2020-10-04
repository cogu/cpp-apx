#include "pch.h"
#include "cpp-apx/serializer.h"
#include "dtl.hpp"
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
      dtl::ScalarValue sv = dtl::make_sv_int32(18);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);      
   }

   TEST(Serializer, SetArray)
   {
      Serializer serializer;
      dtl::ArrayValue av = dtl::make_av();
      av->push(dtl::make_sv_int32(1));
      av->push(dtl::make_sv_int32(2));
      av->push(dtl::make_sv_int32(3));

      ASSERT_EQ(serializer.set_value(av), APX_NO_ERROR);
   }

   TEST(Serializer, SetHash)
   {
      Serializer serializer;
      auto hv = dtl::make_hv();
      hv->set("First", dtl::make_sv_int32(1));
      hv->set("Second", dtl::make_sv_int32(2));
      hv->set("Third", dtl::make_sv_int32(3));

      ASSERT_EQ(serializer.set_value(hv), APX_NO_ERROR);
   }

   TEST(Serializer, PackUInt8FromInt8Value)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 1), APX_NO_ERROR);
      dtl::ScalarValue sv = dtl::make_sv_int32(7);
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
      dtl::ScalarValue sv = dtl::make_sv_uint32(255);
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
      auto sv = dtl::make_sv_uint32(6000);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0u, apx::SizeType::None), APX_VALUE_RANGE_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 0u);

   }
   TEST(Serializer, PackUIn8FromNegativeValue)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 1), APX_NO_ERROR);
      auto sv = dtl::make_sv_int32(-1);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.pack_uint8(0u, apx::SizeType::None), APX_VALUE_CONVERSION_ERROR);
      ASSERT_EQ(serializer.bytes_written(), 0u);
   }

   TEST(Serializer, RangeCheckUInt32ValueOutOfRange)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 1), APX_NO_ERROR);
      auto sv = dtl::make_sv_uint32(15);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_uint32(0u, 7u), APX_VALUE_RANGE_ERROR);
   }

   TEST(Serializer, RangeCheckUInt8Value)
   {
      std::array<std::uint8_t, 1> buf = { 0 };
      Serializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 1), APX_NO_ERROR);
      auto sv = dtl::make_sv_uint32(0);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
      ASSERT_EQ(serializer.check_value_range_uint32(0u, 7u), APX_NO_ERROR);
      sv->set((uint32_t)7);
      ASSERT_EQ(serializer.check_value_range_uint32(0u, 7u), APX_NO_ERROR);
      sv->set((uint32_t)8);
      ASSERT_EQ(serializer.check_value_range_uint32(0u, 7u), APX_VALUE_RANGE_ERROR);
   }

}