#include "pch.h"
#include "cpp-apx/program.h"

using namespace apx::vm;

namespace apx_test
{
   TEST(Program, InitPackHeader)
   {
      Program p;
      init_program_header(p, HEADER_PROG_TYPE_PACK);
      Program const expected{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_PACK, 0, 0, 0, 0 };
      ASSERT_EQ(p, expected);
   }
   TEST(Program, InitUnpackHeader)
   {
      Program p;
      init_program_header(p, HEADER_PROG_TYPE_UNPACK);
      Program const expected{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_UNPACK, 0, 0, 0, 0 };
      ASSERT_EQ(p, expected);
   }

   TEST(Program, ReserveU8Element)
   {
      Program p;
      reserve_elem_size_instruction(p, apx::SizeType::UInt8);
      Program const expected{ OPCODE_DATA_SIZE | (VARIANT_ELEMENT_SIZE_U8 << INST_VARIANT_SHIFT), 0u };
      ASSERT_EQ(p, expected);
   }

   TEST(Program, ReserveU16Element)
   {
      Program p;
      reserve_elem_size_instruction(p, apx::SizeType::UInt16);
      Program const expected{ OPCODE_DATA_SIZE | (VARIANT_ELEMENT_SIZE_U16 << INST_VARIANT_SHIFT), 0u, 0u };
      ASSERT_EQ(p, expected);
   }

   TEST(Program, ReserveU32Element)
   {
      Program p;
      reserve_elem_size_instruction(p, apx::SizeType::UInt32);
      Program const expected{ OPCODE_DATA_SIZE | (VARIANT_ELEMENT_SIZE_U32 << INST_VARIANT_SHIFT), 0u, 0u, 0u, 0u };
      ASSERT_EQ(p, expected);
   }

}