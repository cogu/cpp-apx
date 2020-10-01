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

}