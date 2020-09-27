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

   TEST(Program, DecodeSimpleProgramHeader_U8)
   {
      Program const program{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_PACK | VARIANT_U8, 0xFF };
      ProgramHeader header;
      std::uint8_t const* begin = program.data();
      std::uint8_t const* end = begin + program.size();
      std::uint8_t const* next{ nullptr };
      ASSERT_EQ(decode_program_header(begin, end, next, header), APX_NO_ERROR);
      ASSERT_EQ(next, end);
      ASSERT_EQ(header.major_version, MAJOR_VERSION);
      ASSERT_EQ(header.minor_version, MINOR_VERSION);
      ASSERT_EQ(header.prog_type, apx::ProgramType::Pack);
      ASSERT_FALSE(header.is_dynamic_data);
      ASSERT_EQ(header.queue_length, 0u);
      ASSERT_EQ(header.data_size, 255);
   }

   TEST(Program, DecodeSimpleProgramHeader_U16)
   {
      Program const program{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_PACK | VARIANT_U16, 0xFF, 0xFF };
      ProgramHeader header;
      std::uint8_t const* begin = program.data();
      std::uint8_t const* end = begin + program.size();
      std::uint8_t const* next{ nullptr };
      ASSERT_EQ(decode_program_header(begin, end, next, header), APX_NO_ERROR);
      ASSERT_EQ(next, end);
      ASSERT_EQ(header.major_version, MAJOR_VERSION);
      ASSERT_EQ(header.minor_version, MINOR_VERSION);
      ASSERT_EQ(header.prog_type, apx::ProgramType::Pack);
      ASSERT_FALSE(header.is_dynamic_data);
      ASSERT_EQ(header.queue_length, 0u);
      ASSERT_EQ(header.data_size, 65535);
   }

   TEST(Program, DecodeSimpleProgramHeader_U32)
   {
      Program const program{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, apx::vm::HEADER_PROG_TYPE_PACK | VARIANT_U32, 0xFF, 0xFF, 0xFF, 0xFF };
      ProgramHeader header;
      std::uint8_t const* begin = program.data();
      std::uint8_t const* end = begin + program.size();
      std::uint8_t const* next{ nullptr };
      ASSERT_EQ(decode_program_header(begin, end, next, header), APX_NO_ERROR);
      ASSERT_EQ(next, end);
      ASSERT_EQ(header.major_version, MAJOR_VERSION);
      ASSERT_EQ(header.minor_version, MINOR_VERSION);
      ASSERT_EQ(header.prog_type, apx::ProgramType::Pack);
      ASSERT_FALSE(header.is_dynamic_data);
      ASSERT_EQ(header.queue_length, 0u);
      ASSERT_EQ(header.data_size, 0xFFFFFFFF);
   }

   TEST(Program, DecodeQueuedProgramHeader_E2Q4)
   {
      Program const program{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, HEADER_FLAG_QUEUED_DATA | apx::vm::HEADER_PROG_TYPE_PACK | VARIANT_U8,
         UINT8_SIZE + 2*4 , apx::vm::OPCODE_DATA_SIZE | (VARIANT_ELEMENT_SIZE_U8_QUEUE_SIZE_U8 << INST_VARIANT_SHIFT), 2 };

      ProgramHeader header;
      std::uint8_t const* begin = program.data();
      std::uint8_t const* end = begin + program.size();
      std::uint8_t const* next{ nullptr };
      ASSERT_EQ(decode_program_header(begin, end, next, header), APX_NO_ERROR);
      ASSERT_EQ(next, end);
      ASSERT_EQ(header.major_version, MAJOR_VERSION);
      ASSERT_EQ(header.minor_version, MINOR_VERSION);
      ASSERT_EQ(header.prog_type, apx::ProgramType::Pack);
      ASSERT_FALSE(header.is_dynamic_data);
      ASSERT_EQ(header.data_size, 9);
      ASSERT_EQ(header.element_size, 2);
      ASSERT_EQ(header.queue_length, 4u);
   }

   TEST(Program, DecodeQueuedProgramHeader_E1Q1000)
   {
      Program const program{ 'A', 'P', 'X', apx::vm::MAJOR_VERSION, apx::vm::MINOR_VERSION, HEADER_FLAG_QUEUED_DATA | apx::vm::HEADER_PROG_TYPE_PACK | VARIANT_U16,
         0xEA , 0x03, //2+1000 encoded as little endian
         apx::vm::OPCODE_DATA_SIZE | (VARIANT_ELEMENT_SIZE_U8_QUEUE_SIZE_U16 << INST_VARIANT_SHIFT), 1 };

      ProgramHeader header;
      std::uint8_t const* begin = program.data();
      std::uint8_t const* end = begin + program.size();
      std::uint8_t const* next{ nullptr };
      ASSERT_EQ(decode_program_header(begin, end, next, header), APX_NO_ERROR);
      ASSERT_EQ(next, end);
      ASSERT_EQ(header.major_version, MAJOR_VERSION);
      ASSERT_EQ(header.minor_version, MINOR_VERSION);
      ASSERT_EQ(header.prog_type, apx::ProgramType::Pack);
      ASSERT_FALSE(header.is_dynamic_data);
      ASSERT_EQ(header.data_size, 1002u);
      ASSERT_EQ(header.element_size, 1u);
      ASSERT_EQ(header.queue_length, 1000u);
   }


}