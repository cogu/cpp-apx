#include "pch.h"
#include "cpp-apx/compiler.h"
#include "cpp-apx/parser.h"
#include <sstream>

using namespace apx::vm;

namespace apx_test
{

   TEST(Compiler, PackU8)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"C:=255\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      Program const expected{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_PACK, 1, 0, 0, 0,
         OPCODE_PACK | (VARIANT_U8 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }
   TEST(Compiler, PackU8Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"C[2]:={255, 255}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      Program const expected{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_PACK, 2, 0, 0, 0,
         INST_FLAG | OPCODE_PACK | (VARIANT_U8 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }
   TEST(Compiler, PackU8WithLimit)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"C(0,3):=3\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      Program const expected{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_PACK, 1, 0, 0, 0,
         OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_U8 << INST_VARIANT_SHIFT),
         0u,
         3u,
         OPCODE_PACK | (VARIANT_U8 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(Compiler, PackU8ArrayWithLimit)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"C(0,3)[2]:={3,3}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      Program const expected{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_PACK, 2, 0, 0, 0,
         INST_FLAG | OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_U8 << INST_VARIANT_SHIFT),
         0u,
         3u,
         INST_FLAG | OPCODE_PACK | (VARIANT_U8 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(Compiler, PackS8)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S8Port\"c:=0\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      Program const expected{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_PACK, 1, 0, 0, 0,
         OPCODE_PACK | (VARIANT_S8 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(Compiler, PackS8Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S8Port\"c[2]:={-1, -1}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      Program const expected{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_PACK, 2, 0, 0, 0,
         INST_FLAG | OPCODE_PACK | (VARIANT_S8 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(Compiler, PackS8WithLimit)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S8Port\"c(-10,10):=0\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      Program const expected{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_PACK, 1, 0, 0, 0,
         OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_S8 << INST_VARIANT_SHIFT),
         0xF6,
         0x0A,
         OPCODE_PACK | (VARIANT_S8 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(Compiler, PackS8ArrayWithLimit)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"c(-10,10)[2]:={0,0}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      Program const expected{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_PACK, 2, 0, 0, 0,
         INST_FLAG | OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_S8 << INST_VARIANT_SHIFT),
         0xF6,
         0x0A,
         INST_FLAG | OPCODE_PACK | (VARIANT_S8 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

}