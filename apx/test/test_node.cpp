#include "pch.h"
#include "cpp-apx/node.h"
#include <string>

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
      auto data_type = new apx::DataType("VehicleSpeed_T"s);
      EXPECT_EQ(node.append(data_type), APX_NO_ERROR);
      EXPECT_EQ(node.get_num_data_types(), 1u);
      EXPECT_EQ(data_type->type_id, 0u);
      data_type = new apx::DataType("EngineSpeed_T"s);
      EXPECT_EQ(node.append(data_type), APX_NO_ERROR);
      EXPECT_EQ(node.get_num_data_types(), 2u);
      EXPECT_EQ(data_type->type_id, 1u);
      data_type = new apx::DataType("VehicleSpeed_T"s);
      EXPECT_EQ(node.append(data_type), APX_TYPE_ALREADY_EXIST_ERROR);
      delete data_type;
   }

   TEST(Node, AppendRequirePort)
   {
      apx::Node node{ "Test" };
      auto port = new apx::Port(apx::PortType::RequirePort, "VehicleSpeed"s);
      EXPECT_EQ(node.append(port), APX_NO_ERROR);
      EXPECT_EQ(node.get_num_require_ports(), 1u);
      EXPECT_EQ(port->port_id, 0u);
      port = new apx::Port(apx::PortType::RequirePort, "EngineSpeed"s);
      EXPECT_EQ(node.append(port), APX_NO_ERROR);
      EXPECT_EQ(node.get_num_require_ports(), 2u);
      EXPECT_EQ(port->port_id, 1u);
   }

   TEST(Node, AppendProvidePort)
   {
      apx::Node node{ "Test" };
      auto port = new apx::Port(apx::PortType::ProvidePort, "VehicleSpeed"s);
      EXPECT_EQ(node.append(port), APX_NO_ERROR);
      EXPECT_EQ(node.get_num_provide_ports(), 1u);
      EXPECT_EQ(port->port_id, 0u);
      port = new apx::Port(apx::PortType::ProvidePort, "EngineSpeed"s);
      EXPECT_EQ(node.append(port), APX_NO_ERROR);
      EXPECT_EQ(node.get_num_provide_ports(), 2u);
      EXPECT_EQ(port->port_id, 1u);
   }

   TEST(Node, SameRequireAndProvidePort)
   {
      apx::Node node{ "Test" };
      auto port = new apx::Port(apx::PortType::RequirePort, "VehicleSpeed"s);
      EXPECT_EQ(node.append(port), APX_NO_ERROR);
      EXPECT_EQ(node.get_num_require_ports(), 1u);
      EXPECT_EQ(port->port_id, 0u);
      port = new apx::Port(apx::PortType::ProvidePort, "VehicleSpeed"s);
      EXPECT_EQ(node.append(port), APX_PORT_ALREADY_EXIST_ERROR);
      delete port;
   }

   TEST(Node, GetTypeById)
   {
      apx::Node node{ "Test" };
      auto data_type = new apx::DataType("VehicleSpeed_T"s);
      EXPECT_EQ(node.append(data_type), APX_NO_ERROR);
      data_type = new apx::DataType("EngineSpeed_T"s);
      EXPECT_EQ(node.append(data_type), APX_NO_ERROR);
      data_type = node.get_data_type(1u);
      EXPECT_NE(data_type, nullptr);
      EXPECT_EQ(data_type->name, "EngineSpeed_T");
   }

   TEST(Node, GetRequirePortById)
   {
      apx::Node node{ "Test" };
      auto port = new apx::Port(apx::PortType::RequirePort, "VehicleSpeed"s);
      EXPECT_EQ(node.append(port), APX_NO_ERROR);
      port = new apx::Port(apx::PortType::RequirePort, "EngineSpeed"s);
      EXPECT_EQ(node.append(port), APX_NO_ERROR);
      port = node.get_require_port(0u);
      EXPECT_NE(port, nullptr);
      EXPECT_EQ(port->name, "VehicleSpeed");
   }
}
