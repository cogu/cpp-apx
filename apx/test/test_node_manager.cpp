#include "pch.h"
#include "cpp-apx/node_manager.h"
#include <iostream>

using namespace apx::vm;

namespace apx_test
{
   TEST(NodeManager, buildTestNode1)
   {
      const char* apx_text =
         "APX/1.2\n"
         "N\"TestNode1\"\n"
         "P\"WheelBasedVehicleSpeed\"S:=65535\n"
         "P\"CabTiltLockWarning\"C:=7\n"
         "P\"VehicleMode\"C:=15\n"
         "R\"GearSelectionMode\"C:=7\n";
      apx::NodeManager manager;
      EXPECT_EQ(manager.size(), 0u);
      EXPECT_EQ(manager.build_node(apx_text), APX_NO_ERROR);
      EXPECT_EQ(manager.size(), 1u);
      auto *node = manager.get_last_attached();
      EXPECT_NE(node, nullptr);
      EXPECT_EQ(node->get_name(), "TestNode1");
      EXPECT_EQ(node->get_num_provide_ports(), 3u);
      EXPECT_EQ(node->get_num_require_ports(), 1u);
      EXPECT_EQ(node->get_provide_port_init_data_size(), UINT16_SIZE + UINT8_SIZE + UINT8_SIZE);
      EXPECT_EQ(node->get_require_port_init_data_size(), UINT8_SIZE);
      auto provide_port_data = node->get_provide_port_init_data();
      auto require_port_data = node->get_require_port_init_data();
      EXPECT_NE(provide_port_data, nullptr);
      EXPECT_NE(require_port_data, nullptr);
      EXPECT_EQ(provide_port_data[0], 0xffu);
      EXPECT_EQ(provide_port_data[1], 0xffu);
      EXPECT_EQ(provide_port_data[2], 7u);
      EXPECT_EQ(provide_port_data[3], 15u);
      EXPECT_EQ(require_port_data[0], 7u);
   }

}
