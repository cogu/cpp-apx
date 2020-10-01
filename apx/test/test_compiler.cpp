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
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT8_SIZE,
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
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT8_SIZE * 2,
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
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT8_SIZE,
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


   TEST(Compiler, PackQueuedU8)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "P\"U8QueuedSignal\"C:Q[10]\n";

      apx::Parser parser;
      std::stringstream ss;
      ss.str(apx_text);
      EXPECT_TRUE(parser.parse(ss));
      std::unique_ptr<apx::Node> node{ parser.take_last_node() };

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

   TEST(Compiler, PackU8ArrayDynamic)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U8Port\"C[8*]\n";
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
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_FLAG_DYNAMIC_DATA | HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT8_SIZE * 8,
         INST_FLAG | OPCODE_PACK | (VARIANT_U8 << INST_VARIANT_SHIFT),
         INST_FLAG | OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         8u
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
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, INT8_SIZE,
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
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, INT8_SIZE * 2,
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
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, INT8_SIZE,
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

   TEST(Compiler, PackU16)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U16Port\"S:=65535\n";
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
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT16_SIZE,
         OPCODE_PACK | (VARIANT_U16 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(Compiler, PackU16Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U16Port\"S[2]:={65535, 65535}\n";
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
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT16_SIZE * 2,
         INST_FLAG | OPCODE_PACK | (VARIANT_U16 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(Compiler, PackS16)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S16Port\"s:=-1\n";
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
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, INT16_SIZE,
         OPCODE_PACK | (VARIANT_S16 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(Compiler, PackS16Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S16Port\"s[2]:={-1, -1}\n";
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
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, INT16_SIZE * 2,
         INST_FLAG | OPCODE_PACK | (VARIANT_S16 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(Compiler, PackU32)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U32Port\"L:=0xFFFFFFFF\n";
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
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT32_SIZE,
         OPCODE_PACK | (VARIANT_U32 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(Compiler, PackU32Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"U32Port\"L[2]:={0xFFFFFFFF, 0xFFFFFFFF}\n";
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
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT32_SIZE * 2,
         INST_FLAG | OPCODE_PACK | (VARIANT_U32 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(Compiler, PackS32)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S16Port\"l:=-1\n";
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
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, INT32_SIZE,
         OPCODE_PACK | (VARIANT_S32 << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(Compiler, PackS32Array)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"S32Port\"l[2]:={-1, -1}\n";
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
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, INT32_SIZE * 2,
         INST_FLAG | OPCODE_PACK | (VARIANT_S32 << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         2u
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(Compiler, PackByte)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"BytePort\"B:=255\n";
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
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, BYTE_SIZE,
         OPCODE_PACK | (VARIANT_BYTE << INST_VARIANT_SHIFT)
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(Compiler, PackByteArray)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"BytePort\"B[4095]\n";
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
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U16, 0xFF, 0x0F, //4095 encoded as little endian
         INST_FLAG | OPCODE_PACK | (VARIANT_BYTE << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U16 << INST_VARIANT_SHIFT),
         0xFF,
         0x0F
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(Compiler, PackDynamicByteArray)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"BytePort\"B[4095*]\n";
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
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_FLAG_DYNAMIC_DATA | HEADER_PROG_TYPE_PACK | VARIANT_U16, 0xFF, 0x0F, //4095 encoded as little endian
         INST_FLAG | OPCODE_PACK | (VARIANT_BYTE << INST_VARIANT_SHIFT),
         INST_FLAG | OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U16 << INST_VARIANT_SHIFT),
         0xFF,
         0x0F
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(Compiler, PackString)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"StringPort\"a[120]\n";
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
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, 120,
         INST_FLAG | OPCODE_PACK | (VARIANT_ASCII_CHAR << INST_VARIANT_SHIFT),
         OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U8 << INST_VARIANT_SHIFT),
         120,
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(Compiler, PackDynamicString)
   {
      const char* apx_text =
         "APX/1.3\n"
         "N\"TestNode\"\n"
         "R\"BytePort\"a[256*]:={}\n";
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
      ASSERT_EQ(error_code, APX_NO_ERROR);
      Program const expected{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_FLAG_DYNAMIC_DATA | HEADER_PROG_TYPE_PACK | VARIANT_U16, 0x00, 0x01, //256 in little endian
         INST_FLAG | OPCODE_PACK | (VARIANT_ASCII_CHAR << INST_VARIANT_SHIFT),
         INST_FLAG | OPCODE_DATA_SIZE | (VARIANT_ARRAY_SIZE_U16 << INST_VARIANT_SHIFT),
         0x00,
         0x01
      };
      ASSERT_EQ(*program, expected);
   }

   TEST(Compiler, PackRecord_UINT8_UINT16)
   {
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
}