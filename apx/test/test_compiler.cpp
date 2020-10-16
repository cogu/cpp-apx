#include "pch.h"
#include "cpp-apx/compiler.h"
#include "cpp-apx/parser.h"
#include <sstream>

using namespace apx::vm;

namespace apx_test
{
   TEST(CompilerPack, PackU8)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"C:=255\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT8_SIZE,
         OPCODE_PACK | (VARIANT_U8 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackU8Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"C[2]:={255, 255}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT8_SIZE * 2,
         INST_FLAG | OPCODE_PACK | (VARIANT_U8 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackU8WithRangeCheck)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"C(0,3):=3\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT8_SIZE,
         OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_U8 << INST_VARIANT_SHIFT),
         0u,
         3u,
         OPCODE_PACK | (VARIANT_U8 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackU8ArrayWithRangeCheck)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"C(0,3)[2]:={3,3}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT8_SIZE * 2,
         INST_FLAG | OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_U8 << INST_VARIANT_SHIFT),
         0u,
         3u,
         INST_FLAG | OPCODE_PACK | (VARIANT_U8 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }


   TEST(CompilerPack, PackQueuedU8)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "P\"U8QueuedSignal\"C:Q[10]\n";

      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };

      auto port = node->get_provide_port(0u);
      ASSERT_NE(port, nullptr);
      ASSERT_TRUE(port->is_queued());
      ASSERT_EQ(port->get_queue_length(), 10u);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_FLAG_QUEUED_DATA | HEADER_PROG_TYPE_PACK | VARIANT_U8, 1 + UINT8_SIZE * 10,
         OPCODE_DATA_SIZE | (VARIANT_ELEMENT_SIZE_U8_QUEUE_SIZE_U8 << INST_VARIANT_SHIFT),
         UINT8_SIZE,
         OPCODE_PACK | (VARIANT_U8 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackU8DynamicArray)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"C[8*]\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_FLAG_DYNAMIC_DATA | HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT8_SIZE * 8,
         INST_FLAG | OPCODE_PACK | (VARIANT_U8 << INST_VARIANT_SHIFT),
         INST_FLAG | OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         8u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackS8)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S8Port\"c:=0\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, INT8_SIZE,
         OPCODE_PACK | (VARIANT_S8 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackS8Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S8Port\"c[2]:={-1, -1}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, INT8_SIZE * 2,
         INST_FLAG | OPCODE_PACK | (VARIANT_S8 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackS8WithLimit)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S8Port\"c(-10,10):=0\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, INT8_SIZE,
         OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_S8 << INST_VARIANT_SHIFT),
         0xF6,
         0x0A,
         OPCODE_PACK | (VARIANT_S8 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackS8ArrayWithLimit)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"c(-10,10)[2]:={0,0}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, INT8_SIZE * 2,
         INST_FLAG | OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_S8 << INST_VARIANT_SHIFT),
         0xF6,
         0x0A,
         INST_FLAG | OPCODE_PACK | (VARIANT_S8 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackU16)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U16Port\"S:=65535\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT16_SIZE,
         OPCODE_PACK | (VARIANT_U16 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackU16Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U16Port\"S[2]:={65535, 65535}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT16_SIZE * 2,
         INST_FLAG | OPCODE_PACK | (VARIANT_U16 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackS16)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S16Port\"s:=-1\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, INT16_SIZE,
         OPCODE_PACK | (VARIANT_S16 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackS16Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S16Port\"s[2]:={-1, -1}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, INT16_SIZE * 2,
         INST_FLAG | OPCODE_PACK | (VARIANT_S16 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackU32)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U32Port\"L:=0xFFFFFFFF\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT32_SIZE,
         OPCODE_PACK | (VARIANT_U32 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackU32Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U32Port\"L[2]:={0xFFFFFFFF, 0xFFFFFFFF}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT32_SIZE * 2,
         INST_FLAG | OPCODE_PACK | (VARIANT_U32 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackS32)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S16Port\"l:=-1\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, INT32_SIZE,
         OPCODE_PACK | (VARIANT_S32 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackS32Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S32Port\"l[2]:={-1, -1}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, INT32_SIZE * 2,
         INST_FLAG | OPCODE_PACK | (VARIANT_S32 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackByte)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"BytePort\"B:=255\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, BYTE_SIZE,
         OPCODE_PACK | (VARIANT_BYTE << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackByteArray)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"BytePort\"B[4095]\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U16, 0xFF, 0x0F, //4095 encoded as little endian
         INST_FLAG | OPCODE_PACK | (VARIANT_BYTE << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U16 << INST_VARIANT_SHIFT),
         0xFF,
         0x0F
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackDynamicByteArray)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"BytePort\"B[4095*]\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_FLAG_DYNAMIC_DATA | HEADER_PROG_TYPE_PACK | VARIANT_U16, 0xFF, 0x0F, //4095 encoded as little endian
         INST_FLAG | OPCODE_PACK | (VARIANT_BYTE << INST_VARIANT_SHIFT),
         INST_FLAG | OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U16 << INST_VARIANT_SHIFT),
         0xFF,
         0x0F
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackCharString)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"StringPort\"a[120]\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, 120,
         INST_FLAG | OPCODE_PACK | (VARIANT_ASCII_CHAR << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         120,
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackChar8String)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"StringPort\"A[120]\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, 120,
         INST_FLAG | OPCODE_PACK | (VARIANT_CHAR8 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         120,
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackDynamicString)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"BytePort\"a[256*]:={}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_FLAG_DYNAMIC_DATA | HEADER_PROG_TYPE_PACK | VARIANT_U16, 0x00, 0x01, //256 in little endian
         INST_FLAG | OPCODE_PACK | (VARIANT_ASCII_CHAR << INST_VARIANT_SHIFT),
         INST_FLAG | OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U16 << INST_VARIANT_SHIFT),
         0x00,
         0x01
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackRecord_U8_U16)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"RecordPort\"{\"First\"C\"Second\"S}:={255, 65535}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT8_SIZE + UINT16_SIZE,
         OPCODE_PACK | (VARIANT_RECORD << INST_VARIANT_SHIFT),
         OPCODE_DATA_CTRL | (VARIANT_RECORD_SELECT << INST_VARIANT_SHIFT),
         'F', 'i', 'r', 's', 't', '\0',
         OPCODE_PACK | (VARIANT_U8 << INST_VARIANT_SHIFT),
         LAST_FIELD_FLAG | OPCODE_DATA_CTRL | (VARIANT_RECORD_SELECT << INST_VARIANT_SHIFT),
         'S', 'e', 'c', 'o', 'n', 'd', '\0',
         OPCODE_PACK | (VARIANT_U16 << INST_VARIANT_SHIFT),
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackUInt8Reference)
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
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT8_SIZE,
         OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_U8 << INST_VARIANT_SHIFT),
         0u,
         3u,
         OPCODE_PACK | (VARIANT_U8 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerPack, PackRecordTypeRefsContainingElementTypeRefsById)
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
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Pack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT8_SIZE + UINT8_SIZE,
         OPCODE_PACK | (VARIANT_RECORD << INST_VARIANT_SHIFT),
         OPCODE_DATA_CTRL | (VARIANT_RECORD_SELECT << INST_VARIANT_SHIFT),
         'F', 'i', 'r', 's', 't', '\0',
         OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_U8 << INST_VARIANT_SHIFT),
         0u,
         3u,
         OPCODE_PACK | (VARIANT_U8 << INST_VARIANT_SHIFT),
         LAST_FIELD_FLAG | OPCODE_DATA_CTRL | (VARIANT_RECORD_SELECT << INST_VARIANT_SHIFT),
         'S', 'e', 'c', 'o', 'n', 'd', '\0',
         OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_U8 << INST_VARIANT_SHIFT),
         0u,
         7u,
         OPCODE_PACK | (VARIANT_U8 << INST_VARIANT_SHIFT),
      };
      ASSERT_EQ(*program, expected);
   }

   /*********************** UNPACK TESTS ***********************/

   TEST(CompilerUnpack, UnpackU8)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"C:=255\n";
      apx::Parser parser;
      EXPECT_EQ(parser.parse(apx_text), APX_NO_ERROR);
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, UINT8_SIZE,
         OPCODE_UNPACK | (VARIANT_U8 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackU8Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"C[2]:={255, 255}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, UINT8_SIZE * 2,
         INST_FLAG | OPCODE_UNPACK | (VARIANT_U8 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackU8WithRangeCheck)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"C(0,3):=3\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, UINT8_SIZE,
         OPCODE_UNPACK | (VARIANT_U8 << INST_VARIANT_SHIFT),
         OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_U8 << INST_VARIANT_SHIFT),
         0u,
         3u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackU8ArrayWithRangeCheck)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"C(0,3)[2]:={3,3}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, UINT8_SIZE * 2,
         INST_FLAG | OPCODE_UNPACK | (VARIANT_U8 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u,
         INST_FLAG | OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_U8 << INST_VARIANT_SHIFT),
         0u,
         3u,
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackQueuedU8)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "P\"U8QueuedSignal\"C:Q[10]\n";

      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };

      auto port = node->get_provide_port(0u);
      ASSERT_NE(port, nullptr);
      ASSERT_TRUE(port->is_queued());
      ASSERT_EQ(port->get_queue_length(), 10u);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_FLAG_QUEUED_DATA | HEADER_PROG_TYPE_UNPACK | VARIANT_U8, 1 + UINT8_SIZE * 10,
         OPCODE_DATA_SIZE | (VARIANT_ELEMENT_SIZE_U8_QUEUE_SIZE_U8 << INST_VARIANT_SHIFT), UINT8_SIZE,
         OPCODE_UNPACK | (VARIANT_U8 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackDynamicU8Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"C[8*]\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_FLAG_DYNAMIC_DATA | HEADER_PROG_TYPE_UNPACK | VARIANT_U8, UINT8_SIZE * 8,
         ARRAY_FLAG | OPCODE_UNPACK | (VARIANT_U8 << INST_VARIANT_SHIFT),
         DYN_ARRAY_FLAG | OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         8u,
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackDynamicU8ArrayWithRangeCheck)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"C(0,15)[4*]\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_FLAG_DYNAMIC_DATA | HEADER_PROG_TYPE_UNPACK | VARIANT_U8, UINT8_SIZE * 4,
         ARRAY_FLAG | OPCODE_UNPACK | (VARIANT_U8 << INST_VARIANT_SHIFT),
         DYN_ARRAY_FLAG | OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         4u,
         ARRAY_FLAG | OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_U8 << INST_VARIANT_SHIFT),
         0u,
         15u,
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackS8)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S8Port\"c:=0\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, INT8_SIZE,
         OPCODE_UNPACK | (VARIANT_S8 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackS8WithRangeCheck)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S8Port\"c(-10,10):=0\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, INT8_SIZE,
         OPCODE_UNPACK | (VARIANT_S8 << INST_VARIANT_SHIFT),
         OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_S8 << INST_VARIANT_SHIFT),
         0xF6,
         0x0A
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackS8Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S8ArrayPort\"c[2]:={-1, -1}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, INT8_SIZE * 2,
         ARRAY_FLAG | OPCODE_UNPACK | (VARIANT_S8 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackS8ArrayWithRangeCheck)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S8ArrayPort\"c(-10,10)[2]:={0,0}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, INT8_SIZE * 2,
         ARRAY_FLAG | OPCODE_UNPACK | (VARIANT_S8 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u,
         ARRAY_FLAG | OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_S8 << INST_VARIANT_SHIFT),
         0xF6,
         0x0A
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackU16)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U16Port\"S:=65535\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, UINT16_SIZE,
         OPCODE_UNPACK | (VARIANT_U16 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackU16WithRangeCheck)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U16Port\"S(0,1023):=1023\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, UINT16_SIZE,
         OPCODE_UNPACK | (VARIANT_U16 << INST_VARIANT_SHIFT),
         OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_U16 << INST_VARIANT_SHIFT),
         0x00, //lower-limit LSB
         0x00, //lower-limit MSB
         0xFF, //upper-limit LSB
         0x03, //upper-limit MSB
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackU16Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U16Port\"S[2]:={65535, 65535}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, UINT16_SIZE * 2,
         ARRAY_FLAG | OPCODE_UNPACK | (VARIANT_U16 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackU16ArrayWithRangeCheck)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U16Port\"S(0,1023)[2]:={65535, 65535}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, UINT16_SIZE * 2,
         ARRAY_FLAG | OPCODE_UNPACK | (VARIANT_U16 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u,
         ARRAY_FLAG | OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_U16 << INST_VARIANT_SHIFT),
         0x00, //lower-limit LSB
         0x00, //lower-limit MSB
         0xFF, //upper-limit LSB
         0x03, //upper-limit MSB
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackS16)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S16Port\"s:=-1\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, INT16_SIZE,
         OPCODE_UNPACK | (VARIANT_S16 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackS16WithRangeCheck)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S16Port\"s(-1000, 1000):=0\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, INT16_SIZE,
         OPCODE_UNPACK | (VARIANT_S16 << INST_VARIANT_SHIFT),
         OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_S16 << INST_VARIANT_SHIFT),
         0x18, //lower-limit LSB
         0xFC, //lower-limit MSB
         0xE8, //upper-limit LSB
         0x03, //upper-limit MSB
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackS16Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S16Port\"s[2]:={-1, -1}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, INT16_SIZE * 2,
         ARRAY_FLAG | OPCODE_UNPACK | (VARIANT_S16 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackS16ArrayWithRangeCheck)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S16Port\"s(-1000, 1000)[2]:={-1, -1}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, INT16_SIZE * 2,
         ARRAY_FLAG | OPCODE_UNPACK | (VARIANT_S16 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u,
         ARRAY_FLAG | OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_S16 << INST_VARIANT_SHIFT),
         0x18, //lower-limit LSB
         0xFC, //lower-limit MSB
         0xE8, //upper-limit LSB
         0x03, //upper-limit MSB
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackU32)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U32Port\"L:=0xFFFFFFFF\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, UINT32_SIZE,
         OPCODE_UNPACK | (VARIANT_U32 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackU32WithRangeCheck)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U32Port\"L(0, 99999):=0\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, UINT32_SIZE,
         OPCODE_UNPACK | (VARIANT_U32 << INST_VARIANT_SHIFT),
         OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_U32 << INST_VARIANT_SHIFT),
         0x00, //lower-limit LSB
         0x00,
         0x00,
         0x00, //lower-limit MSB
         0x9F, //upper-limit LSB
         0x86,
         0x01,
         0x00, //upper-limit MSB
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackU32Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U32Port\"L[2]:={0xFFFFFFFF, 0xFFFFFFFF}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, UINT32_SIZE * 2,
         ARRAY_FLAG | OPCODE_UNPACK | (VARIANT_U32 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackU32ArrayWithRangeCheck)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U32Port\"L(0, 99999)[2]:={99999, 99999}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, UINT32_SIZE * 2,
         ARRAY_FLAG | OPCODE_UNPACK | (VARIANT_U32 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u,
         ARRAY_FLAG | OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_U32 << INST_VARIANT_SHIFT),
         0x00, //lower-limit LSB
         0x00,
         0x00,
         0x00, //lower-limit MSB
         0x9F, //upper-limit LSB
         0x86,
         0x01,
         0x00, //upper-limit MSB
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackS32)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S32Port\"l:=-1\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, INT32_SIZE,
         OPCODE_UNPACK | (VARIANT_S32 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackS32WithRangeCheck)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S32Port\"l(-100000, 100000):=-100000\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, INT32_SIZE,
         OPCODE_UNPACK | (VARIANT_S32 << INST_VARIANT_SHIFT),
         OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_S32 << INST_VARIANT_SHIFT),
         0x60, //lower-limit LSB
         0x79,
         0xFE,
         0xFF, //lower-limit MSB
         0xA0, //upper-limit LSB
         0x86,
         0x01,
         0x00, //upper-limit MSB
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackS32Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S32Port\"l[2]:={-1, -1}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, INT32_SIZE * 2,
         ARRAY_FLAG | OPCODE_UNPACK | (VARIANT_S32 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackS32ArrayWithRangeCheck)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S32Port\"l(-100000, 100000)[2]:={-1, -1}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, INT32_SIZE * 2,
         ARRAY_FLAG | OPCODE_UNPACK | (VARIANT_S32 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u,
         ARRAY_FLAG | OPCODE_DATA_CTRL | (VARIANT_LIMIT_CHECK_S32 << INST_VARIANT_SHIFT),
         0x60, //lower-limit LSB
         0x79,
         0xFE,
         0xFF, //lower-limit MSB
         0xA0, //upper-limit LSB
         0x86,
         0x01,
         0x00, //upper-limit MSB
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackByte)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"BytePort\"B:=255\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, BYTE_SIZE,
         OPCODE_UNPACK | (VARIANT_BYTE << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackByteArray)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"BytePort\"B[4095]\n";
      apx::Parser parser;
      EXPECT_EQ(parser.parse(apx_text), APX_NO_ERROR);
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U16,
         0xFF,
         0x0F,
         INST_FLAG | OPCODE_UNPACK | (VARIANT_BYTE << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U16 << INST_VARIANT_SHIFT),
         0xFF,
         0x0F
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackDynamicByteArray)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"BytePort\"B[4095*]\n";
      apx::Parser parser;
      EXPECT_EQ(parser.parse(apx_text), APX_NO_ERROR);
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_FLAG_DYNAMIC_DATA | HEADER_PROG_TYPE_UNPACK | VARIANT_U16,
         0xFF,
         0x0F,
         ARRAY_FLAG | OPCODE_UNPACK | (VARIANT_BYTE << INST_VARIANT_SHIFT),
         DYN_ARRAY_FLAG | OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U16 << INST_VARIANT_SHIFT),
         0xFF,
         0x0F
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackCharString)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"StringPort\"a[120]\n";
      apx::Parser parser;
      EXPECT_EQ(parser.parse(apx_text), APX_NO_ERROR);
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8,
         120,
         INST_FLAG | OPCODE_UNPACK | (VARIANT_ASCII_CHAR << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         120,
      };
      ASSERT_EQ(*program, expected);
   }
   TEST(CompilerUnpack, UnpackChar8String)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"StringPort\"A[120]\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8,
         120,
         INST_FLAG | OPCODE_UNPACK | (VARIANT_CHAR8 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         120,
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackCharDynamicString)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"BytePort\"a[256*]:={}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_FLAG_DYNAMIC_DATA | HEADER_PROG_TYPE_UNPACK | VARIANT_U16,
         0x00,
         0x01,
         ARRAY_FLAG | OPCODE_UNPACK | (VARIANT_ASCII_CHAR << INST_VARIANT_SHIFT),
         DYN_ARRAY_FLAG | OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U16 << INST_VARIANT_SHIFT),
         0x00,
         0x01
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackChar8DynamicString)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"BytePort\"A[256*]:={}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_FLAG_DYNAMIC_DATA | HEADER_PROG_TYPE_UNPACK | VARIANT_U16,
         0x00,
         0x01,
         ARRAY_FLAG | OPCODE_UNPACK | (VARIANT_CHAR8 << INST_VARIANT_SHIFT),
         DYN_ARRAY_FLAG | OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U16 << INST_VARIANT_SHIFT),
         0x00,
         0x01
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(CompilerUnpack, UnpackRecord_U8_U6)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"RecordPort\"{\"First\"C\"Second\"S}:={255, 65535}\n";
      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      auto node{ parser.take_last_node() };
      auto port = node->get_require_port(0u);
      ASSERT_NE(port, nullptr);
      apx::Compiler compiler;
      apx::error_t error_code = APX_NO_ERROR;
      auto program = compiler.compile_port(port, apx::ProgramType::Unpack, error_code);
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, UINT8_SIZE + UINT16_SIZE,
         OPCODE_UNPACK | (VARIANT_RECORD << INST_VARIANT_SHIFT),
         OPCODE_DATA_CTRL | (VARIANT_RECORD_SELECT << INST_VARIANT_SHIFT),
         'F', 'i', 'r', 's', 't', '\0',
         OPCODE_UNPACK | (VARIANT_U8 << INST_VARIANT_SHIFT),
         LAST_FIELD_FLAG | OPCODE_DATA_CTRL | (VARIANT_RECORD_SELECT << INST_VARIANT_SHIFT),
         'S', 'e', 'c', 'o', 'n', 'd', '\0',
         OPCODE_UNPACK | (VARIANT_U16 << INST_VARIANT_SHIFT),
      };
      ASSERT_EQ(*program, expected);
   }



}