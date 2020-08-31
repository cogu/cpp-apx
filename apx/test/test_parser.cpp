#include "pch.h"
#include "cpp-apx/parser.h"
#include <sstream>
#include <string>

using namespace std::string_literals;

TEST(ApxParser, ParseEmptyString) {
   const char* apx_text = "";
   apx::Parser parser;
   std::stringstream ss;
   ss.str(apx_text);
   EXPECT_FALSE(parser.parse(ss));
}

TEST(ApxParser, ParseEmptyNode) {
   const char* apx_text =
      "APX/1.3\n"
      "N\"EmptyNode\"\n";
   apx::Parser parser;
   std::stringstream ss;
   ss.str(apx_text);
   EXPECT_TRUE(parser.parse(ss));
   std::unique_ptr<apx::Node> node{ parser.take_last_node() };
   EXPECT_EQ(node->get_name(), "EmptyNode"s);
}

TEST(ApxParser, ParseTypeDeclaration_U8Type)
{
   const char* apx_text =
      "APX/1.3\n"
      "N\"TestNode\"\n"
      "T\"Percentage_T\"C";
   apx::Parser parser;
   std::stringstream ss;
   ss.str(apx_text);
   EXPECT_TRUE(parser.parse(ss));
   std::unique_ptr<apx::Node> node{ parser.take_last_node() };
   auto data_type = node->get_last_data_type();
   EXPECT_NE(data_type, nullptr);
   EXPECT_EQ(data_type->name, "Percentage_T"s);
   auto data_element = data_type->dsg.element.get();
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt8);
   EXPECT_FALSE(data_element->has_limits());
}

TEST(ApxParser, ParseTypeDeclaration_U8Type_WithAttribute)
{
   const char* apx_text =
      "APX/1.3\n"
      "N\"TestNode\"\n"
      "T\"OnOff_T\"C(0,3):VT(\"OnOff_Off\", \"OnOff_On\", \"OnOff_Error\", \"OnOff_NotAvailable\")";
   apx::Parser parser;
   std::stringstream ss;
   ss.str(apx_text);
   EXPECT_TRUE(parser.parse(ss));
   std::unique_ptr<apx::Node> node{ parser.take_last_node() };
   auto data_type = node->get_last_data_type();
   EXPECT_NE(data_type, nullptr);
   EXPECT_EQ(data_type->name, "OnOff_T"s);
   auto data_element = data_type->dsg.element.get();
   EXPECT_NE(data_element, nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt8);
   auto [lower_limit, upper_limit] = data_element->get_limits_unsigned();
   EXPECT_EQ(lower_limit, 0u);
   EXPECT_EQ(upper_limit, 3u);
   auto attr = data_type->get_attributes();
   EXPECT_NE(attr, nullptr);
   EXPECT_EQ(attr->computations.size(), 1);
   auto vt = dynamic_cast<apx::ValueTable*>(attr->computations.at(0).get());
   EXPECT_NE(vt, nullptr);
   EXPECT_EQ(vt->values[0], "OnOff_Off");
   EXPECT_EQ(vt->values[1], "OnOff_On");
   EXPECT_EQ(vt->values[2], "OnOff_Error");
   EXPECT_EQ(vt->values[3], "OnOff_NotAvailable");
}

TEST(ApxParser, ParseRequirePortDeclaration_U8_NoInit)
{
   const char* apx_text =
      "APX/1.3\n"
      "N\"TestNode\"\n"
      "R\"FuelLevel\"C";
   apx::Parser parser;
   std::stringstream ss;
   ss.str(apx_text);
   EXPECT_TRUE(parser.parse(ss));
   std::unique_ptr<apx::Node> node{ parser.take_last_node() };
   auto port = node->get_last_require_port();
   ASSERT_NE(port, nullptr);
   ASSERT_EQ(port->name, "FuelLevel"s);
   auto data_element = port->get_data_element();
   EXPECT_NE(data_element, nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt8);
}

TEST(ApxParser, ParseRequirePortDeclaration_U8Array)
{
   const char* apx_text =
      "APX/1.3\n"
      "N\"TestNode\"\n"
      "R\"DataPoints\"C[8]";
   apx::Parser parser;
   std::stringstream ss;
   ss.str(apx_text);
   EXPECT_TRUE(parser.parse(ss));
   std::unique_ptr<apx::Node> node{ parser.take_last_node() };
   auto port = node->get_last_require_port();
   ASSERT_NE(port, nullptr);
   ASSERT_EQ(port->name, "DataPoints"s);
   auto data_element = port->get_data_element();
   EXPECT_NE(data_element, nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt8);
   EXPECT_TRUE(data_element->is_array());
   EXPECT_EQ(data_element->get_array_length(), 8u);
}

TEST(ApxParser, ParseRequirePortDeclaration_U8_Init)
{
   const char* apx_text =
      "APX/1.3\n"
      "N\"TestNode\"\n"
      "R\"FuelLevel\"C:=255";
   apx::Parser parser;
   std::stringstream ss;
   ss.str(apx_text);
   EXPECT_TRUE(parser.parse(ss));
   std::unique_ptr<apx::Node> node{ parser.take_last_node() };
   auto port = node->get_last_require_port();
   ASSERT_NE(port, nullptr);
   ASSERT_EQ(port->name, "FuelLevel"s);
   auto data_element = port->get_data_element();
   EXPECT_NE(data_element, nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt8);
   EXPECT_FALSE(data_element->has_limits());
   auto attr = port->get_attributes();
   EXPECT_NE(attr, nullptr);
   auto dv = attr->get_init_value();
   auto sv = dynamic_cast<dtl::Scalar*>(dv);
   ASSERT_NE(sv, nullptr);
   bool ok = false;
   ASSERT_EQ(sv->to_i32(ok), 255);
   ASSERT_TRUE(ok);
}

TEST(ApxParser, ParseRequirePortDeclaration_U8_WithRange)
{
   const char* apx_text =
      "APX/1.3\n"
      "N\"TestNode\"\n"
      "R\"U8Value\"C(0,3):=3";
   apx::Parser parser;
   std::stringstream ss;
   ss.str(apx_text);
   EXPECT_TRUE(parser.parse(ss));
   std::unique_ptr<apx::Node> node{ parser.take_last_node() };
   auto port = node->get_last_require_port();
   ASSERT_NE(port, nullptr);
   ASSERT_EQ(port->name, "U8Value"s);
   auto data_element = port->get_data_element();
   EXPECT_NE(data_element, nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt8);
   auto [lower, upper] = data_element->get_limits_unsigned();
   EXPECT_EQ(lower, 0u);
   EXPECT_EQ(upper, 3u);
   auto attr = port->get_attributes();
   EXPECT_NE(attr, nullptr);
   auto dv = attr->get_init_value();
   auto sv = dynamic_cast<dtl::Scalar*>(dv);
   ASSERT_NE(sv, nullptr);
   bool ok = false;
   ASSERT_EQ(sv->to_i32(ok), 3);
   ASSERT_TRUE(ok);
}


TEST(ApxParser, ParseRequirePortDeclaration_U8Ref_NoInit) {
   const char* apx_text =
      "APX/1.3\n"
      "N\"TestNode\"\n"
      "T\"Percentage_T\"C\n"
      "R\"FuelLevel1\"T[0]\n"
      "R\"FuelLevel2\"T[0]\n";
   apx::Parser parser;
   std::stringstream ss;
   ss.str(apx_text);
   EXPECT_TRUE(parser.parse(ss));
   std::unique_ptr<apx::Node> node{ parser.take_last_node() };
   ASSERT_EQ(node->get_name(), "TestNode"s);
   ASSERT_EQ(node->get_num_require_ports(), 2u);
   auto port = node->get_require_port(0u);
   ASSERT_NE(port, nullptr);
   ASSERT_EQ(port->name, "FuelLevel1"s);
   auto data_element = port->get_data_element();
   EXPECT_NE(data_element, nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::TypeRefId);
   EXPECT_EQ(data_element->get_typeref_id(), (uint32_t)0u);
   port = node->get_require_port(1u);
   ASSERT_NE(port, nullptr);
   ASSERT_EQ(port->name, "FuelLevel2"s);
   data_element = port->get_data_element();
   EXPECT_NE(data_element, nullptr);
   EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::TypeRefId);
   EXPECT_EQ(data_element->get_typeref_id(), (uint32_t)0u);
}

TEST(ApxParser, Record_Notification_T)
{
   const char* apx_text =
      "APX/1.3\n"
      "N\"TestNode\"\n"
      "T\"Notification_T\"{\"ID\"C(0,127)\"Stat\"C(0,3)\"Type\"C(0,7)}\n";
   apx::Parser parser;
   std::stringstream ss;
   ss.str(apx_text);
   EXPECT_TRUE(parser.parse(ss));
   std::unique_ptr<apx::Node> node{ parser.take_last_node() };
   auto port = node->get_data_type(0u);
   ASSERT_NE(port, nullptr);
   ASSERT_EQ(port->name, "Notification_T"s);
   auto data_element = port->get_data_element();
   ASSERT_NE(data_element, nullptr);
   ASSERT_EQ(data_element->get_num_child_elements(), 3);
   auto child_element = data_element->get_child_at(0u);
   ASSERT_NE(child_element, nullptr);
   ASSERT_EQ(child_element->get_name(), "ID"s);
   child_element = data_element->get_child_at(1u);
   ASSERT_NE(child_element, nullptr);
   ASSERT_EQ(child_element->get_name(), "Stat"s);
   child_element = data_element->get_child_at(2);
   ASSERT_NE(child_element, nullptr);
   ASSERT_EQ(child_element->get_name(), "Type"s);
}

TEST(ApxParser, ParseErrorInRecord)
{
   const char* apx_text =
      "APX/1.3\n"
      "N\"TestNode\"\n"
      "T\"Notification_T\"{{\"ID\"C(0,127)\"Stat\"C(0,3)\"Type\"C(0,7)}\n";
   apx::Parser parser;
   std::stringstream ss;
   ss.str(apx_text);
   EXPECT_FALSE(parser.parse(ss));
   auto rc = parser.get_last_error();
   const auto& str = parser.get_parse_error_str();
   auto error_line = parser.get_line_number();
   EXPECT_EQ(rc, APX_PARSE_ERROR);
   EXPECT_EQ(str, "{"s);
   EXPECT_EQ(error_line, 3);
}

