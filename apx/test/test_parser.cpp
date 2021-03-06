#include "pch.h"
#include "cpp-apx/parser.h"
#include <sstream>
#include <string>

using namespace std::string_literals;

namespace apx_test
{
   TEST(Parser, ParseEmptyString)
   {
      const char* apx_text = "";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_FALSE(parser.parse(ss));
   }

   TEST(Parser, ParseEmptyNode)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"EmptyNode\"\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      EXPECT_EQ(node->get_name(), "EmptyNode"s);
   }

   TEST(Parser, ParseTypeDeclaration_U8Type)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "T\"Percentage_T\"C";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto data_type = node->get_last_data_type();
      EXPECT_NE(data_type, nullptr);
      EXPECT_EQ(data_type->name, "Percentage_T"s);
      auto data_element = data_type->dsg.element.get();
      EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt8);
      EXPECT_FALSE(data_element->has_limits());
   }

   TEST(Parser, ParseTypeDeclaration_U8Type_WithAttribute)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "T\"OnOff_T\"C(0,3):VT(\"OnOff_Off\", \"OnOff_On\", \"OnOff_Error\", \"OnOff_NotAvailable\")";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto data_type = node->get_last_data_type();
      ASSERT_TRUE(data_type);
      EXPECT_EQ(data_type->name, "OnOff_T"s);
      auto data_element = data_type->dsg.element.get();
      ASSERT_TRUE(data_element);
      EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt8);
      auto [lower_limit, upper_limit] = data_element->get_limits_u32();
      EXPECT_EQ(lower_limit, 0u);
      EXPECT_EQ(upper_limit, 3u);
      auto attr = data_type->get_attributes();
      ASSERT_TRUE(attr);
      EXPECT_EQ(attr->computations.size(), 1);
      auto vt = dynamic_cast<apx::ValueTable*>(attr->computations.at(0).get());
      ASSERT_TRUE(vt);
      EXPECT_EQ(vt->values[0], "OnOff_Off");
      EXPECT_EQ(vt->values[1], "OnOff_On");
      EXPECT_EQ(vt->values[2], "OnOff_Error");
      EXPECT_EQ(vt->values[3], "OnOff_NotAvailable");
   }

   TEST(Parser, ParseRequirePortDeclaration_U8_NoInit)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"FuelLevel\"C";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_last_require_port();
      ASSERT_NE(port, nullptr);
      ASSERT_EQ(port->name, "FuelLevel"s);
      auto data_element = port->get_data_element();
      EXPECT_NE(data_element, nullptr);
      EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt8);
   }

   TEST(Parser, ParseRequirePortDeclaration_U8Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"DataPoints\"C[8]";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_last_require_port();
      ASSERT_NE(port, nullptr);
      ASSERT_EQ(port->name, "DataPoints"s);
      auto data_element = port->get_data_element();
      EXPECT_NE(data_element, nullptr);
      EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt8);
      EXPECT_TRUE(data_element->is_array());
      EXPECT_EQ(data_element->get_array_length(), 8u);
   }

   TEST(Parser, ParseRequirePortDeclaration_U8_Init)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"FuelLevel\"C:=255";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
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

   TEST(Parser, ParseRequirePortDeclaration_U8_WithRange)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Value\"C(0,3):=3";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_last_require_port();
      ASSERT_NE(port, nullptr);
      ASSERT_EQ(port->name, "U8Value"s);
      auto data_element = port->get_data_element();
      EXPECT_NE(data_element, nullptr);
      EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::UInt8);
      auto [lower, upper] = data_element->get_limits_u32();
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


   TEST(Parser, ParseRequirePortDeclaration_U8Ref_NoInit) {
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
      auto node{ parser.take_last_node() };
      ASSERT_EQ(node->get_name(), "TestNode"s);
      ASSERT_EQ(node->get_num_require_ports(), 2u);
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      ASSERT_EQ(port->name, "FuelLevel1"s);
      auto data_element = port->get_data_element();
      EXPECT_NE(data_element, nullptr);
      EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::TypeRefPtr);
      EXPECT_NE(data_element->get_typeref_ptr(), nullptr);
      port = node->get_require_port(1u);
      ASSERT_NE(port, nullptr);
      ASSERT_EQ(port->name, "FuelLevel2"s);
      data_element = port->get_data_element();
      EXPECT_NE(data_element, nullptr);
      EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::TypeRefPtr);
      EXPECT_NE(data_element->get_typeref_ptr(), nullptr);
   }

   TEST(Parser, ParseRequirePortDeclaration_U8Ref)
   {

      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "T\"Type_T\"C(0,3):VT(\"Off\",\"On\",\"Error\",\"NotAvailable\")\n"
         "R\"UInt8Port\"T[0]:=3\n";
      apx::Parser parser;
      EXPECT_EQ(parser.parse(apx_text), APX_NO_ERROR);
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      ASSERT_EQ(port->name, "UInt8Port"s);
      auto data_element = port->get_data_element();
      EXPECT_NE(data_element, nullptr);
      EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::TypeRefPtr);
      EXPECT_NE(data_element->get_typeref_ptr(), nullptr);
   }

   TEST(Parser, RecordTypeRefsContainingElementTypeRefsById)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "T\"FirstType_T\"C(0,3)\n"
         "T\"SecondType_T\"C(0,7)\n"
         "T\"RecordType_T\"{\"First\"T[0]\"Second\"T[1]}\n"
         "R\"RecordPort\"T[2]:={3,7}\n";
      apx::Parser parser;
      EXPECT_EQ(parser.parse(apx_text), APX_NO_ERROR);
      auto node{ parser.take_last_node() };
      ASSERT_EQ(node->get_num_require_ports(), 1u);
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      auto data_element = port->get_data_element();
      EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::TypeRefPtr);
      auto data_type = data_element->get_typeref_ptr();
      EXPECT_NE(data_type, nullptr);
      auto record_element = data_type->get_data_element();
      EXPECT_EQ(record_element->get_type_code(), apx::TypeCode::Record);
      {
         auto child_type_element = record_element->get_child_at(0);
         EXPECT_NE(child_type_element, nullptr);
         EXPECT_EQ(child_type_element->get_name(), "First");
         EXPECT_EQ(child_type_element->get_type_code(), apx::TypeCode::TypeRefPtr);
         auto child_data_type = child_type_element->get_typeref_ptr();
         EXPECT_NE(child_data_type, nullptr);
         auto child_data_element = child_data_type->get_data_element();
         EXPECT_NE(child_data_element, nullptr);
         EXPECT_EQ(child_data_element->get_type_code(), apx::TypeCode::UInt8);
      }
      {
         auto child_type_element = record_element->get_child_at(1);
         EXPECT_NE(child_type_element, nullptr);
         EXPECT_EQ(child_type_element->get_name(), "Second");
         EXPECT_EQ(child_type_element->get_type_code(), apx::TypeCode::TypeRefPtr);
         auto child_data_type = child_type_element->get_typeref_ptr();
         EXPECT_NE(child_data_type, nullptr);
         auto child_data_element = child_data_type->get_data_element();
         EXPECT_NE(child_data_element, nullptr);
         EXPECT_EQ(child_data_element->get_type_code(), apx::TypeCode::UInt8);
      }

   }

   TEST(Parser, Record_Notification_T)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "T\"Notification_T\"{\"ID\"C(0,127)\"Stat\"C(0,3)\"Type\"C(0,7)}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
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

   TEST(Parser, ParseErrorInRecord)
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

   TEST(Parser, ParseQueuedProvidePortWithUint8DataAndUint8QueueSize)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "P\"U8Signal\"C:Q[10]\n";

      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_provide_port(0u);
      ASSERT_NE(port, nullptr);
      auto attr = port->get_attributes();
      EXPECT_NE(attr, nullptr);
      EXPECT_EQ(attr->queue_length, 10u);
   }

   TEST(Parser, ParseChar)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "P\"CharSignal\"a\n";
      apx::Parser parser;
      EXPECT_EQ(parser.parse(apx_text), APX_NO_ERROR);
      auto node{ parser.take_last_node() };
      auto port = node->get_last_provide_port();
      ASSERT_NE(port, nullptr);
      auto data_element = port->get_data_element();
      EXPECT_NE(data_element, nullptr);
      EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Char);
   }

   TEST(Parser, ParseCharArray)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "P\"CharSignal\"a[10]\n";
      apx::Parser parser;
      EXPECT_EQ(parser.parse(apx_text), APX_NO_ERROR);
      auto node{ parser.take_last_node() };
      auto port = node->get_last_provide_port();
      ASSERT_NE(port, nullptr);
      auto data_element = port->get_data_element();
      EXPECT_NE(data_element, nullptr);
      EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Char);
      EXPECT_TRUE(data_element->is_array());
      EXPECT_EQ(data_element->get_array_length(), 10u);
   }

   TEST(Parser, ParseCharArrayWithEmptyInitializer)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "P\"CharSignal\"a[10]:=\"\"\n";
      apx::Parser parser;
      EXPECT_EQ(parser.parse(apx_text), APX_NO_ERROR);
      auto node{ parser.take_last_node() };
      auto port = node->get_last_provide_port();
      ASSERT_NE(port, nullptr);
      auto data_element = port->get_data_element();
      EXPECT_NE(data_element, nullptr);
      EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Char);
      EXPECT_TRUE(data_element->is_array());
      EXPECT_EQ(data_element->get_array_length(), 10u);
   }


   TEST(Parser, ParseChar8)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "P\"CharSignal\"A\n";
      apx::Parser parser;
      EXPECT_EQ(parser.parse(apx_text), APX_NO_ERROR);
      auto node{ parser.take_last_node() };
      auto port = node->get_last_provide_port();
      ASSERT_NE(port, nullptr);
      auto data_element = port->get_data_element();
      EXPECT_NE(data_element, nullptr);
      EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::Char8);
   }

   TEST(Parser, RecordOfRecords)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "T\"FirstType_T\"{\"Inner1\"C\"Inner2\"S}\n"
         "T\"SecondType_T\"{\"Inner3\"S\"Inner4\"L}\n"
         "T\"RecordOfRecordType_T\"{\"First\"T[0]\"Second\"T[1]}\n"
         "R\"RecordPort\"T[2]:={ {3,0xFFFF}, {0xFFFF, 0xFFFFFFFF} }\n";
      apx::Parser parser;
      EXPECT_EQ(parser.parse(apx_text), APX_NO_ERROR);
      auto node{ parser.take_last_node() };
      ASSERT_EQ(node->get_num_require_ports(), 1u);
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      auto data_element = port->get_data_element();
      EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::TypeRefPtr);
      auto data_type = data_element->get_typeref_ptr();
      EXPECT_NE(data_type, nullptr);
      auto record_element = data_type->get_data_element();
      EXPECT_EQ(record_element->get_type_code(), apx::TypeCode::Record);
      {
         auto child_type_element = record_element->get_child_at(0);
         EXPECT_NE(child_type_element, nullptr);
         EXPECT_EQ(child_type_element->get_name(), "First");
         EXPECT_EQ(child_type_element->get_type_code(), apx::TypeCode::TypeRefPtr);
         auto child_data_type = child_type_element->get_typeref_ptr();
         EXPECT_NE(child_data_type, nullptr);
         auto child_data_element = child_data_type->get_data_element();
         EXPECT_NE(child_data_element, nullptr);
         EXPECT_EQ(child_data_element->get_type_code(), apx::TypeCode::Record);
         auto grand_child_type_element = child_data_element->get_child_at(0);
         EXPECT_EQ(grand_child_type_element->get_name(), "Inner1");
         EXPECT_EQ(grand_child_type_element->get_type_code(), apx::TypeCode::UInt8);
         grand_child_type_element = child_data_element->get_child_at(1);
         EXPECT_EQ(grand_child_type_element->get_name(), "Inner2");
         EXPECT_EQ(grand_child_type_element->get_type_code(), apx::TypeCode::UInt16);
      }
      {
         auto child_type_element = record_element->get_child_at(1);
         EXPECT_NE(child_type_element, nullptr);
         EXPECT_EQ(child_type_element->get_name(), "Second");
         EXPECT_EQ(child_type_element->get_type_code(), apx::TypeCode::TypeRefPtr);
         auto child_data_type = child_type_element->get_typeref_ptr();
         EXPECT_NE(child_data_type, nullptr);
         auto child_data_element = child_data_type->get_data_element();
         EXPECT_NE(child_data_element, nullptr);
         EXPECT_EQ(child_data_element->get_type_code(), apx::TypeCode::Record);
         auto grand_child_type_element = child_data_element->get_child_at(0);
         EXPECT_EQ(grand_child_type_element->get_name(), "Inner3");
         EXPECT_EQ(grand_child_type_element->get_type_code(), apx::TypeCode::UInt16);
         grand_child_type_element = child_data_element->get_child_at(1);
         EXPECT_EQ(grand_child_type_element->get_name(), "Inner4");
         EXPECT_EQ(grand_child_type_element->get_type_code(), apx::TypeCode::UInt32);

      }
   }

   TEST(Parser, ArrayOfRecordsUsingTypeRef)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "T\"RecordType_T\"{\"Id\"S\"Value\"C}\n"
         "R\"RecordPort\"T[0][2]:={ {0xFFFF, 0}, {0xFFFF, 0} }\n";
      apx::Parser parser;
      EXPECT_EQ(parser.parse(apx_text), APX_NO_ERROR);
      auto node{ parser.take_last_node() };
      ASSERT_EQ(node->get_num_require_ports(), 1u);
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      auto data_element = port->get_data_element();
      EXPECT_EQ(data_element->get_type_code(), apx::TypeCode::TypeRefPtr);
      EXPECT_TRUE(data_element->is_array());
      EXPECT_EQ(data_element->get_array_length(), 2u);
      auto data_type = data_element->get_typeref_ptr();
      EXPECT_NE(data_type, nullptr);
      auto record_element = data_type->get_data_element();
      EXPECT_EQ(record_element->get_type_code(), apx::TypeCode::Record);

   }

}
