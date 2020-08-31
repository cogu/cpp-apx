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

   TEST(Node, FollowSimpleTypeRefById)
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
   
   
   TEST(Node, FollowRecursiveTypeRefById)
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
   
}
