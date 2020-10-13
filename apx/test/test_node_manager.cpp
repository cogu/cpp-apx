#include "pch.h"
#include "cpp-apx/node_manager.h"
#include "sample_nodes.h"
#include <iostream>



namespace apx_test
{
   TEST(NodeManager, buildTestNode1)
   {
      apx::NodeManager manager;
      EXPECT_EQ(manager.size(), 0u);
      EXPECT_EQ(manager.build_node(apx::g_sample_node1), APX_NO_ERROR);
      EXPECT_EQ(manager.size(), 1u);
      auto *node = manager.get_last_attached();
      EXPECT_NE(node, nullptr);
      EXPECT_EQ(node->get_name(), "TestNode1");
   }
}
