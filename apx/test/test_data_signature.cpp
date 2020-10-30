#include "pch.h"
#include <string>
#include "cpp-apx/data_signature.h"

using namespace apx;
using namespace std::string_literals;

namespace apx_test
{

   TEST(DataSignature, EncodeDecodeUInt8)
   {
      std::string const expected{ "C" };
      auto test_element = std::make_unique<DataElement>(TypeCode::UInt8);
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_EQ(*test_element, *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeUInt16)
   {
      std::string const expected{ "S" };
      auto test_element = std::make_unique<DataElement>(TypeCode::UInt16);
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_EQ(*test_element, *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeUInt64)
   {
      std::string const expected{ "Q" };
      auto test_element = std::make_unique<DataElement>(TypeCode::UInt64);
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_EQ(*test_element, *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeInt8)
   {
      std::string const expected{ "c" };
      auto test_element = std::make_unique<DataElement>(TypeCode::Int8);
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_EQ(*test_element, *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeInt16)
   {
      std::string const expected{ "s" };
      auto test_element = std::make_unique<DataElement>(TypeCode::Int16);
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_EQ(*test_element, *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeUInt32)
   {
      std::string const expected{ "l" };
      auto test_element = std::make_unique<DataElement>(TypeCode::Int32);
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_EQ(*test_element, *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeInt64)
   {
      std::string const expected{ "q" };
      auto test_element = std::make_unique<DataElement>(TypeCode::Int64);
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_EQ(*test_element, *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeChar)
   {
      std::string const expected{ "a" };
      auto test_element = std::make_unique<DataElement>(TypeCode::Char);
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_EQ(*test_element, *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeChar8)
   {
      std::string const expected{ "A" };
      auto test_element = std::make_unique<DataElement>(TypeCode::Char8);
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_EQ(*test_element, *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeChar16)
   {
      std::string const expected{ "u" };
      auto test_element = std::make_unique<DataElement>(TypeCode::Char16);
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_EQ(*test_element, *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeChar32)
   {
      std::string const expected{ "U" };
      auto test_element = std::make_unique<DataElement>(TypeCode::Char32);
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_EQ(*test_element, *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeBool)
   {
      std::string const expected{ "b" };
      auto test_element = std::make_unique<DataElement>(TypeCode::Bool);
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_EQ(*test_element, *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeByte)
   {
      std::string const expected{ "B" };
      auto test_element = std::make_unique<DataElement>(TypeCode::Byte);
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_EQ(*test_element, *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeUInt8WithLimit)
   {
      std::string const expected{ "C(0,3)" };
      auto test_element = std::make_unique<DataElement>(TypeCode::UInt8, static_cast<uint32_t>(0), static_cast<uint32_t>(3));
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_TRUE( *test_element == *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeUInt16WithLimit)
   {
      std::string const expected{ "S(0,10000)" };
      auto test_element = std::make_unique<DataElement>(TypeCode::UInt16, static_cast<uint32_t>(0), static_cast<uint32_t>(10000));
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_TRUE(*test_element == *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeInt8WithLimit)
   {
      std::string const expected{ "c(-10,10)" };
      auto test_element = std::make_unique<DataElement>(TypeCode::Int8, static_cast<int32_t>(-10), static_cast<int32_t>(10));
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_TRUE(*test_element == *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeInt16WithLimit)
   {
      std::string const expected{ "s(-1000,1000)" };
      auto test_element = std::make_unique<DataElement>(TypeCode::Int16, static_cast<int32_t>(-1000), static_cast<int32_t>(1000));
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_TRUE(*test_element == *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeUInt8Array)
   {
      std::string const expected{ "C[8]" };
      auto test_element = std::make_unique<DataElement>(TypeCode::UInt8);
      test_element->set_array_length(8);
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_TRUE(*test_element == *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeUInt8ArrayWithLimit)
   {
      std::string const expected{ "C(0,7)[10]" };
      auto test_element = std::make_unique<DataElement>(TypeCode::UInt8, static_cast<uint32_t>(0), static_cast<uint32_t>(7));
      test_element->set_array_length(10);
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_TRUE(*test_element == *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeUInt8DynamicArrayWithLimit)
   {
      std::string const expected{ "C(0,7)[10*]" };
      auto test_element = std::make_unique<DataElement>(TypeCode::UInt8, static_cast<uint32_t>(0), static_cast<uint32_t>(7));
      test_element->set_array_length(10);
      test_element->set_dynamic_array();
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_TRUE(*test_element == *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeByteDynamicArray)
   {
      std::string const expected{ "B[4095*]" };
      auto test_element = std::make_unique<DataElement>(TypeCode::Byte);
      test_element->set_array_length(4095);
      test_element->set_dynamic_array();
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_TRUE(*test_element == *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeRecord_Uint8Uint16)
   {
      std::string const expected{ "{\"First\"C(0,3)\"Second\"S}" };
      auto test_element = std::make_unique<DataElement>(TypeCode::Record);
      auto child = std::make_unique<DataElement>(TypeCode::UInt8, static_cast<uint32_t>(0), static_cast<uint32_t>(3));
      child->set_name("First"s);
      test_element->append(std::move(child));
      child = std::make_unique<DataElement>(TypeCode::UInt16);
      child->set_name("Second"s);
      test_element->append(std::move(child));
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_TRUE(*test_element == *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeRecordArray_DynamicStringUint32)
   {
      std::string const expected{ "{\"First\"A[32*]\"Second\"L}[40]" };
      auto test_element = std::make_unique<DataElement>(TypeCode::Record);
      test_element->set_array_length(40u);
      auto child = std::make_unique<DataElement>(TypeCode::Char8);
      child->set_name("First"s);
      child->set_array_length(32);
      child->set_dynamic_array();
      test_element->append(std::move(child));
      child = std::make_unique<DataElement>(TypeCode::UInt32);
      child->set_name("Second"s);
      test_element->append(std::move(child));
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_TRUE(*test_element == *dsg.effective_element);
   }

   TEST(DataSignature, EncodeDecodeRecordInsideRecordArray)
   {
      std::string const expected{ "{\"First\"L\"Second\"{\"Inner1\"S\"Inner2\"C(0,3)}}[10]" };
      auto test_element = std::make_unique<DataElement>(TypeCode::Record);
      test_element->set_array_length(10u);
      auto child = std::make_unique<DataElement>(TypeCode::UInt32);
      child->set_name("First"s);
      test_element->append(std::move(child));
      child = std::make_unique<DataElement>(TypeCode::Record);
      child->set_name("Second"s);
      auto grand_child = std::make_unique<DataElement>(TypeCode::UInt16);
      grand_child->set_name("Inner1"s);
      child->append(std::move(grand_child));
      grand_child = std::make_unique<DataElement>(TypeCode::UInt8, static_cast<uint32_t>(0), static_cast<uint32_t>(3));
      grand_child->set_name("Inner2"s);
      child->append(std::move(grand_child));
      test_element->append(std::move(child));
      EXPECT_EQ(test_element->to_string(), expected);
      DataSignature dsg;
      EXPECT_EQ(dsg.parse_effective(expected), APX_NO_ERROR);
      EXPECT_TRUE(dsg.effective_element.get());
      EXPECT_TRUE(*test_element == *dsg.effective_element);
   }
}