#include "pch.h"
#include <cstring>
#include "cpp-apx/attribute_parser.h"

namespace apx_test
{
   TEST(AttributeParser, ParseInitValue_Zero)
   {
      const char* begin = "0";
      const char* end = begin + strlen(begin);
      apx::AttributeParser parser;
      dtl::DynamicValue dv = nullptr;
      bool ok = false;

      const char *result = parser.parse_initializer(begin, end, dv);
      ASSERT_EQ(result, end);
      ASSERT_EQ(dv.use_count(), 1);
      dtl::Scalar* sv = dynamic_cast<dtl::Scalar*>(dv.get());
      ASSERT_EQ(sv->sv_type(), dtl::ScalarType::Int32);
      ASSERT_EQ(sv->to_i32(ok), (int32_t)0);
      ASSERT_TRUE(ok);
      dtl::ScalarValue sv2 = std::dynamic_pointer_cast<dtl::Scalar>(dv);
      ASSERT_EQ(dv.use_count(), 2);
      ASSERT_EQ(sv2.use_count(), 2);
   }

   TEST(AttributeParser, ParseInitValue_MinusOne)
   {
      const char* begin1 = "-";
      const char* end1 = begin1 + strlen(begin1);
      const char* begin2 = "-1";
      const char* end2 = begin2 + strlen(begin2);

      apx::AttributeParser parser;
      dtl::DynamicValue dv = nullptr;
      bool ok = false;

      const char* result = parser.parse_initializer(begin1, end1, dv);
      ASSERT_EQ(result, nullptr);

      result = parser.parse_initializer(begin2, end2, dv);
      ASSERT_EQ(result, end2);
      dtl::Scalar* sv = dynamic_cast<dtl::Scalar*>(dv.get());
      ASSERT_EQ(sv->sv_type(), dtl::ScalarType::Int32);
      ASSERT_EQ(sv->to_i32(ok), -1);
      ASSERT_TRUE(ok);
   }

   TEST(AttributeParser, ParseInitValue_UInt32Max)
   {
      const char* begin = "0xFFFFFFFF";
      const char* end = begin + strlen(begin);

      apx::AttributeParser parser;
      dtl::DynamicValue dv = nullptr;
      bool ok = false;

      const char *result = parser.parse_initializer(begin, end, dv);
      ASSERT_EQ(result, end);
      dtl::Scalar* sv = dynamic_cast<dtl::Scalar*>(dv.get());
      ASSERT_EQ(sv->sv_type(), dtl::ScalarType::UInt32);
      ASSERT_EQ(sv->to_u32(ok), 0xFFFFFFFF);
      ASSERT_TRUE(ok);
   }

   TEST(AttributeParser, ParseInitValue_Int32Max)
   {
      const char* begin = "2147483647";
      const char* end = begin + strlen(begin);

      apx::AttributeParser parser;
      dtl::DynamicValue dv = nullptr;
      bool ok = false;

      const char* result = parser.parse_initializer(begin, end, dv);
      ASSERT_EQ(result, end);
      dtl::Scalar* sv = dynamic_cast<dtl::Scalar*>(dv.get());
      ASSERT_EQ(sv->sv_type(), dtl::ScalarType::Int32);
      ASSERT_EQ(sv->to_i32(ok), INT32_MAX);
      ASSERT_TRUE(ok);
   }

   TEST(AttributeParser, ParseInitValue_Int32Min)
   {
      const char* begin = "-2147483648";
      const char* end = begin + strlen(begin);

      apx::AttributeParser parser;
      dtl::DynamicValue dv = nullptr;
      bool ok = false;

      const char* result = parser.parse_initializer(begin, end, dv);
      ASSERT_EQ(result, end);
      dtl::Scalar* sv = dynamic_cast<dtl::Scalar*>(dv.get());
      ASSERT_EQ(sv->sv_type(), dtl::ScalarType::Int32);
      ASSERT_EQ(sv->to_i32(ok), INT32_MIN);
      ASSERT_TRUE(ok);
   }

   TEST(AttributeParser, ParseInitValue_Int32List)
   {
      const char* begin = "{3, 4}";
      const char* end = begin + strlen(begin);
      bool ok = false;
      apx::AttributeParser parser;
      dtl::DynamicValue dv = nullptr;
      const char* result = parser.parse_initializer(begin, end, dv);
      ASSERT_EQ(result, end);
      auto av = dynamic_cast<dtl::Array*>(dv.get());
      ASSERT_NE(av, nullptr);
      ASSERT_EQ(av->length(), 2);
      auto sv = dynamic_cast<dtl::Scalar*>(av->at(0).get());
      ASSERT_NE(sv, nullptr);
      ASSERT_EQ(sv->to_i32(ok), 3);
      ASSERT_TRUE(ok);
      sv = dynamic_cast<dtl::Scalar*>(av->at(1).get());
      ASSERT_NE(sv, nullptr);
      ok = false;
      ASSERT_EQ(sv->to_i32(ok), 4);
      ASSERT_TRUE(ok);
   }

   TEST(AttributeParser, ParseInitValue_Int32ListList)
   {
      const char* begin = "{ {1, 2}, {3,4} }";
      const char* end = begin + strlen(begin);
      bool ok = false;
      apx::AttributeParser parser;
      dtl::DynamicValue dv = nullptr;
      const char* result = parser.parse_initializer(begin, end, dv);
      ASSERT_EQ(result, end);
      auto av = dynamic_cast<dtl::Array*>(dv.get());
      ASSERT_NE(av, nullptr);
      ASSERT_EQ(av->length(), 2);
      auto inner_av = dynamic_cast<dtl::Array*>(av->at(0).get());
      ASSERT_NE(inner_av, nullptr);
      ASSERT_EQ(inner_av->length(), 2);
      auto sv = dynamic_cast<dtl::Scalar*>(inner_av->at(0).get()); //[0,0]
      ASSERT_EQ(sv->to_i32(ok), 1);
      ASSERT_TRUE(ok);
      sv = dynamic_cast<dtl::Scalar*>(inner_av->at(1).get()); //[0,1]
      ASSERT_EQ(sv->to_i32(ok), 2);
      ASSERT_TRUE(ok);
      inner_av = dynamic_cast<dtl::Array*>(av->at(1).get());
      ASSERT_NE(inner_av, nullptr);
      ASSERT_EQ(inner_av->length(), 2);
      sv = dynamic_cast<dtl::Scalar*>(inner_av->at(0).get()); //[1,0]
      ASSERT_EQ(sv->to_i32(ok), 3);
      ASSERT_TRUE(ok);
      sv = dynamic_cast<dtl::Scalar*>(inner_av->at(1).get()); //[1,1]
      ASSERT_EQ(sv->to_i32(ok), 4);
      ASSERT_TRUE(ok);
   }


   TEST(AttributeParser, ParseInitValue_EmptyString)
   {
      const char* begin = "\"\"";
      const char* end = begin + strlen(begin);
      apx::AttributeParser parser;
      dtl::DynamicValue dv = nullptr;
      const char* result = parser.parse_initializer(begin, end, dv);
      ASSERT_EQ(result, end);
      auto sv = dynamic_cast<dtl::Scalar*>(dv.get());
      ASSERT_NE(sv, nullptr);
      ASSERT_EQ(sv->to_string(), "");
   }

   TEST(AttributeParser, ParseInitValue_UTF8String)
   {
      const char* begin = "\"\u00c4\u00c5\u00d6\"";
      const char* end = begin + strlen(begin);
      apx::AttributeParser parser;
      dtl::DynamicValue dv = nullptr;
      const char* result = parser.parse_initializer(begin, end, dv);
      ASSERT_EQ(result, end);
      auto sv = dynamic_cast<dtl::Scalar*>(dv.get());
      ASSERT_NE(sv, nullptr);
      ASSERT_EQ(sv->to_string(), "\u00c4\u00c5\u00d6");
   }

   TEST(AttributeParser, ScalarInitValueWithExtraCharacters)
   {
      const char* begin1 = "255, ";
      const char* end1 = begin1 + strlen(begin1);
      const char* begin2 = "{3,\"None\"}, P";
      const char* end2 = begin2 + strlen(begin2);
      apx::AttributeParser parser;
      dtl::DynamicValue dv = nullptr;
      bool ok = false;
      const char* result = parser.parse_initializer(begin1, end1, dv);
      ASSERT_EQ(result, end1-2);
      auto sv = dynamic_cast<dtl::Scalar*>(dv.get());
      ASSERT_NE(sv, nullptr);
      ASSERT_EQ(sv->to_i32(ok), 255);
      ASSERT_TRUE(ok);
      result = parser.parse_initializer(begin2, end2, dv);
      ASSERT_EQ(result, end2 - 3);
      auto av = dynamic_cast<dtl::Array*>(dv.get());
      ASSERT_NE(av, nullptr);
      ASSERT_EQ(av->length(), 2);
      sv = dynamic_cast<dtl::Scalar*>(av->at(0).get());
      ASSERT_NE(sv, nullptr);
      ASSERT_EQ(sv->to_i32(ok), 3);
      ASSERT_TRUE(ok);
      sv = dynamic_cast<dtl::Scalar*>(av->at(1).get());
      ASSERT_NE(sv, nullptr);
      ASSERT_EQ(sv->to_string(), "None");
   }
}

TEST(AttributeParser, ParsePortInitValues)
{
   const char* begin1 = "=3";
   const char* end1 = begin1 + strlen(begin1);
   const char* begin2 = "={7, \"\"}, P";
   const char* end2 = begin2 + strlen(begin2);
   bool ok = false;
   apx::AttributeParser parser;
   apx::PortAttributes attr1;
   ASSERT_FALSE(attr1.has_init_value());
   const char* result = parser.parse_port_attributes(begin1, end1, attr1);
   ASSERT_EQ(result, end1);
   ASSERT_TRUE(attr1.has_init_value());
   ASSERT_FALSE(attr1.is_parameter);
   dtl::Value* dv = attr1.init_value.get();
   ASSERT_EQ(dv->dv_type(), dtl::ValueType::Scalar);
   dtl::Scalar* sv = dynamic_cast<dtl::Scalar*>(dv);
   ASSERT_EQ(sv->to_i32(ok), 3);
   ASSERT_TRUE(ok);

   apx::PortAttributes attr2;
   ASSERT_FALSE(attr2.has_init_value());
   result = parser.parse_port_attributes(begin2, end2, attr2);
   ASSERT_EQ(result, end2);
   ASSERT_TRUE(attr2.has_init_value());
   ASSERT_TRUE(attr2.is_parameter);
   dv = attr2.init_value.get();
   ASSERT_EQ(dv->dv_type(), dtl::ValueType::Array);
   dtl::Array* av = dynamic_cast<dtl::Array*>(dv);
   ASSERT_NE(av, nullptr);
   sv = dynamic_cast<dtl::Scalar*>(av->at(0).get());
   ASSERT_NE(sv, nullptr);
   ASSERT_EQ(sv->to_i32(ok), 7);
   ASSERT_TRUE(ok);
   sv = dynamic_cast<dtl::Scalar*>(av->at(1).get());
   ASSERT_NE(sv, nullptr);
   ASSERT_EQ(sv->sv_type(), dtl::ScalarType::String);
   ASSERT_EQ(sv->to_string(), "");

}

TEST(AttributeParser, TraditionalValueTable)
{
   const char* begin = "VT(\"OnOff_Off\", \"OnOff_On\", \"OnOff_Error\", \"OnOff_NotAvailable\")";
   const char* end = begin + strlen(begin);
   apx::AttributeParser parser;
   apx::TypeAttributes attr;
   const char* result = parser.parse_type_attributes(begin, end, attr);
   ASSERT_EQ(result, end);
   ASSERT_EQ(attr.computations.size(), 1);
   auto vt = dynamic_cast<apx::ValueTable*>(attr.computations.at(0).get());
   ASSERT_NE(vt, nullptr);
   ASSERT_EQ(vt->lower_range.u32, 0u);
   ASSERT_EQ(vt->upper_range.u32, 3u);
   EXPECT_EQ(vt->values[0], "OnOff_Off");
   EXPECT_EQ(vt->values[1], "OnOff_On");
   EXPECT_EQ(vt->values[2], "OnOff_Error");
   EXPECT_EQ(vt->values[3], "OnOff_NotAvailable");
}


TEST(AttributeParser, ValueTableWithOffset)
{
   const char* begin = "VT(4, \"OnOff_Off\", \"OnOff_On\", \"OnOff_Error\", \"OnOff_NotAvailable\")";
   const char* end = begin + strlen(begin);
   apx::AttributeParser parser;
   apx::TypeAttributes attr;
   const char* result = parser.parse_type_attributes(begin, end, attr);
   ASSERT_EQ(result, end);
   ASSERT_EQ(attr.computations.size(), 1);
   auto vt = dynamic_cast<apx::ValueTable*>(attr.computations.at(0).get());
   ASSERT_NE(vt, nullptr);
   ASSERT_EQ(vt->lower_range.u32, 4u);
   ASSERT_EQ(vt->upper_range.u32, 7u);
}

TEST(AttributeParser, ValueTableWithNegativeOffset)
{
   const char* begin = "VT(-3, \"ErrorCode3\", \"ErrorCode2\", \"ErrorCode1\", \"NoError\")";
   const char* end = begin + strlen(begin);
   apx::AttributeParser parser;
   apx::TypeAttributes attr;
   const char* result = parser.parse_type_attributes(begin, end, attr);
   ASSERT_EQ(result, end);
   ASSERT_EQ(attr.computations.size(), 1);
   auto vt = dynamic_cast<apx::ValueTable*>(attr.computations.at(0).get());
   ASSERT_NE(vt, nullptr);
   ASSERT_EQ(vt->lower_range.i32, -3);
   ASSERT_EQ(vt->upper_range.i32, 0);
}

TEST(AttributeParser, ValueTableWithNegativeOffsetExplicit)
{
   const char* begin = "VT(-3, 0, \"ErrorCode3\", \"ErrorCode2\", \"ErrorCode1\", \"NoError\")";
   const char* end = begin + strlen(begin);
   apx::AttributeParser parser;
   apx::TypeAttributes attr;
   const char* result = parser.parse_type_attributes(begin, end, attr);
   ASSERT_EQ(result, end);
   ASSERT_EQ(attr.computations.size(), 1);
   auto vt = dynamic_cast<apx::ValueTable*>(attr.computations.at(0).get());
   ASSERT_NE(vt, nullptr);
   ASSERT_EQ(vt->lower_range.i32, -3);
   ASSERT_EQ(vt->upper_range.i32, 0);
}

TEST(AttributeParser, ValueTableRanges)
{
   const char* begin = "VT(251, 254, \"Error\"), VT(255, \"NotAvailable\")";
   const char* end = begin + strlen(begin);
   apx::AttributeParser parser;
   apx::TypeAttributes attr;
   const char* result = parser.parse_type_attributes(begin, end, attr);
   ASSERT_EQ(result, end);
   ASSERT_EQ(attr.computations.size(), 2);
   auto vt = dynamic_cast<apx::ValueTable*>(attr.computations.at(0).get());
   ASSERT_NE(vt, nullptr);
   ASSERT_EQ(vt->lower_range.u32, 251);
   ASSERT_EQ(vt->upper_range.u32, 254);
   ASSERT_EQ(vt->values.size(), 1);
   ASSERT_EQ(vt->values.at(0), "Error");

   vt = dynamic_cast<apx::ValueTable*>(attr.computations.at(1).get());
   ASSERT_NE(vt, nullptr);
   ASSERT_EQ(vt->lower_range.u32, 255);
   ASSERT_EQ(vt->upper_range.u32, 255);
   ASSERT_EQ(vt->values.size(), 1);
   ASSERT_EQ(vt->values.at(0), "NotAvailable");
}

TEST(AttributeParser, RationalScaling_VehicleSpeed)
{
   const char* begin = "RS(0, 65280, 0, 1, 64, \"km/h\")";
   const char* end = begin + strlen(begin);
   apx::AttributeParser parser;
   apx::TypeAttributes attr;
   const char* result = parser.parse_type_attributes(begin, end, attr);
   ASSERT_EQ(result, end);
   ASSERT_EQ(attr.computations.size(), 1);
   auto rs = dynamic_cast<apx::RationalScaling*>(attr.computations.at(0).get());
   ASSERT_NE(rs, nullptr);
   ASSERT_EQ(rs->lower_range.u32, 0u);
   ASSERT_EQ(rs->upper_range.u32, 65280u);
   ASSERT_EQ(rs->offset, 0.0);
   ASSERT_EQ(rs->numerator, 1);
   ASSERT_EQ(rs->denominator, 64);
   ASSERT_EQ(rs->unit, "km/h");
}

TEST(AttributeParser, CombinedTypeAttribute)
{
   const char* begin = "RS(0, 0xFDFF, 0, 1, 64, \"km/h\"),VT(0xFE00, 0xFEFF, \"Error\"),VT(0xFF00, 0xFFFF, \"NotAvailable\")";
   const char* end = begin + strlen(begin);
   apx::AttributeParser parser;
   apx::TypeAttributes attr;
   const char* result = parser.parse_type_attributes(begin, end, attr);
   ASSERT_EQ(result, end);
   ASSERT_EQ(attr.computations.size(), 3);
   auto rs = dynamic_cast<apx::RationalScaling*>(attr.computations.at(0).get());
   ASSERT_NE(rs, nullptr);
   ASSERT_EQ(rs->lower_range.u32, 0u);
   ASSERT_EQ(rs->upper_range.u32, 65023u);
   ASSERT_EQ(rs->offset, 0.0);
   ASSERT_EQ(rs->numerator, 1);
   ASSERT_EQ(rs->denominator, 64);
   ASSERT_EQ(rs->unit, "km/h");
   auto vt = dynamic_cast<apx::ValueTable*>(attr.computations.at(1).get());
   ASSERT_NE(vt, nullptr);
   ASSERT_EQ(vt->lower_range.u32, 65024u);
   ASSERT_EQ(vt->upper_range.u32, 65279u);
   ASSERT_EQ(vt->values.size(), 1u);
   ASSERT_EQ(vt->values[0], "Error");
   vt = dynamic_cast<apx::ValueTable*>(attr.computations.at(2).get());
   ASSERT_NE(vt, nullptr);
   ASSERT_EQ(vt->lower_range.u32, 65280u);
   ASSERT_EQ(vt->upper_range.u32, 65535u);
   ASSERT_EQ(vt->values.size(), 1u);
   ASSERT_EQ(vt->values[0], "NotAvailable");
}

TEST(AttributeParser, QueueLength)
{
   const char* begin = "Q[10]";
   const char* end = begin + strlen(begin);
   apx::AttributeParser parser;
   apx::PortAttributes attr;
   const char* result = parser.parse_port_attributes(begin, end, attr);
   ASSERT_EQ(result, end);
   ASSERT_EQ(attr.queue_length, 10u);
}

TEST(AttributeParser, EmptyInitializerList)
{
   const char* begin = "={}";
   const char* end = begin + strlen(begin);
   apx::AttributeParser parser;
   apx::PortAttributes attr;
   const char* result = parser.parse_port_attributes(begin, end, attr);
   ASSERT_EQ(result, end);
   ASSERT_TRUE(attr.has_init_value());
   ASSERT_FALSE(attr.is_parameter);
   dtl::Value* dv = attr.init_value.get();
   ASSERT_EQ(dv->dv_type(), dtl::ValueType::Array);
   dtl::Array* av = dynamic_cast<dtl::Array*>(dv);
   ASSERT_EQ(av->length(), 0);
}

TEST(AttributeParser, EmptyInitializerListInRecord)
{
   const char* begin = "={3, {}}";
   const char* end = begin + strlen(begin);
   apx::AttributeParser parser;
   apx::PortAttributes attr;
   const char* result = parser.parse_port_attributes(begin, end, attr);
   ASSERT_EQ(result, end);
   ASSERT_TRUE(attr.has_init_value());
   ASSERT_FALSE(attr.is_parameter);
   ASSERT_EQ(attr.init_value->dv_type(), dtl::ValueType::Array);
   dtl::Array* outer_av = dynamic_cast<dtl::Array*>(attr.init_value.get());
   ASSERT_EQ(outer_av->length(), 2);
   auto dv = outer_av->at(0);
   ASSERT_EQ(dv->dv_type(), dtl::ValueType::Scalar);
   auto sv = dynamic_cast<dtl::Scalar*>(dv.get());
   bool ok;
   ASSERT_EQ(sv->to_u32(ok), 3u);
   ASSERT_TRUE(ok);
   dv = outer_av->at(1);
   ASSERT_EQ(dv->dv_type(), dtl::ValueType::Array);
   auto inner_av = dynamic_cast<dtl::Array*>(dv.get());
   ASSERT_EQ(inner_av->length(), 0);
}

