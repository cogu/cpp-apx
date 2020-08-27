#include "pch.h"
#include "cpp-apx/data_element.h"

TEST(ApxDataElement, CreateEmptyDataElement)
{
   apx::DataElement elem;
   EXPECT_EQ(elem.get_type_code(), apx::TypeCode::None);
}

TEST(ApxDataElement, CreateUInt8DataElement)
{
   apx::DataElement elem(apx::TypeCode::UInt8);
   EXPECT_EQ(elem.get_type_code(), apx::TypeCode::UInt8);
   EXPECT_FALSE(elem.is_array());
   EXPECT_FALSE(elem.has_limits());
}

TEST(ApxDataElement, CreateUInt8DataElementWithLimits)
{
   apx::DataElement elem(apx::TypeCode::UInt8, 0u, 7u);
   EXPECT_EQ(elem.get_type_code(), apx::TypeCode::UInt8);
   EXPECT_FALSE(elem.is_array());
   EXPECT_TRUE(elem.has_limits());
   auto const [lower_limit, upper_limit] = elem.get_limits_unsigned();
   EXPECT_EQ(lower_limit, (uint32_t) 0u);
   EXPECT_EQ(upper_limit, (uint32_t) 7u);
}

TEST(ApxDataElement, CreateUInt8ArrayDataElement)
{
   apx::DataElement elem(apx::TypeCode::UInt8, 10u);
   EXPECT_EQ(elem.get_type_code(), apx::TypeCode::UInt8);
   EXPECT_FALSE(elem.has_limits());
   EXPECT_TRUE(elem.is_array());
   EXPECT_EQ(elem.get_array_length(), 10);
}

TEST(ApxDataElement, CreateUInt8ArrayDataElementWithLimits)
{
   apx::DataElement elem{ apx::TypeCode::UInt8, 0u, 7u, 10u };
   EXPECT_EQ(elem.get_type_code(), apx::TypeCode::UInt8);
   EXPECT_TRUE(elem.has_limits());
   auto const [lower_limit, upper_limit] = elem.get_limits_unsigned();
   EXPECT_EQ(lower_limit, (uint32_t)0u);
   EXPECT_EQ(upper_limit, (uint32_t)7u);
   EXPECT_TRUE(elem.is_array());
   EXPECT_EQ(elem.get_array_length(), 10);
}


TEST(ApxDataElement, CreateInt8DataElement)
{
   apx::DataElement elem(apx::TypeCode::Int8);
   EXPECT_EQ(elem.get_type_code(), apx::TypeCode::Int8);
   EXPECT_FALSE(elem.has_limits());
}

TEST(ApxDataElement, CreateInt8DataElementWithLimits)
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
      elem.set_limits((int32_t) 0, (int32_t) 10);
      EXPECT_TRUE(elem.has_limits());
      auto const [lower_limit, upper_limit] = elem.get_limits_signed();
      EXPECT_EQ(lower_limit, (int32_t) 0);
      EXPECT_EQ(upper_limit, (int32_t) 10);
   }
}

TEST(ApxDataElement, CreateInt8ArrayDataElementWithLimits)
{
   apx::DataElement elem{ apx::TypeCode::Int8, -100, 100, 20u };
   EXPECT_EQ(elem.get_type_code(), apx::TypeCode::Int8);
   EXPECT_TRUE(elem.has_limits());
   auto const [lower_limit, upper_limit] = elem.get_limits_signed();
   EXPECT_EQ(lower_limit, (int32_t) -100);
   EXPECT_EQ(upper_limit, (int32_t) 100);
   EXPECT_TRUE(elem.is_array());
   EXPECT_EQ(elem.get_array_length(), 20);
}

TEST(ApxDataElement, CreateRecord_UInt8_Uint16)
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