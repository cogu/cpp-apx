#include "pch.h"
#include "cpp-apx/program.h"

using namespace apx::vm;

namespace apx_test
{
   TEST(Program, InitPackHeaderU8)
   {
      Program min_header;
      create_program_header(min_header, apx::ProgramType::Pack, 0, 0, false);
      Program const min_expected{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_PACK | VARIANT_U8, 0};
      ASSERT_EQ(min_header, min_expected);

      Program max_header;
      create_program_header(max_header, apx::ProgramType::Pack, 255, 0, false);
      Program const max_expected{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_PACK | VARIANT_U8, 255 };
      ASSERT_EQ(max_header, max_expected);
   }

   TEST(Program, InitPackHeaderU16)
   {
      Program min_header;
      create_program_header(min_header, apx::ProgramType::Pack, 256, 0, false);
      Program const min_expected{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_PACK | VARIANT_U16, 0, 1 }; //256 encoded as LE
      ASSERT_EQ(min_header, min_expected);

      Program max_header;
      create_program_header(max_header, apx::ProgramType::Pack, 65535, 0, false);
      Program const max_expected{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_PACK | VARIANT_U16, 0xFF, 0xFF }; //65565 in LE
      ASSERT_EQ(max_header, max_expected);
   }

   TEST(Program, InitPackHeaderU32)
   {
      Program min_header;
      create_program_header(min_header, apx::ProgramType::Pack, 65536, 0, false);
      Program const min_expected{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_PACK | VARIANT_U32, 0, 0, 1, 0 };
      ASSERT_EQ(min_header, min_expected);

      Program max_header;
      create_program_header(max_header, apx::ProgramType::Pack, UINT32_MAX, 0, false);
      Program const max_expected{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_PACK | VARIANT_U32, 0xFF, 0xFF, 0xFF, 0xFF };
      ASSERT_EQ(max_header, max_expected);
   }

   TEST(Program, InitPackHeaderU8ElementU8QueueSize)
   {
      Program header;
      create_program_header(header, apx::ProgramType::Pack, UINT8_SIZE, 10, false);
      Program const expected{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, HEADER_FLAG_QUEUED_DATA | HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT8_SIZE*10+1,
         apx::vm::OPCODE_DATA_SIZE | (VARIANT_ELEMENT_SIZE_U8_QUEUE_SIZE_U8 << INST_VARIANT_SHIFT), UINT8_SIZE};
      ASSERT_EQ(header, expected);
   }

   TEST(Program, InitPackHeaderU8ElementU16QueueSize)
   {
      Program header;
      create_program_header(header, apx::ProgramType::Pack, UINT8_SIZE, 4095, false);
      Program const expected{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, HEADER_FLAG_QUEUED_DATA | HEADER_PROG_TYPE_PACK | VARIANT_U16, 0x01, 0x10, //4095+2
         apx::vm::OPCODE_DATA_SIZE | (VARIANT_ELEMENT_SIZE_U8_QUEUE_SIZE_U16 << INST_VARIANT_SHIFT), UINT8_SIZE };
      ASSERT_EQ(header, expected);
   }


}