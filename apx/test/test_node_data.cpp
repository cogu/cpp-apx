#include "pch.h"
#include <array>
#include "cpp-apx/node_data.h"
#include "cpp-apx/vmdefs.h"

using namespace apx::vm;

namespace apx_test
{
   TEST(NodeData, CreateEmptyNodeData)
   {
      apx::NodeData node_data;
      ASSERT_EQ(node_data.definition_data_size(), 0u);
      ASSERT_EQ(node_data.provide_port_data_size(), 0u);
      ASSERT_EQ(node_data.require_port_data_size(), 0u);
   }

   TEST(NodeData, WriteProvidePortDataUint8)
   {
      std::array<std::uint8_t, UINT8_SIZE> provide_port_init{ 0x07u };
      apx::NodeData node_data;
      std::array<std::uint8_t, UINT8_SIZE> buf{ 0 };
      ASSERT_EQ(node_data.create_provide_port_data(1u, provide_port_init.data(), provide_port_init.size()), APX_NO_ERROR);
      ASSERT_EQ(node_data.read_provide_port_data(0u, buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(buf[0], 0x07);
      std::array<std::uint8_t, UINT8_SIZE> new_value{ 0x03u };
      ASSERT_EQ(node_data.write_provide_port_data(0u, new_value.data(), new_value.size()), APX_NO_ERROR);
      ASSERT_EQ(node_data.read_provide_port_data(0u, buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(buf[0], 0x03);
   }

   TEST(NodeData, WriteProvidePortDataUint16)
   {
      std::array<std::uint8_t, UINT16_SIZE> provide_port_init{ 0xff, 0xff };
      apx::NodeData node_data;
      std::array<std::uint8_t, UINT16_SIZE> buf{ 0, 0 };
      ASSERT_EQ(node_data.create_provide_port_data(1u, provide_port_init.data(), provide_port_init.size()), APX_NO_ERROR);
      std::array<std::uint8_t, UINT16_SIZE> new_value{ 0x34u, 0x12u  };
      ASSERT_EQ(node_data.write_provide_port_data(0u, new_value.data(), new_value.size()), APX_NO_ERROR);
      ASSERT_EQ(node_data.read_provide_port_data(0u, buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(buf[0], 0x34);
      ASSERT_EQ(buf[1], 0x12);
   }

   TEST(NodeData, WriteProvidePortDataUint32)
   {
      std::array<std::uint8_t, UINT32_SIZE> provide_port_init{ 0xffu, 0xffu, 0xffu, 0xffu };
      apx::NodeData node_data;
      std::array<std::uint8_t, UINT32_SIZE> buf{ 0u, 0u, 0u, 0u };
      ASSERT_EQ(node_data.create_provide_port_data(1u, provide_port_init.data(), provide_port_init.size()), APX_NO_ERROR);
      std::array<std::uint8_t, UINT32_SIZE> new_value{ 0x78u, 0x56u, 0x34u, 0x12u };
      ASSERT_EQ(node_data.write_provide_port_data(0u, new_value.data(), new_value.size()), APX_NO_ERROR);
      ASSERT_EQ(node_data.read_provide_port_data(0u, buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(buf[0], 0x78u);
      ASSERT_EQ(buf[1], 0x56u);
      ASSERT_EQ(buf[2], 0x34u);
      ASSERT_EQ(buf[3], 0x12u);
   }

   TEST(NodeData, WriteRequirePortDataUint8)
   {
      std::array<std::uint8_t, UINT8_SIZE> require_port_init{ 0x07u };
      apx::NodeData node_data;
      std::array<std::uint8_t, UINT8_SIZE> buf{ 0 };
      ASSERT_EQ(node_data.create_require_port_data(1u, require_port_init.data(), require_port_init.size()), APX_NO_ERROR);
      ASSERT_EQ(node_data.read_require_port_data(0u, buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(buf[0], 0x07);
      std::array<std::uint8_t, UINT8_SIZE> new_value{ 0x03u };
      ASSERT_EQ(node_data.write_require_port_data(0u, new_value.data(), new_value.size()), APX_NO_ERROR);
      ASSERT_EQ(node_data.read_require_port_data(0u, buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(buf[0], 0x03);
   }

   TEST(NodeData, WriteRequirePortDataUint16)
   {
      std::array<std::uint8_t, UINT16_SIZE> require_port_init{ 0xff, 0xff };
      apx::NodeData node_data;
      std::array<std::uint8_t, UINT16_SIZE> buf{ 0, 0 };
      ASSERT_EQ(node_data.create_require_port_data(1u, require_port_init.data(), require_port_init.size()), APX_NO_ERROR);
      std::array<std::uint8_t, UINT16_SIZE> new_value{ 0x34u, 0x12u };
      ASSERT_EQ(node_data.write_require_port_data(0u, new_value.data(), new_value.size()), APX_NO_ERROR);
      ASSERT_EQ(node_data.read_require_port_data(0u, buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(buf[0], 0x34);
      ASSERT_EQ(buf[1], 0x12);
   }

   TEST(NodeData, WriteRequirePortDataUint32)
   {
      std::array<std::uint8_t, UINT32_SIZE> require_port_init{ 0xffu, 0xffu, 0xffu, 0xffu };
      apx::NodeData node_data;
      std::array<std::uint8_t, UINT32_SIZE> buf{ 0u, 0u, 0u, 0u };
      ASSERT_EQ(node_data.create_require_port_data(1u, require_port_init.data(), require_port_init.size()), APX_NO_ERROR);
      std::array<std::uint8_t, UINT32_SIZE> new_value{ 0x78u, 0x56u, 0x34u, 0x12u };
      ASSERT_EQ(node_data.write_require_port_data(0u, new_value.data(), new_value.size()), APX_NO_ERROR);
      ASSERT_EQ(node_data.read_require_port_data(0u, buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(buf[0], 0x78u);
      ASSERT_EQ(buf[1], 0x56u);
      ASSERT_EQ(buf[2], 0x34u);
      ASSERT_EQ(buf[3], 0x12u);
   }

}