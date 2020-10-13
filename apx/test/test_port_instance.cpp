#include "pch.h"
#include "cpp-apx/port_instance.h"
#include "cpp-apx/parser.h"
#include "cpp-apx/compiler.h"
#include <string>
#include <iostream>
#include <cstddef>

using namespace std::string_literals;


namespace apx_test
{
   TEST(PortInstance, DerivePropertiesOnProvidePortUint8)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "P\"U8Signal\"C:=255\n";

      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      EXPECT_EQ(node->finalize(), APX_NO_ERROR);
      auto port = node->get_provide_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t result{ APX_NO_ERROR };
      auto pack_program = compiler.compile_port(port, apx::ProgramType::Pack, result);
      ASSERT_EQ(result, APX_NO_ERROR);
      apx::PortInstance port_instance( apx::PortType::ProvidePort, 0, "U8Signal"s, pack_program.release(), nullptr );
      std::uint32_t data_size{ 0 };
      ASSERT_EQ(port_instance.derive_properties(0u, data_size), APX_NO_ERROR);
      ASSERT_EQ(data_size, sizeof(std::uint8_t));
      ASSERT_EQ(data_size, port_instance.get_data_size());
   }

   TEST(PortInstance, DerivePropertiesOnProvidePortUint16)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "P\"U16Signal\"S:=65535\n";

      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      EXPECT_EQ(node->finalize(), APX_NO_ERROR);
      auto port = node->get_provide_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t result{ APX_NO_ERROR };
      auto pack_program = compiler.compile_port(port, apx::ProgramType::Pack, result);
      ASSERT_EQ(result, APX_NO_ERROR);
      apx::PortInstance port_instance(apx::PortType::ProvidePort, 0, "U16Signal"s, pack_program.release(), nullptr);
      std::uint32_t data_size{ 0 };
      ASSERT_EQ(port_instance.derive_properties(0u, data_size), APX_NO_ERROR);
      ASSERT_EQ(data_size, sizeof(std::uint16_t));
      ASSERT_EQ(data_size, port_instance.get_data_size());
   }

   TEST(PortInstance, DerivePropertiesOnProvidePortUint32)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "P\"U32Signal\"L:=0xFFFFFFFF\n";

      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      EXPECT_EQ(node->finalize(), APX_NO_ERROR);
      auto port = node->get_provide_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t result{ APX_NO_ERROR };
      auto pack_program = compiler.compile_port(port, apx::ProgramType::Pack, result);
      ASSERT_EQ(result, APX_NO_ERROR);
      apx::PortInstance port_instance(apx::PortType::ProvidePort, 0, "U32Signal"s, pack_program.release(), nullptr);
      std::uint32_t data_size{ 0 };
      ASSERT_EQ(port_instance.derive_properties(0u, data_size), APX_NO_ERROR);
      ASSERT_EQ(data_size, sizeof(std::uint32_t));
      ASSERT_EQ(data_size, port_instance.get_data_size());
   }

   TEST(PortInstance, DerivePropertiesOnQueuedProvidePortWithUint8DataAndUint8QueueSize)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "P\"U8Signal\"C:Q[10]\n";

      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      auto port = node->get_provide_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t result{ APX_NO_ERROR };
      auto pack_program = compiler.compile_port(port, apx::ProgramType::Pack, result);
      ASSERT_EQ(result, APX_NO_ERROR);
      apx::PortInstance port_instance(apx::PortType::ProvidePort, 0, "U8Signal"s, pack_program.release(), nullptr);
      std::uint32_t data_size{ 0 };
      ASSERT_EQ(port_instance.derive_properties(0u, data_size), APX_NO_ERROR);
      ASSERT_EQ(data_size, sizeof(std::uint8_t)+sizeof(std::uint8_t)*10u );
      ASSERT_EQ(data_size, port_instance.get_data_size());
      ASSERT_EQ(port_instance.get_queue_length(), 10u);
      ASSERT_EQ(port_instance.get_element_size(), sizeof(std::uint8_t));
   }
}
