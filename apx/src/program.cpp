#include <array>
#include <iostream> //DEBUG ONLY
#include "cpp-apx/program.h"

namespace apx
{
   namespace vm
   {
      apx::error_t init_program_header(apx::vm::Program& program, std::uint8_t prog_type)
      {
         const std::array < std::uint8_t, MAGIC_NUMBER_SIZE + VERSION_SIZE> fixed_data = {
            HEADER_MAGIC_NUMBER_0 , HEADER_MAGIC_NUMBER_1, HEADER_MAGIC_NUMBER_2, MAJOR_VERSION, MINOR_VERSION };
         if ((prog_type == HEADER_PROG_TYPE_UNPACK || prog_type == HEADER_PROG_TYPE_PACK))
         {
            const std::array < std::uint8_t, sizeof(uint32_t)> data_size_placeholder = { 0u, 0u, 0u, 0u };
            program.insert(program.end(), fixed_data.begin(), fixed_data.end());
            program.push_back(prog_type);
            program.insert(program.end(), data_size_placeholder.begin(), data_size_placeholder.end());
         }
         else
         {
            return APX_INVALID_ARGUMENT_ERROR;
         }
         return APX_NO_ERROR;
      }
      apx::error_t encode_program_header(std::uint8_t* program, ProgramHeader const& header)
      {
         return APX_NOT_IMPLEMENTED_ERROR;
      }

      apx::error_t decode_program_header(std::uint8_t const* program, ProgramHeader& header)
      {
         return APX_NOT_IMPLEMENTED_ERROR;
      }
      void reserve_elem_size_instruction(apx::vm::Program& program, apx::SizeType elem_size)
      {
         std::initializer_list<std::uint8_t> const u8_instruction = { OPCODE_DATA_SIZE | (VARIANT_ELEMENT_SIZE_U8 << INST_VARIANT_SHIFT), 0u };
         std::initializer_list<std::uint8_t> const  u16_instruction = { OPCODE_DATA_SIZE | (VARIANT_ELEMENT_SIZE_U16 << INST_VARIANT_SHIFT), 0u, 0u };
         std::initializer_list<std::uint8_t> const  u32_instruction = { OPCODE_DATA_SIZE | (VARIANT_ELEMENT_SIZE_U32 << INST_VARIANT_SHIFT), 0u, 0u, 0u, 0u };
         switch (elem_size)
         {
         case apx::SizeType::None:
            break;
         case apx::SizeType::UInt8:
            program.insert(program.end(), u8_instruction);
            break;
         case apx::SizeType::UInt16:
            program.insert(program.end(), u16_instruction);
            break;
         case apx::SizeType::UInt32:
            program.insert(program.end(), u32_instruction);
            break;
         }
      }
      std::uint8_t encode_instruction(std::uint8_t opcode, std::uint8_t variant, bool flag)
      {
         uint8_t result = (opcode & INST_OPCODE_MASK) | ((variant & INST_VARIANT_MASK) << INST_VARIANT_SHIFT);
         if (flag)
         {
            result |= apx::vm::INST_FLAG;
         }
         return result;
      }

      apx::error_t decode_instruction(std::uint8_t instruction, std::uint8_t& opcode, std::uint8_t& variant, bool& flag)
      {
         opcode = instruction & INST_OPCODE_MASK;
         variant = (instruction >> INST_VARIANT_SHIFT) & INST_VARIANT_MASK;
         flag = (instruction & INST_FLAG) == INST_FLAG? true : false;
         return APX_NO_ERROR;
      }
   }
}