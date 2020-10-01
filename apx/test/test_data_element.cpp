#include "pch.h"
#include "cpp-apx/data_element.h"

namespace apx_test
{

   TEST(DataElement, CreateEmptyDataElement)
   {
      apx::DataElement elem;
      EXPECT_EQ(elem.get_type_code(), apx::TypeCode::None);
   }

   TEST(DataElement, CreateUInt8DataElement)
   {
      apx::DataElement elem(apx::TypeCode::UInt8);
      EXPECT_EQ(elem.get_type_code(), apx::TypeCode::UInt8);
      EXPECT_FALSE(elem.is_array());
      EXPECT_FALSE(elem.has_limits());
   }

   TEST(DataElement, CreateUInt8DataElementWithLimits)
   {
      apx::DataElement elem(apx::TypeCode::UInt8, 0u, 7u);
      EXPECT_EQ(elem.get_type_code(), apx::TypeCode::UInt8);
      EXPECT_FALSE(elem.is_array());
      EXPECT_TRUE(elem.has_limits());
      auto const [lower_limit, upper_limit] = elem.get_limits_unsigned();
      EXPECT_EQ(lower_limit, (uint32_t)0u);
      EXPECT_EQ(upper_limit, (uint32_t)7u);
   }

   TEST(DataElement, CreateUInt8ArrayDataElement)
   {
      apx::DataElement elem(apx::TypeCode::UInt8, 10u);
      EXPECT_EQ(elem.get_type_code(), apx::TypeCode::UInt8);
      EXPECT_FALSE(elem.has_limits());
      EXPECT_TRUE(elem.is_array());
      EXPECT_EQ(elem.get_array_length(), 10u);
   }

   TEST(DataElement, CreateUInt8ArrayDataElementWithLimits)
   {
      apx::DataElement elem{ apx::TypeCode::UInt8, 0u, 7u, 10u };
      EXPECT_EQ(elem.get_type_code(), apx::TypeCode::UInt8);
      EXPECT_TRUE(elem.has_limits());
      auto const [lower_limit, upper_limit] = elem.get_limits_unsigned();
      EXPECT_EQ(lower_limit, (uint32_t)0u);
      EXPECT_EQ(upper_limit, (uint32_t)7u);
      EXPECT_TRUE(elem.is_array());
      EXPECT_EQ(elem.get_array_length(), 10u);
   }


   TEST(DataElement, CreateInt8DataElement)
   {
      apx::DataElement elem(apx::TypeCode::Int8);
      EXPECT_EQ(elem.get_type_code(), apx::TypeCode::Int8);
      EXPECT_FALSE(elem.has_limits());
   }

   TEST(DataElement, CreateInt8DataElementWithLimits)
   {
      {
         apx::DataElement elem(apx::TypeCode::Int8, -10, 10);
         EXPECT_EQ(elem.get_type_code(), apx::TypeCode::Int8);
         EXPECT_TRUE(elem.has_limits());
         auto const [lower_limit, upper_limit] = elem.get_limits_signed();
         EXPECT_EQ(lower_limit, (int32_t)-10);
         EXPECT_EQ(upper_limit, (int32_t)10);
      }

      //Try again but this time set limits only after object has been constructed
      {
         apx::DataElement elem(apx::TypeCode::Int8);
         EXPECT_FALSE(elem.has_limits());
         elem.set_limits((int32_t)0, (int32_t)10);
         EXPECT_TRUE(elem.has_limits());
         auto const [lower_limit, upper_limit] = elem.get_limits_signed();
         EXPECT_EQ(lower_limit, (int32_t)0);
         EXPECT_EQ(upper_limit, (int32_t)10);
      }
   }

   TEST(DataElement, CreateInt8ArrayDataElementWithLimits)
   {
      apx::DataElement elem{ apx::TypeCode::Int8, -100, 100, 20u };
      EXPECT_EQ(elem.get_type_code(), apx::TypeCode::Int8);
      EXPECT_TRUE(elem.has_limits());
      auto const [lower_limit, upper_limit] = elem.get_limits_signed();
      EXPECT_EQ(lower_limit, (int32_t)-100);
      EXPECT_EQ(upper_limit, (int32_t)100);
      EXPECT_TRUE(elem.is_array());
      EXPECT_EQ(elem.get_array_length(), 20u);
   }

   TEST(DataElement, CreateRecord_UInt8_Uint16)
   {
      apx::DataElement elem{ apx::TypeCode::Record };
      elem.append(std::make_unique<apx::DataElement>(apx::TypeCode::UInt8));
      elem.append(std::make_unique<apx::DataElement>(apx::TypeCode::UInt16));
      EXPECT_EQ(elem.get_num_child_elements(), 2u);
      auto child_elem = elem.get_child_at(0);
      EXPECT_NE(child_elem, nullptr);
      EXPECT_EQ(child_elem->get_type_code(), apx::TypeCode::UInt8);
      child_elem = elem.get_child_at(1);
      EXPECT_NE(child_elem, nullptr);
      EXPECT_EQ(child_elem->get_type_code(), apx::TypeCode::UInt16);
   }

   TEST(DataElement, DeriveProperInitValue_UInt8)
   {
      apx::DataElement elem{ apx::TypeCode::UInt8 };
      dtl::Scalar* sv = new dtl::Scalar((uint32_t)7u);
      dtl::DynamicValue parsed{ sv };
      dtl::DynamicValue derived;

      EXPECT_EQ(elem.derive_proper_init_value(parsed, derived), APX_NO_ERROR);
      dtl::Value* dv = derived.get();
      EXPECT_NE(dv, nullptr);
      EXPECT_EQ(dv->dv_type(), dtl::ValueType::Scalar);
      sv = dynamic_cast<dtl::Scalar*>(dv);
      EXPECT_NE(sv, nullptr);
      bool ok;
      EXPECT_EQ(sv->to_u32(ok), 7u);
      EXPECT_TRUE(ok);
   }

   TEST(DataElement, DeriveProperInitValue_Int8)
   {
      apx::DataElement elem{ apx::TypeCode::Int8 };
      dtl::Scalar* sv = new dtl::Scalar((int32_t)-1);
      dtl::DynamicValue parsed{ sv };
      dtl::DynamicValue derived;

      EXPECT_EQ(elem.derive_proper_init_value(parsed, derived), APX_NO_ERROR);
      dtl::Value* dv = derived.get();
      EXPECT_NE(dv, nullptr);
      EXPECT_EQ(dv->dv_type(), dtl::ValueType::Scalar);
      sv = dynamic_cast<dtl::Scalar*>(dv);
      EXPECT_NE(sv, nullptr);
      bool ok;
      EXPECT_EQ(sv->to_i32(ok), -1);
      EXPECT_TRUE(ok);
   }

   TEST(DataElement, DeriveProperInitValue_UInt16)
   {
      apx::DataElement elem{ apx::TypeCode::UInt16 };
      dtl::Scalar* sv = new dtl::Scalar((uint32_t)65535u);
      dtl::DynamicValue parsed{ sv };
      dtl::DynamicValue derived;

      EXPECT_EQ(elem.derive_proper_init_value(parsed, derived), APX_NO_ERROR);
      dtl::Value* dv = derived.get();
      EXPECT_NE(dv, nullptr);
      EXPECT_EQ(dv->dv_type(), dtl::ValueType::Scalar);
      sv = dynamic_cast<dtl::Scalar*>(dv);
      EXPECT_NE(sv, nullptr);
      bool ok;
      EXPECT_EQ(sv->to_u32(ok), 65535u);
      EXPECT_TRUE(ok);
   }

   TEST(DataElement, DeriveProperInitValue_Int16)
   {
      apx::DataElement elem{ apx::TypeCode::Int16 };
      dtl::Scalar* sv = new dtl::Scalar((int32_t) 0);
      dtl::DynamicValue parsed{ sv };
      dtl::DynamicValue derived;

      EXPECT_EQ(elem.derive_proper_init_value(parsed, derived), APX_NO_ERROR);
      dtl::Value* dv = derived.get();
      EXPECT_NE(dv, nullptr);
      EXPECT_EQ(dv->dv_type(), dtl::ValueType::Scalar);
      sv = dynamic_cast<dtl::Scalar*>(dv);
      EXPECT_NE(sv, nullptr);
      bool ok;
      EXPECT_EQ(sv->to_i32(ok), 0);
      EXPECT_TRUE(ok);
   }

   TEST(DataElement, DeriveProperInitValue_UInt8Array)
   {
      apx::DataElement elem{ apx::TypeCode::UInt8 };
      elem.set_array_length(3);
      dtl::Array* original_av = new dtl::Array();
      dtl::DynamicValue parsed{ original_av };
      dtl::DynamicValue derived;
      original_av->push(dtl::make_sv_uint32(3));
      original_av->push(dtl::make_sv_uint32(7));
      original_av->push(dtl::make_sv_uint32(15));

      EXPECT_EQ(elem.derive_proper_init_value(parsed, derived), APX_NO_ERROR);
      dtl::Value* dv = derived.get();
      EXPECT_NE(dv, nullptr);
      EXPECT_EQ(dv->dv_type(), dtl::ValueType::Array);
      auto derived_av = dynamic_cast<dtl::Array*>(dv);
      EXPECT_NE(derived_av, nullptr);
      EXPECT_NE(derived_av, original_av);
      EXPECT_EQ(derived_av->length(), 3);
      bool ok;
      auto child_value = derived_av->at(0);
      EXPECT_EQ(child_value->dv_type(), dtl::ValueType::Scalar);
      auto sv = dynamic_cast<dtl::Scalar*>(child_value.get());
      EXPECT_EQ(sv->to_u32(ok), 3u);
      EXPECT_TRUE(ok);

      child_value = derived_av->at(1);
      EXPECT_EQ(child_value->dv_type(), dtl::ValueType::Scalar);
      sv = dynamic_cast<dtl::Scalar*>(child_value.get());
      EXPECT_EQ(sv->to_u32(ok), 7u);
      EXPECT_TRUE(ok);

      child_value = derived_av->at(2);
      EXPECT_EQ(child_value->dv_type(), dtl::ValueType::Scalar);
      sv = dynamic_cast<dtl::Scalar*>(child_value.get());
      EXPECT_EQ(sv->to_u32(ok), 15u);
      EXPECT_TRUE(ok);
   }
}