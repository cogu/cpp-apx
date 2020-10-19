#include "pch.h"
#include <array>
#include "cpp-apx/decoder.h"


namespace apx_test
{
   TEST(Decoder, ProgramEndOnEmptyProgram)
   {
      std::array<std::uint8_t, 1> program {0};
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data()), APX_NO_ERROR);
   }

   TEST(Decoder, UnpackUInt8)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_U8 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_UNPACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data()+program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Unpack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::UInt8);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, PackUInt8)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_U8 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_PACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Pack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::UInt8);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, UnpackUInt16)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_U16 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_UNPACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Unpack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::UInt16);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, PackUInt16)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_U16 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_PACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Pack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::UInt16);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, UnpackUInt32)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_U32 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_UNPACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Unpack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::UInt32);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, PackUInt32)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_U32 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_PACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Pack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::UInt32);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, UnpackInt8)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_S8 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_UNPACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Unpack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Int8);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, PackInt8)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_S8 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_PACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Pack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Int8);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, UnpackInt16)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_S16 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_UNPACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Unpack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Int16);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, PackInt16)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_S16 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_PACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Pack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Int16);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, UnpackInt32)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_S32 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_UNPACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Unpack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Int32);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, PackInt32)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_S32 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_PACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Pack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Int32);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, UnpackByte)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_BYTE << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_UNPACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Unpack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Byte);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, PackByte)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_BYTE << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_PACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Pack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Byte);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, UnpackBool)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_BOOL << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_UNPACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Unpack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Bool);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, PackBool)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_BOOL << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_PACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Pack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Bool);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, UnpackChar)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_ASCII_CHAR << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_UNPACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Unpack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Char);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, PackChar)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_ASCII_CHAR << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_PACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Pack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Char);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, UnpackChar8)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_CHAR8 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_UNPACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Unpack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Char8);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, PackChar8)
   {
      std::array<std::uint8_t, 1> program{ (apx::vm::VARIANT_CHAR8 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_PACK };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Pack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Char8);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, UnpackUInt8ArrayWithUInt8Length)
   {
      std::array<std::uint8_t, 3> program{ apx::vm::ARRAY_FLAG | (apx::vm::VARIANT_U8 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_UNPACK,
                                           (apx::vm::VARIANT_ARRAY_SIZE_U8 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_SIZE, 0x12 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Unpack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::UInt8);
      EXPECT_EQ(info.array_length, 0x12u);
      EXPECT_EQ(info.is_dynamic_array, false);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, PackUInt8ArrayWithUInt8Length)
   {
      std::array<std::uint8_t, 3> program{ apx::vm::ARRAY_FLAG | (apx::vm::VARIANT_U8 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_PACK,
                                           (apx::vm::VARIANT_ARRAY_SIZE_U8 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_SIZE, 0x12 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Pack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::UInt8);
      EXPECT_EQ(info.array_length, 0x12u);
      EXPECT_EQ(info.is_dynamic_array, false);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, UnpackUInt8ArrayWithUInt16Length)
   {
      std::array<std::uint8_t, 4> program{ apx::vm::ARRAY_FLAG | (apx::vm::VARIANT_U8 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_UNPACK,
                                           (apx::vm::VARIANT_ARRAY_SIZE_U16 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_SIZE, 0x34, 0x12 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Unpack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::UInt8);
      EXPECT_EQ(info.array_length, 0x1234u);
      EXPECT_EQ(info.is_dynamic_array, false);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, PackUInt8ArrayWithUInt16Length)
   {
      std::array<std::uint8_t, 4> program{ apx::vm::ARRAY_FLAG | (apx::vm::VARIANT_U8 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_PACK,
                                           (apx::vm::VARIANT_ARRAY_SIZE_U16 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_SIZE, 0x34, 0x12 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Pack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::UInt8);
      EXPECT_EQ(info.array_length, 0x1234u);
      EXPECT_EQ(info.is_dynamic_array, false);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, UnpackUInt8ArrayWithUInt32Length)
   {
      std::array<std::uint8_t, 6> program{ apx::vm::ARRAY_FLAG | (apx::vm::VARIANT_U8 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_UNPACK,
                                           (apx::vm::VARIANT_ARRAY_SIZE_U32 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_SIZE, 0x78, 0x56, 0x34, 0x12 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Unpack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::UInt8);
      EXPECT_EQ(info.array_length, 0x12345678u);
      EXPECT_EQ(info.is_dynamic_array, false);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, PackUInt8ArrayWithUInt32Length)
   {
      std::array<std::uint8_t, 6> program{ apx::vm::ARRAY_FLAG | (apx::vm::VARIANT_U8 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_PACK,
                                           (apx::vm::VARIANT_ARRAY_SIZE_U32 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_SIZE, 0x78, 0x56, 0x34, 0x12 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Pack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::UInt8);
      EXPECT_EQ(info.array_length, 0x12345678u);
      EXPECT_EQ(info.is_dynamic_array, false);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, UnpackDynamicByterrayWithUInt8Length)
   {
      std::array<std::uint8_t, 3> program{ apx::vm::ARRAY_FLAG | (apx::vm::VARIANT_BYTE << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_UNPACK,
                                          apx::vm::DYN_ARRAY_FLAG | (apx::vm::VARIANT_ARRAY_SIZE_U8 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_SIZE, 0x12 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Unpack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Byte);
      EXPECT_EQ(info.array_length, 0x12u);
      EXPECT_EQ(info.is_dynamic_array, true);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, PackDynamicByterrayWithUInt8Length)
   {
      std::array<std::uint8_t, 3> program{ apx::vm::ARRAY_FLAG | (apx::vm::VARIANT_BYTE << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_PACK,
                                           apx::vm::DYN_ARRAY_FLAG | (apx::vm::VARIANT_ARRAY_SIZE_U8 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_SIZE, 0x12 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Pack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Byte);
      EXPECT_EQ(info.array_length, 0x12u);
      EXPECT_EQ(info.is_dynamic_array, true);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, PackDynamicByterrayWithUInt16Length)
   {
      std::array<std::uint8_t, 4> program{ apx::vm::ARRAY_FLAG | (apx::vm::VARIANT_BYTE << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_UNPACK,
                                           apx::vm::DYN_ARRAY_FLAG | (apx::vm::VARIANT_ARRAY_SIZE_U16 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_SIZE,
                                           0x34, 0x12 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Unpack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Byte);
      EXPECT_EQ(info.array_length, 0x1234u);
      EXPECT_EQ(info.is_dynamic_array, true);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, PackDynamicByteArrayWithUInt16Length)
   {
      std::array<std::uint8_t, 4> program{ apx::vm::ARRAY_FLAG | (apx::vm::VARIANT_BYTE << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_PACK,
                                           apx::vm::DYN_ARRAY_FLAG | (apx::vm::VARIANT_ARRAY_SIZE_U16 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_SIZE,
                                           0x34, 0x12 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Pack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Byte);
      EXPECT_EQ(info.array_length, 0x1234u);
      EXPECT_EQ(info.is_dynamic_array, true);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, UnpackDynamicByteArrayWithUInt32Length)
   {
      std::array<std::uint8_t, 6> program{ apx::vm::ARRAY_FLAG | (apx::vm::VARIANT_BYTE << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_UNPACK,
                                           apx::vm::DYN_ARRAY_FLAG | (apx::vm::VARIANT_ARRAY_SIZE_U32 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_SIZE,
                                           0x78, 0x56, 0x34, 0x12 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Unpack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Byte);
      EXPECT_EQ(info.array_length, 0x12345678u);
      EXPECT_EQ(info.is_dynamic_array, true);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, PackDynamicByteArrayWithUInt32Length)
   {
      std::array<std::uint8_t, 6> program{ apx::vm::ARRAY_FLAG | (apx::vm::VARIANT_BYTE << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_PACK,
                                           apx::vm::DYN_ARRAY_FLAG | (apx::vm::VARIANT_ARRAY_SIZE_U32 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_SIZE,
                                           0x78, 0x56, 0x34, 0x12 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::Pack);
      auto const& info = decoder.get_pack_unpack_info();
      EXPECT_EQ(info.type_code, apx::TypeCode::Byte);
      EXPECT_EQ(info.array_length, 0x12345678u);
      EXPECT_EQ(info.is_dynamic_array, true);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, LimitCheckUInt8)
   {
      std::array<std::uint8_t, 3> program{ (apx::vm::VARIANT_LIMIT_CHECK_U8 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_CTRL,
                                           0, 7};
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::LimitCheckUInt32);
      auto const& info = decoder.get_limit_check_uint32();
      EXPECT_EQ(info.lower_limit, 0u);
      EXPECT_EQ(info.upper_limit, 7u);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, LimitCheckInt8)
   {
      std::array<std::uint8_t, 3> program{ (apx::vm::VARIANT_LIMIT_CHECK_S8 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_CTRL,
                                           static_cast<std::uint8_t>(-10), 10 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::LimitCheckInt32);
      auto const& info = decoder.get_limit_check_int32();
      EXPECT_EQ(info.lower_limit, -10);
      EXPECT_EQ(info.upper_limit, 10);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, LimitCheckUInt16)
   {
      std::array<std::uint8_t, 5> program{ (apx::vm::VARIANT_LIMIT_CHECK_U16 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_CTRL,
                                           0, 0, 0x34, 0x12 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::LimitCheckUInt32);
      auto const& info = decoder.get_limit_check_uint32();
      EXPECT_EQ(info.lower_limit, 0u);
      EXPECT_EQ(info.upper_limit, 0x1234u);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, LimitCheckInt16)
   {
      std::array<std::uint8_t, 5> program{ (apx::vm::VARIANT_LIMIT_CHECK_S16 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_CTRL,
                                           0xF0, 0xD8, 0x10, 0x27 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::LimitCheckInt32);
      auto const& info = decoder.get_limit_check_int32();
      EXPECT_EQ(info.lower_limit, -10000);
      EXPECT_EQ(info.upper_limit, 10000);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, LimitCheckUInt32)
   {
      std::array<std::uint8_t, 9> program{ (apx::vm::VARIANT_LIMIT_CHECK_U32 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_CTRL,
                                           0, 0, 0, 0, 0x78, 0x56, 0x34, 0x12 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::LimitCheckUInt32);
      auto const& info = decoder.get_limit_check_uint32();
      EXPECT_EQ(info.lower_limit, 0u);
      EXPECT_EQ(info.upper_limit, 0x12345678u);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, LimitCheckInt32)
   {
      std::array<std::uint8_t, 9> program{ (apx::vm::VARIANT_LIMIT_CHECK_S32 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_CTRL,
                                           0xF0, 0xD8, 0xFF, 0xFF, 0x10, 0x27, 0x00, 0x00 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::LimitCheckInt32);
      auto const& info = decoder.get_limit_check_int32();
      EXPECT_EQ(info.lower_limit, -10000);
      EXPECT_EQ(info.upper_limit, 10000);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, LimitCheckUInt64)
   {
      std::array<std::uint8_t, 17> program{ (apx::vm::VARIANT_LIMIT_CHECK_U64 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_CTRL,
                                           0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xDE, 0xBC, 0x9A, 0x78, 0x56, 0x34, 0x12 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::LimitCheckUInt64);
      auto const& info = decoder.get_limit_check_uint64();
      EXPECT_EQ(info.lower_limit, 0u);
      EXPECT_EQ(info.upper_limit, 0x123456789abcdefful);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

   TEST(Decoder, LimitCheckInt64)
   {
      std::array<std::uint8_t, 17> program{ (apx::vm::VARIANT_LIMIT_CHECK_S64 << apx::vm::INST_VARIANT_SHIFT) | apx::vm::OPCODE_DATA_CTRL,
                                           0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
      apx::vm::Decoder decoder;
      EXPECT_EQ(decoder.select_program(program.data(), program.data() + program.size()), APX_NO_ERROR);
      apx::vm::OperationType operation = apx::vm::OperationType::ProgramEnd;
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::LimitCheckInt64);
      auto const& info = decoder.get_limit_check_int64();
      EXPECT_EQ(info.lower_limit, -1);
      EXPECT_EQ(info.upper_limit, 1);
      EXPECT_EQ(decoder.parse_next_operation(operation), APX_NO_ERROR);
      EXPECT_EQ(operation, apx::vm::OperationType::ProgramEnd);
   }

}