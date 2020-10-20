#include "pch.h"
#include "cpp-apx/node_manager.h"
#include <iostream>

using namespace apx::vm;

namespace apx_test
{
   TEST(NodeManager, buildNodeContainingUnsignedTypesWithoutLimits)
   {
      const char* apx_text =
         "APX/1.2\n"
         "N\"TestNode\"\n"
         "P\"U16Signal\"S:=65535\n"
         "P\"U8Signal1\"C:=7\n"
         "P\"U8Signal2\"C:=15\n"
         "R\"U8Signal3\"C:=7\n"
         "R\"U32Signal\"L:=0\n";
      apx::NodeManager manager;
      EXPECT_EQ(manager.build_node(apx_text), APX_NO_ERROR);
      EXPECT_EQ(manager.size(), 1u);
      auto *node = manager.get_last_attached();
      EXPECT_NE(node, nullptr);
      EXPECT_EQ(node->get_num_provide_ports(), 3u);
      EXPECT_EQ(node->get_num_require_ports(), 2u);
      EXPECT_EQ(node->get_provide_port_init_data_size(), UINT16_SIZE + UINT8_SIZE + UINT8_SIZE);
      EXPECT_EQ(node->get_require_port_init_data_size(), UINT8_SIZE + UINT32_SIZE);
      auto provide_port_data = node->get_provide_port_init_data();
      auto require_port_data = node->get_require_port_init_data();
      EXPECT_NE(provide_port_data, nullptr);
      EXPECT_NE(require_port_data, nullptr);
      EXPECT_EQ(provide_port_data[0], 0xffu);
      EXPECT_EQ(provide_port_data[1], 0xffu);
      EXPECT_EQ(provide_port_data[2], 7u);
      EXPECT_EQ(provide_port_data[3], 15u);
      EXPECT_EQ(require_port_data[0], 7u);
      EXPECT_EQ(require_port_data[1], 0u);
      EXPECT_EQ(require_port_data[2], 0u);
      EXPECT_EQ(require_port_data[3], 0u);
      EXPECT_EQ(require_port_data[4], 0u);
   }

   TEST(NodeManager, buildNodeContainingUnsignedTypesWithLimits)
   {
      const char* apx_text =
         "APX/1.2\n"
         "N\"TestNode\"\n"
         "P\"U16Signal\"S:=65535\n"
         "P\"U8Signal1\"C(0,7):=7\n"
         "P\"U8Signal2\"C(0,15):=15\n"
         "R\"U8Signal3\"C(0,7):=7\n"
         "R\"U32Signal\"L:=0\n";
      apx::NodeManager manager;
      EXPECT_EQ(manager.build_node(apx_text), APX_NO_ERROR);
      EXPECT_EQ(manager.size(), 1u);
      auto* node = manager.get_last_attached();
      EXPECT_NE(node, nullptr);
      EXPECT_EQ(node->get_num_provide_ports(), 3u);
      EXPECT_EQ(node->get_num_require_ports(), 2u);
      EXPECT_EQ(node->get_provide_port_init_data_size(), UINT16_SIZE + UINT8_SIZE + UINT8_SIZE);
      EXPECT_EQ(node->get_require_port_init_data_size(), UINT8_SIZE + UINT32_SIZE);
      auto provide_port_data = node->get_provide_port_init_data();
      auto require_port_data = node->get_require_port_init_data();
      EXPECT_NE(provide_port_data, nullptr);
      EXPECT_NE(require_port_data, nullptr);
      EXPECT_EQ(provide_port_data[0], 0xffu);
      EXPECT_EQ(provide_port_data[1], 0xffu);
      EXPECT_EQ(provide_port_data[2], 7u);
      EXPECT_EQ(provide_port_data[3], 15u);
      EXPECT_EQ(require_port_data[0], 7u);
      EXPECT_EQ(require_port_data[1], 0u);
      EXPECT_EQ(require_port_data[2], 0u);
      EXPECT_EQ(require_port_data[3], 0u);
      EXPECT_EQ(require_port_data[4], 0u);
   }

   TEST(NodeManager, buildNodeContainingOutOfRangeInitValue)
   {
      const char* apx_text =
         "APX/1.2\n"
         "N\"TestNode\"\n"
         "P\"CabTiltLockWarning\"C(0,7):=15\n";
      apx::NodeManager manager;
      EXPECT_EQ(manager.build_node(apx_text), APX_VALUE_RANGE_ERROR);
   }

   TEST(NodeManager, buildNodeContainingUnsignedArrayTypes)
   {
      const char* apx_text =
         "APX/1.2\n"
         "N\"TestNode\"\n"
         "P\"U8Array\"C(0,7)[4]:={7, 7, 7, 7}\n"
         "P\"U16Array\"S[3]:={0xFFFF, 0xFFFF, 0xFFFF}\n"
         "P\"U32Array\"L[2]:={0xFFFFFFFF, 0xFFFFFFFF}\n";
      apx::NodeManager manager;
      EXPECT_EQ(manager.build_node(apx_text), APX_NO_ERROR);
      auto* node = manager.get_last_attached();
      EXPECT_NE(node, nullptr);
      EXPECT_EQ(node->get_num_provide_ports(), 3u);
      EXPECT_EQ(node->get_num_require_ports(), 0u);
      EXPECT_EQ(node->get_provide_port_init_data_size(), UINT8_SIZE*4u + UINT16_SIZE*3 +  UINT32_SIZE*2);
      auto provide_port_data = node->get_provide_port_init_data();
      auto require_port_data = node->get_require_port_init_data();
      EXPECT_NE(provide_port_data, nullptr);
      EXPECT_EQ(require_port_data, nullptr);
      EXPECT_EQ(provide_port_data[0], 7u);
      EXPECT_EQ(provide_port_data[1], 7u);
      EXPECT_EQ(provide_port_data[2], 7u);
      EXPECT_EQ(provide_port_data[3], 7u);
      for (std::size_t i = 4u; i < 18u; i++)
      {
         EXPECT_EQ(provide_port_data[i], 0xffu);
      }
      EXPECT_EQ(provide_port_data[4], 0xffu);
   }

   TEST(NodeManager, buildNodeContainingSignedTypesWithoutLimits)
   {
      const char* apx_text =
         "APX/1.2\n"
         "N\"TestNode\"\n"
         "P\"S8Value\"c:=1\n"
         "P\"S16Value\"s:=2\n"
         "P\"S32Value\"l:=3\n";
      apx::NodeManager manager;
      EXPECT_EQ(manager.build_node(apx_text), APX_NO_ERROR);
      auto* node = manager.get_last_attached();
      EXPECT_NE(node, nullptr);
      EXPECT_EQ(node->get_num_provide_ports(), 3u);
      EXPECT_EQ(node->get_provide_port_init_data_size(), INT8_SIZE + INT16_SIZE + INT32_SIZE);
      auto provide_port_data = node->get_provide_port_init_data();
      EXPECT_NE(provide_port_data, nullptr);
      EXPECT_EQ(provide_port_data[0], 1u);
      EXPECT_EQ(provide_port_data[1], 2u);
      EXPECT_EQ(provide_port_data[2], 0u);
      EXPECT_EQ(provide_port_data[3], 3u);
      EXPECT_EQ(provide_port_data[4], 0u);
      EXPECT_EQ(provide_port_data[5], 0u);
      EXPECT_EQ(provide_port_data[6], 0u);
   }

   TEST(NodeManager, buildNodeContainingSignedTypesWithLimits)
   {
      const char* apx_text =
         "APX/1.2\n"
         "N\"TestNode\"\n"
         "P\"S8Value\"c(-10,10):=-1\n"
         "P\"S16Value\"s(-1000, 1000):=0\n"
         "P\"S32Value\"l:=-1\n";
      apx::NodeManager manager;
      EXPECT_EQ(manager.build_node(apx_text), APX_NO_ERROR);
      auto* node = manager.get_last_attached();
      EXPECT_NE(node, nullptr);
      EXPECT_EQ(node->get_num_provide_ports(), 3u);
      EXPECT_EQ(node->get_provide_port_init_data_size(), INT8_SIZE + INT16_SIZE + INT32_SIZE);
      auto provide_port_data = node->get_provide_port_init_data();
      EXPECT_NE(provide_port_data, nullptr);
      EXPECT_EQ(provide_port_data[0], 0xffu);
      EXPECT_EQ(provide_port_data[1], 0u);
      EXPECT_EQ(provide_port_data[2], 0u);
      EXPECT_EQ(provide_port_data[3], 0xffu);
      EXPECT_EQ(provide_port_data[4], 0xffu);
      EXPECT_EQ(provide_port_data[5], 0xffu);
      EXPECT_EQ(provide_port_data[6], 0xffu);
   }

   TEST(NodeManager, buildNodeContainingSignedArrayTypes)
   {
      const char* apx_text =
         "APX/1.2\n"
         "N\"TestNode\"\n"
         "P\"U8Array\"c(-10, 10)[4]:={-10, -10, -10, -10}\n"
         "P\"U16Array\"s[3]:={-1, -1, 0}\n"
         "P\"U32Array\"l[2]:={0, -1}\n";
      apx::NodeManager manager;
      EXPECT_EQ(manager.build_node(apx_text), APX_NO_ERROR);
      auto* node = manager.get_last_attached();
      EXPECT_NE(node, nullptr);
      EXPECT_EQ(node->get_num_provide_ports(), 3u);
      EXPECT_EQ(node->get_provide_port_init_data_size(), INT8_SIZE * 4 + INT16_SIZE * 3 + INT32_SIZE * 2);
      auto provide_port_data = node->get_provide_port_init_data();
      EXPECT_NE(provide_port_data, nullptr);
      EXPECT_EQ(provide_port_data[0], 0xf6u);
      EXPECT_EQ(provide_port_data[1], 0xf6u);
      EXPECT_EQ(provide_port_data[2], 0xf6u);
      EXPECT_EQ(provide_port_data[3], 0xf6u);
      EXPECT_EQ(provide_port_data[4], 0xffu);
      EXPECT_EQ(provide_port_data[5], 0xffu);
      EXPECT_EQ(provide_port_data[6], 0xffu);
      EXPECT_EQ(provide_port_data[7], 0xffu);
      EXPECT_EQ(provide_port_data[8], 0x00u);
      EXPECT_EQ(provide_port_data[9], 0x00u);
      EXPECT_EQ(provide_port_data[10], 0x00u);
      EXPECT_EQ(provide_port_data[11], 0x00u);
      EXPECT_EQ(provide_port_data[12], 0x00u);
      EXPECT_EQ(provide_port_data[13], 0x00u);
      EXPECT_EQ(provide_port_data[14], 0xffu);
      EXPECT_EQ(provide_port_data[15], 0xffu);
      EXPECT_EQ(provide_port_data[16], 0xffu);
      EXPECT_EQ(provide_port_data[17], 0xffu);
   }

   TEST(NodeManager, buildNodeContainingStringType)
   {
      const char* apx_text =
         "APX/1.2\n"
         "N\"TestNode\"\n"
         "P\"String\"a[12]:=\"Hello World!\"\n";
      apx::NodeManager manager;
      EXPECT_EQ(manager.build_node(apx_text), APX_NO_ERROR);
      auto* node = manager.get_last_attached();
      EXPECT_NE(node, nullptr);
      EXPECT_EQ(node->get_num_provide_ports(), 1u);
      EXPECT_EQ(node->get_provide_port_init_data_size(), CHAR_SIZE * 12);
      auto provide_port_data = node->get_provide_port_init_data();
      EXPECT_NE(provide_port_data, nullptr);
      EXPECT_EQ(provide_port_data[0], 'H');
      EXPECT_EQ(provide_port_data[1], 'e');
      EXPECT_EQ(provide_port_data[2], 'l');
      EXPECT_EQ(provide_port_data[3], 'l');
      EXPECT_EQ(provide_port_data[4], 'o');
      EXPECT_EQ(provide_port_data[5], ' ');
      EXPECT_EQ(provide_port_data[6], 'W');
      EXPECT_EQ(provide_port_data[7], 'o');
      EXPECT_EQ(provide_port_data[8], 'r');
      EXPECT_EQ(provide_port_data[9], 'l');
      EXPECT_EQ(provide_port_data[10], 'd');
      EXPECT_EQ(provide_port_data[11], '!');
   }

   TEST(NodeManager, buildNodeContainingRecordType)
   {
      const char* apx_text =
         "APX/1.2\n"
         "N\"TestNode\"\n"
         "P\"RecordPort\"{\"First\"C\"Second\"S}:={0xab, 0x1234}\n";
      apx::NodeManager manager;
      EXPECT_EQ(manager.build_node(apx_text), APX_NO_ERROR);
      auto* node = manager.get_last_attached();
      EXPECT_NE(node, nullptr);
      EXPECT_EQ(node->get_num_provide_ports(), 1u);
      EXPECT_EQ(node->get_provide_port_init_data_size(), UINT8_SIZE + UINT16_SIZE);
      auto provide_port_data = node->get_provide_port_init_data();
      EXPECT_NE(provide_port_data, nullptr);
      EXPECT_EQ(provide_port_data[0], 0xabu);
      EXPECT_EQ(provide_port_data[1], 0x34u);
      EXPECT_EQ(provide_port_data[2], 0x12u);

   }

}
