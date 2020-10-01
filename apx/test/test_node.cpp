#include "pch.h"
#include "cpp-apx/node.h"
#include "cpp-apx/parser.h"
#include <string>
#include <sstream>

using namespace std::string_literals;


namespace apx_test
{
   TEST(Node, CreateEmptyNode)
   {
      apx::Node node{ "Test" };
      EXPECT_EQ(node.get_name(), "Test"s);
      EXPECT_EQ(node.get_num_data_types(), 0u);
      EXPECT_EQ(node.get_num_require_ports(), 0u);
      EXPECT_EQ(node.get_num_provide_ports(), 0u);
   }

   TEST(Node, AppendDataType)
   {
      apx::Node node{ "Test" };
      auto data_type = new apx::DataType("VehicleSpeed_T"s, 2);
      EXPECT_EQ(node.append(data_type), APX_NO_ERROR);
      EXPECT_EQ(node.get_num_data_types(), 1u);
      EXPECT_EQ(data_type->type_id, 0u);
      data_type = new apx::DataType("EngineSpeed_T"s, 3);
      EXPECT_EQ(node.append(data_type), APX_NO_ERROR);
      EXPECT_EQ(node.get_num_data_types(), 2u);
      EXPECT_EQ(data_type->type_id, 1u);
      data_type = new apx::DataType("VehicleSpeed_T"s, 4);
      EXPECT_EQ(node.append(data_type), APX_TYPE_ALREADY_EXIST_ERROR);
      delete data_type;
   }

   TEST(Node, AppendRequirePort)
   {
      apx::Node node{ "Test" };
      auto port = new apx::Port(apx::PortType::RequirePort, "VehicleSpeed"s, 2);
      EXPECT_EQ(node.append(port), APX_NO_ERROR);
      EXPECT_EQ(node.get_num_require_ports(), 1u);
      EXPECT_EQ(port->port_id, 0u);
      port = new apx::Port(apx::PortType::RequirePort, "EngineSpeed"s, 3);
      EXPECT_EQ(node.append(port), APX_NO_ERROR);
      EXPECT_EQ(node.get_num_require_ports(), 2u);
      EXPECT_EQ(port->port_id, 1u);
   }

   TEST(Node, AppendProvidePort)
   {
      apx::Node node{ "Test" };
      auto port = new apx::Port(apx::PortType::ProvidePort, "VehicleSpeed"s, 2);
      EXPECT_EQ(node.append(port), APX_NO_ERROR);
      EXPECT_EQ(node.get_num_provide_ports(), 1u);
      EXPECT_EQ(port->port_id, 0u);
      port = new apx::Port(apx::PortType::ProvidePort, "EngineSpeed"s, 3);
      EXPECT_EQ(node.append(port), APX_NO_ERROR);
      EXPECT_EQ(node.get_num_provide_ports(), 2u);
      EXPECT_EQ(port->port_id, 1u);
   }

   TEST(Node, SameRequireAndProvidePort)
   {
      apx::Node node{ "Test" };
      auto port = new apx::Port(apx::PortType::RequirePort, "VehicleSpeed"s, 2);
      EXPECT_EQ(node.append(port), APX_NO_ERROR);
      EXPECT_EQ(node.get_num_require_ports(), 1u);
      EXPECT_EQ(port->port_id, 0u);
      port = new apx::Port(apx::PortType::ProvidePort, "VehicleSpeed"s, 3);
      EXPECT_EQ(node.append(port), APX_PORT_ALREADY_EXIST_ERROR);
      delete port;
   }

   TEST(Node, GetTypeById)
   {
      apx::Node node{ "Test" };
      auto data_type = new apx::DataType("VehicleSpeed_T"s, 2);
      EXPECT_EQ(node.append(data_type), APX_NO_ERROR);
      data_type = new apx::DataType("EngineSpeed_T"s, 3);
      EXPECT_EQ(node.append(data_type), APX_NO_ERROR);
      data_type = node.get_data_type(1u);
      EXPECT_NE(data_type, nullptr);
      EXPECT_EQ(data_type->name, "EngineSpeed_T");
   }

   TEST(Node, GetRequirePortById)
   {
      apx::Node node{ "Test" };
      auto port = new apx::Port(apx::PortType::RequirePort, "VehicleSpeed"s, 2);
      EXPECT_EQ(node.append(port), APX_NO_ERROR);
      port = new apx::Port(apx::PortType::RequirePort, "EngineSpeed"s, 3);
      EXPECT_EQ(node.append(port), APX_NO_ERROR);
      port = node.get_require_port(0u);
      EXPECT_NE(port, nullptr);
      EXPECT_EQ(port->name, "VehicleSpeed");
   }

   TEST(Node, FollowTypeRefById)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "T\"Percentage_T\"C\n"
         "R\"FuelLevel\"T[0]\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      EXPECT_EQ(node->finalize(), APX_NO_ERROR);

      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      ASSERT_EQ(port->line_number, 4);
      auto data_element = port->get_data_element();
      ASSERT_NE(data_element, nullptr);
      ASSERT_EQ(data_element->get_type_code(), apx::TypeCode::TypeRefPtr);
   }


   TEST(Node, RecursiveFollowTypeRefById)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "T\"UserName_T\"a[20]\n"
         "T\"UserId_T\"L\n"
         "R\"UserInfo\"{\"UserName\"T[0]\"UserId\"T[1]}\n";

      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      EXPECT_EQ(node->finalize(), APX_NO_ERROR);

      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      ASSERT_EQ(port->line_number, 5);
      auto data_element = port->get_data_element();
      ASSERT_NE(data_element, nullptr);
      ASSERT_EQ(data_element->get_type_code(), apx::TypeCode::Record);
      ASSERT_EQ(data_element->get_num_child_elements(), 2u);
      auto child_element = data_element->get_child_at(0);
      ASSERT_NE(child_element, nullptr);
      ASSERT_EQ(child_element->get_name(), "UserName");
      ASSERT_EQ(child_element->get_type_code(), apx::TypeCode::TypeRefPtr);
      child_element = data_element->get_child_at(1);
      ASSERT_NE(child_element, nullptr);
      ASSERT_EQ(child_element->get_name(), "UserId");
      ASSERT_EQ(child_element->get_type_code(), apx::TypeCode::TypeRefPtr);
   }

   TEST(Node, FollowTypeRefByName)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "T\"Percentage_T\"C\n"
         "R\"FuelLevel\"T[\"Percentage_T\"]\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      EXPECT_EQ(node->finalize(), APX_NO_ERROR);

      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      ASSERT_EQ(port->line_number, 4);
      auto data_element = port->get_data_element();
      ASSERT_NE(data_element, nullptr);
      ASSERT_EQ(data_element->get_type_code(), apx::TypeCode::TypeRefPtr);
   }

   TEST(Node, RecursiveFollowTypeRefByName)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "T\"UserName_T\"a[20]\n"
         "T\"UserId_T\"L\n"
         "R\"UserInfo\"{\"UserName\"T[\"UserName_T\"]\"UserId\"T[\"UserId_T\"]}\n";

      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      EXPECT_EQ(node->finalize(), APX_NO_ERROR);

      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      ASSERT_EQ(port->line_number, 5);
      auto data_element = port->get_data_element();
      ASSERT_NE(data_element, nullptr);
      ASSERT_EQ(data_element->get_type_code(), apx::TypeCode::Record);
      ASSERT_EQ(data_element->get_num_child_elements(), 2u);
      auto child_element = data_element->get_child_at(0);
      ASSERT_NE(child_element, nullptr);
      ASSERT_EQ(child_element->get_name(), "UserName");
      ASSERT_EQ(child_element->get_type_code(), apx::TypeCode::TypeRefPtr);
      child_element = data_element->get_child_at(1);
      ASSERT_NE(child_element, nullptr);
      ASSERT_EQ(child_element->get_name(), "UserId");
      ASSERT_EQ(child_element->get_type_code(), apx::TypeCode::TypeRefPtr);
   }

   TEST(Node, DeriveProperInitValueU8)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Signal\"C:=255\n";

      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      EXPECT_EQ(node->finalize(), APX_NO_ERROR);

      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      ASSERT_NE(port->proper_init_value, nullptr);
      auto init_value = port->proper_init_value;
      ASSERT_EQ(init_value->dv_type(), dtl::ValueType::Scalar);
      auto sv = dynamic_cast<dtl::Scalar*>(init_value.get());
      bool ok;
      ASSERT_EQ(sv->to_u32(ok), 255u);
      ASSERT_TRUE(ok);
   }

   TEST(Node, QueuedProvidePort)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "P\"U8QueuedSignal\"C(0,15):Q[10]\n";

      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };

      auto port = node->get_provide_port(0u);
      ASSERT_NE(port, nullptr);
      ASSERT_TRUE(port->is_queued());
      ASSERT_EQ(port->get_queue_length(), 10u);
   }

   TEST(Node, DeriveProperInitValueRecord_UInt8_Uint16)
   {
      bool ok;
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"RecordPort\"{\"First\"C\"Second\"S}:={255, 65535}\n";

      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      ASSERT_NE(port->proper_init_value, nullptr);
      auto init_value = port->proper_init_value;
      ASSERT_EQ(init_value->dv_type(), dtl::ValueType::Hash);
      auto derived_hv = dynamic_cast<dtl::Hash*>(init_value.get());
      auto const* first = derived_hv->get("First");
      ASSERT_NE(first, nullptr);
      EXPECT_EQ(first->dv_type(), dtl::ValueType::Scalar);
      auto const* sv = dynamic_cast<const dtl::Scalar*>(first);
      EXPECT_EQ(sv->to_u32(ok), 255u);
      EXPECT_TRUE(ok);
      auto const* second = derived_hv->get("Second");
      ASSERT_NE(second, nullptr);
      EXPECT_EQ(second->dv_type(), dtl::ValueType::Scalar);
      sv = dynamic_cast<const dtl::Scalar*>(second);
      EXPECT_EQ(sv->to_u32(ok), 65535u);
      EXPECT_TRUE(ok);
   }

   TEST(Node, DeriveProperInitValueRecord_U8U8_U16)
   {
      bool ok;
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"RecordPort\"{\"Outer1\"{\"Inner1\"C(0,7)\"Inner2\"C(0,3)}\"Outer2\"L}:={ {7,3}, 65535}\n";

      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      auto init_value = port->proper_init_value;
      ASSERT_NE(init_value.get(), nullptr);
      ASSERT_EQ(init_value->dv_type(), dtl::ValueType::Hash);
      auto derived_hv = dynamic_cast<dtl::Hash*>(init_value.get());
      auto const* inner_dv = derived_hv->get("Outer1");
      ASSERT_EQ(inner_dv->dv_type(), dtl::ValueType::Hash);
      auto inner_hv = dynamic_cast<const dtl::Hash*>(inner_dv);
      auto const* inner1 = inner_hv->get("Inner1");
      ASSERT_NE(inner1, nullptr);
      EXPECT_EQ(inner1->dv_type(), dtl::ValueType::Scalar);
      auto const* sv = dynamic_cast<const dtl::Scalar*>(inner1);
      ASSERT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_u32(ok), 7u);
      EXPECT_TRUE(ok);
      auto const* inner2 = inner_hv->get("Inner2");
      ASSERT_NE(inner2, nullptr);
      EXPECT_EQ(inner2->dv_type(), dtl::ValueType::Scalar);
      sv = dynamic_cast<const dtl::Scalar*>(inner2);
      ASSERT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_u32(ok), 3u);
      EXPECT_TRUE(ok);
      auto const* outer2_dv = derived_hv->get("Outer2");
      ASSERT_EQ(outer2_dv->dv_type(), dtl::ValueType::Scalar);
      sv = dynamic_cast<const dtl::Scalar*>(outer2_dv);
      ASSERT_NE(sv, nullptr);
      EXPECT_EQ(sv->to_u32(ok), 65535u);
      EXPECT_TRUE(ok);
   }

   TEST(Node, DeriveProperInitValue_DynamicArray)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"DynamicPort\"b[4095*]:={}\n";

      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      auto init_value = port->proper_init_value;
      ASSERT_NE(init_value.get(), nullptr);
      ASSERT_EQ(init_value->dv_type(), dtl::ValueType::Array);
      auto av = dynamic_cast<dtl::Array*>(init_value.get());
      ASSERT_TRUE(av->is_empty());
   }
}
