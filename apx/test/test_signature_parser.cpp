#include "pch.h"
#include "cpp-apx/signature_parser.h"
#include <string>
#include <cstring>

using namespace std;
using namespace std::string_literals;

TEST(SignatureParser, U8)
{
   const char* begin = "C";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt8);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_FALSE(data_element->is_array());
}

TEST(SignatureParser, U8WithLimits)
{
   const char* begin = "C(0,3)";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt8);
   EXPECT_TRUE(data_element->has_limits());
   EXPECT_FALSE(data_element->is_array());
   auto [lower_limit, upper_limit] = data_element->get_limits_u32();
   EXPECT_EQ(lower_limit, 0u);
   EXPECT_EQ(upper_limit, 3u);
}

TEST(SignatureParser, U8Array)
{
   const char* begin = "C[8]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt8);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_TRUE(data_element->is_array());
   EXPECT_FALSE(data_element->is_dynamic_array());
   EXPECT_EQ(data_element->get_array_length(), 8u);
}
TEST(SignatureParser, U16)
{
   const char* begin = "S";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt16);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_FALSE(data_element->is_array());
}

TEST(SignatureParser, U16WithLimits)
{
   const char* begin = "S(0,1023)";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt16);
   EXPECT_TRUE(data_element->has_limits());
   EXPECT_FALSE(data_element->is_array());
   auto [lower_limit, upper_limit] = data_element->get_limits_u32();
   EXPECT_EQ(lower_limit, 0u);
   EXPECT_EQ(upper_limit, 1023u);
}

TEST(SignatureParser, U16Array)
{
   const char* begin = "S[8]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt16);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_TRUE(data_element->is_array());
   EXPECT_FALSE(data_element->is_dynamic_array());
   EXPECT_EQ(data_element->get_array_length(), 8u);
}

TEST(SignatureParser, U16ArrayWithLimits)
{
   const char* begin = "S(0, 1023)[ 8 ]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt16);
   EXPECT_TRUE(data_element->has_limits());
   EXPECT_TRUE(data_element->is_array());
   EXPECT_FALSE(data_element->is_dynamic_array());
   auto [lower_limit, upper_limit] = data_element->get_limits_u32();
   EXPECT_EQ(lower_limit, 0u);
   EXPECT_EQ(upper_limit, 1023u);
   EXPECT_EQ(data_element->get_array_length(), 8u);
}

TEST(SignatureParser, U32)
{
   const char* begin = "L";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt32);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_FALSE(data_element->is_array());
}

TEST(SignatureParser, U32WithLimits)
{
   const char* begin = "L(0, 99999)";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt32);
   EXPECT_TRUE(data_element->has_limits());
   EXPECT_FALSE(data_element->is_array());
   auto [lower_limit, upper_limit] = data_element->get_limits_u32();
   EXPECT_EQ(lower_limit, 0u);
   EXPECT_EQ(upper_limit, 99999u);
}

TEST(SignatureParser, U32Array)
{
   const char* begin = "L[8]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt32);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_TRUE(data_element->is_array());
   EXPECT_FALSE(data_element->is_dynamic_array());
   EXPECT_EQ(data_element->get_array_length(), 8u);
}

TEST(SignatureParser, U32ArrayWithLimits)
{
   const char* begin = "L(0, 99999)[ 8 ]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt32);
   EXPECT_TRUE(data_element->has_limits());
   EXPECT_TRUE(data_element->is_array());
   EXPECT_FALSE(data_element->is_dynamic_array());
   auto [lower_limit, upper_limit] = data_element->get_limits_u32();
   EXPECT_EQ(lower_limit, 0u);
   EXPECT_EQ(upper_limit, 99999u);
   EXPECT_EQ(data_element->get_array_length(), 8u);
}

TEST(SignatureParser, U64)
{
   const char* begin = "Q";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt64);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_FALSE(data_element->is_array());
}

TEST(SignatureParser, U64WithLimits)
{
   const char* begin = "Q(0 , 0x100000000)";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt64);
   EXPECT_TRUE(data_element->has_limits());
   EXPECT_FALSE(data_element->is_array());
   auto [lower_limit, upper_limit] = data_element->get_limits_u64();
   EXPECT_EQ(lower_limit, 0u);
   EXPECT_EQ(upper_limit, 0x100000000ULL);
}

TEST(SignatureParser, U64Array)
{
   const char* begin = "Q[8]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt64);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_TRUE(data_element->is_array());
   EXPECT_FALSE(data_element->is_dynamic_array());
   EXPECT_EQ(data_element->get_array_length(), 8u);
}

TEST(SignatureParser, U64ArrayWithLimits)
{
   const char* begin = "Q(0, 0x100000000)[ 8 ]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt64);
   EXPECT_TRUE(data_element->has_limits());
   EXPECT_TRUE(data_element->is_array());
   EXPECT_FALSE(data_element->is_dynamic_array());
   auto [lower_limit, upper_limit] = data_element->get_limits_u64();
   EXPECT_EQ(lower_limit, 0u);
   EXPECT_EQ(upper_limit, 0x100000000ULL);
   EXPECT_EQ(data_element->get_array_length(), 8u);
}

TEST(SignatureParser, ByteObject)
{
   const char* begin = "B";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Byte);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_FALSE(data_element->is_array());
}

TEST(SignatureParser, ByteObjectWithLimits)
{
   const char* begin = "B(0,3)";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Byte);
   EXPECT_TRUE(data_element->has_limits());
   EXPECT_FALSE(data_element->is_array());
   auto [lower_limit, upper_limit] = data_element->get_limits_u32();
   EXPECT_EQ(lower_limit, 0u);
   EXPECT_EQ(upper_limit, 3u);
}

TEST(SignatureParser, ByteArray)
{
   const char* begin = "B[8]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Byte);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_TRUE(data_element->is_array());
   EXPECT_FALSE(data_element->is_dynamic_array());
   EXPECT_EQ(data_element->get_array_length(), 8u);
}

TEST(SignatureParser, ByteArrayWithLimits)
{
   const char* begin = "B(0, 7)[ 8 ]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Byte);
   EXPECT_TRUE(data_element->has_limits());
   EXPECT_TRUE(data_element->is_array());
   EXPECT_FALSE(data_element->is_dynamic_array());
   auto [lower_limit, upper_limit] = data_element->get_limits_u32();
   EXPECT_EQ(lower_limit, 0u);
   EXPECT_EQ(upper_limit, 7u);
   EXPECT_EQ(data_element->get_array_length(), 8u);
}

TEST(SignatureParser, DynamicByteArray)
{
   const char* begin = "B[8*]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Byte);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_TRUE(data_element->is_array());
   EXPECT_TRUE(data_element->is_dynamic_array());
   EXPECT_EQ(data_element->get_array_length(), 8u);
}

TEST(SignatureParser, DynamicByteArrayWithLimit)
{
   const char* begin = "B(0,7)[8*]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Byte);
   EXPECT_TRUE(data_element->has_limits());
   EXPECT_TRUE(data_element->is_array());
   EXPECT_TRUE(data_element->is_dynamic_array());
   auto [lower_limit, upper_limit] = data_element->get_limits_u32();
   EXPECT_EQ(lower_limit, 0u);
   EXPECT_EQ(upper_limit, 7u);
   EXPECT_EQ(data_element->get_array_length(), 8u);
}

TEST(SignatureParser, Char)
{
   const char* begin = "a";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Char);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_FALSE(data_element->is_array());
}

TEST(SignatureParser, Char8)
{
   const char* begin = "A";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Char8);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_FALSE(data_element->is_array());
}

TEST(SignatureParser, Char16)
{
   const char* begin = "u";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Char16);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_FALSE(data_element->is_array());
}

TEST(SignatureParser, Char32)
{
   const char* begin = "U";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Char32);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_FALSE(data_element->is_array());
}

TEST(SignatureParser, CharString)
{
   const char* begin = "a[32]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Char);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_TRUE(data_element->is_array());
   EXPECT_EQ(data_element->get_array_length(), 32u);
}

TEST(SignatureParser, Char8String)
{
   const char* begin = "A[32]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Char8);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_TRUE(data_element->is_array());
   EXPECT_EQ(data_element->get_array_length(), 32u);
}

TEST(SignatureParser, Char16String)
{
   const char* begin = "u[32]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Char16);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_TRUE(data_element->is_array());
   EXPECT_EQ(data_element->get_array_length(), 32u);
}

TEST(SignatureParser, Char32String)
{
   const char* begin = "U[32]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Char32);
   EXPECT_FALSE(data_element->has_limits());
   EXPECT_TRUE(data_element->is_array());
   EXPECT_EQ(data_element->get_array_length(), 32u);
}

TEST(SignatureParser, CharDynamicString)
{
   const char* begin = "a[256*]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Char);
   EXPECT_EQ(data_element->get_array_length(), 256u);
   EXPECT_TRUE(data_element->is_dynamic_array());
}

TEST(SignatureParser, Char8DynamicString)
{
   const char* begin = "A[256*]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Char8);
   EXPECT_EQ(data_element->get_array_length(), 256u);
   EXPECT_TRUE(data_element->is_dynamic_array());
}

TEST(SignatureParser, Char16DynamicString)
{
   const char* begin = "u[256*]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Char16);
   EXPECT_EQ(data_element->get_array_length(), 256u);
   EXPECT_TRUE(data_element->is_dynamic_array());
}

TEST(SignatureParser, Char32DynamicString)
{
   const char* begin = "U[256*]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Char32);
   EXPECT_EQ(data_element->get_array_length(), 256u);
   EXPECT_TRUE(data_element->is_dynamic_array());
}

TEST(SignatureParser, CharStringWithLimits)
{
   const char* begin = "a(0,127)[20]";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, begin + 1); //Parsing stopped on parenthesis (which is an invalid character in this context)
}

TEST(SignatureParser, Record_U8_U8_U8)
{
   const char* begin = "{\"ID\"C(0,127)\"Stat\"C(0,3)\"Type\"C(0,7)}";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Record);
   EXPECT_FALSE(data_element->is_array());
   EXPECT_EQ(data_element->get_num_child_elements(), 3u);
}

TEST(SignatureParser, Record_U8U8_U8U8)
{
   const char* begin = "{\"Notification1\"{\"ID1\"C(0,127)\"Stat1\"C(0,3)}\"Notification2\"{\"ID2\"C(0,127)\"Stat2\"C(0,3)}}";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Record);
   EXPECT_FALSE(data_element->is_array());
   EXPECT_EQ(data_element->get_num_child_elements(), 2);
   auto child_elem = data_element->get_child_at(0);
   EXPECT_NE(child_elem, nullptr);
   EXPECT_EQ(child_elem->get_name(), "Notification1");
   EXPECT_EQ(child_elem->get_type_code(), apx::TypeCode::Record);
   EXPECT_EQ(child_elem->get_num_child_elements(), 2);
   auto grand_child = child_elem->get_child_at(0);
   EXPECT_NE(grand_child, nullptr);
   EXPECT_EQ(grand_child->get_name(), "ID1");
   EXPECT_EQ(grand_child->get_type_code(), apx::TypeCode::UInt8);
   grand_child = child_elem->get_child_at(1);
   EXPECT_NE(grand_child, nullptr);
   EXPECT_EQ(grand_child->get_name(), "Stat1");
   EXPECT_EQ(grand_child->get_type_code(), apx::TypeCode::UInt8);
   child_elem = data_element->get_child_at(1);
   EXPECT_NE(child_elem, nullptr);
   EXPECT_EQ(child_elem->get_name(), "Notification2");
   EXPECT_EQ(child_elem->get_type_code(), apx::TypeCode::Record);
   grand_child = child_elem->get_child_at(0);
   EXPECT_NE(grand_child, nullptr);
   EXPECT_EQ(grand_child->get_name(), "ID2");
   EXPECT_EQ(grand_child->get_type_code(), apx::TypeCode::UInt8);
   grand_child = child_elem->get_child_at(1);
   EXPECT_NE(grand_child, nullptr);
   EXPECT_EQ(grand_child->get_name(), "Stat2");
   EXPECT_EQ(grand_child->get_type_code(), apx::TypeCode::UInt8);
}

TEST(SignatureParser, Record_String_U16)
{
   const char* begin = "{\"Name\"a[32]\"ID\"S}";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, end);
   std::unique_ptr<apx::DataElement> data_element{ parser.take_data_element() };
   EXPECT_NE(data_element.get(), nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Record);
   EXPECT_FALSE(data_element->is_array());
   EXPECT_EQ(data_element->get_num_child_elements(), 2u);
   auto child = data_element->get_child_at(0);
   EXPECT_NE(child, nullptr);
   EXPECT_EQ(child->get_name(), "Name");
   EXPECT_EQ(child->get_type_code(), apx::TypeCode::Char);
   EXPECT_EQ(child->get_array_length(), 32u);
   child = data_element->get_child_at(1);
   EXPECT_NE(child, nullptr);
   EXPECT_EQ(child->get_name(), "ID");
   EXPECT_EQ(child->get_type_code(), apx::TypeCode::UInt16);
   EXPECT_FALSE(child->is_array());
}



TEST(SignatureParser, ParseErrorInRecord)
{
   const char* begin = "{{\"ID\"C(0,127)\"Stat\"C(0,3)\"Type\"C(0,7)}";
   const char* end = begin + strlen(begin);
   apx::SignatureParser parser;
   auto result = parser.parse_data_signature(begin, end);
   EXPECT_EQ(result, nullptr);
   const char* error_next = nullptr;
   auto rc = parser.get_last_error(error_next);
   EXPECT_EQ(rc, APX_PARSE_ERROR);
   EXPECT_EQ(error_next, begin+1);
}