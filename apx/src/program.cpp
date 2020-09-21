#include <array>
#include <cassert>
#include <iostream> //DEBUG ONLY
#include "cpp-apx/program.h"
#include "cpp-apx/pack.h"

namespace apx
{
   namespace vm
   {
      static std::uint8_t encode_program_byte(apx::ProgramType program_type, bool is_dynamic, bool is_queued, std::uint8_t data_size_variant)
      {
         std::uint8_t retval = (program_type == apx::ProgramType::Pack)? HEADER_PROG_TYPE_PACK : HEADER_PROG_TYPE_UNPACK;
         retval |= (data_size_variant & HEADER_DATA_SIZE_MASK);
         if (is_dynamic)
         {
            retval |= HEADER_FLAG_DYNAMIC_DATA;
         }
         if (is_queued)
         {
            retval |= HEADER_FLAG_QUEUED_DATA;
         }
         return retval;
      }

      static std::uint8_t calc_data_size_variant(std::uint8_t element_variant, std::uint8_t queue_variant)
      {
         std::uint8_t retval = (element_variant == VARIANT_U8) ? VARIANT_ELEMENT_SIZE_U8_BASE : (element_variant == VARIANT_U16) ? VARIANT_ELEMENT_SIZE_U16_BASE : VARIANT_ELEMENT_SIZE_U32_BASE;
         retval += queue_variant;
         return retval;
      }

      apx::error_t create_program_header(apx::vm::Program& header, apx::ProgramType program_type, std::uint32_t element_size, std::uint32_t queue_size, bool is_dynamic)
      {
         const std::array < std::uint8_t, MAGIC_NUMBER_SIZE + VERSION_SIZE> fixed_data = {
            HEADER_MAGIC_NUMBER_0 , HEADER_MAGIC_NUMBER_1, HEADER_MAGIC_NUMBER_2, MAJOR_VERSION, MINOR_VERSION };
         std::array <std::uint8_t, sizeof(uint32_t)> encoded_size = { 0u, 0u, 0u, 0u };
         std::uint8_t* p = encoded_size.data();
         std::uint8_t data_size_variant = 0u;
         std::uint32_t data_size = 0u;
         std::uint8_t queue_variant = 0u; //Only used when is_queued is true
         std::uint8_t element_variant = 0u; //Only used when is_queued is true
         bool const is_queued = queue_size > 0u;

         header.insert(header.end(), fixed_data.begin(), fixed_data.end());
         if (is_queued)
         {
            queue_variant = (queue_size <= UINT8_MAX) ? VARIANT_U8 : (queue_size <= UINT16_MAX) ? VARIANT_U16 : VARIANT_U32;
            element_variant = (element_size <= UINT8_MAX)? VARIANT_U8 : (element_size <= UINT16_MAX)? VARIANT_U16 : VARIANT_U32;
            std::uint32_t const queue_length_size = (queue_variant == VARIANT_U8) ? UINT8_SIZE : (queue_variant == VARIANT_U16) ? UINT16_SIZE : UINT32_SIZE;
            std::uint64_t const tmp = queue_length_size + (static_cast<std::uint64_t>(element_size) * queue_size);
            if (tmp > UINT32_MAX)
            {
               return APX_LENGTH_ERROR;
            }
            data_size = static_cast<std::uint32_t>(tmp);
         }
         else
         {
            data_size = element_size;
         }
         if (data_size <= UINT8_MAX)
         {
            data_size_variant = VARIANT_U8;
            apx::packLE<std::uint8_t>(p, static_cast<std::uint8_t>(data_size));
            p += UINT8_SIZE;
         }
         else if (data_size <= UINT16_MAX)
         {
            data_size_variant = VARIANT_U16;
            apx::packLE<std::uint16_t>(p, static_cast<std::uint16_t>(data_size));
            p += UINT16_SIZE;
         }
         else
         {
            data_size_variant = VARIANT_U32;
            apx::packLE<std::uint32_t>(p, data_size);
            p += UINT32_SIZE;
         }
         assert((p > encoded_size.data()) && ((p - encoded_size.data()) <= sizeof(encoded_size)));
         std::uint8_t program_byte = encode_program_byte(program_type, is_dynamic, is_queued, data_size_variant);
         header.push_back(program_byte);
         header.insert(header.end(), encoded_size.data(), p);
         if (is_queued)
         {
            std::uint8_t const opcode = OPCODE_DATA_SIZE;
            std::uint8_t const variant = calc_data_size_variant(element_variant, queue_variant);
            std::uint8_t const instruction = encode_instruction(opcode, variant, false);
            header.push_back(instruction);
            std::uint8_t* p = encoded_size.data();
            switch (element_variant)
            {
            case VARIANT_U8:
               apx::packLE<std::uint8_t>(p, static_cast<std::uint8_t>(element_size));
               p += UINT8_SIZE;
               break;
            case VARIANT_U16:
               apx::packLE<std::uint16_t>(p, static_cast<std::uint16_t>(element_size));
               p += UINT16_SIZE;
               break;
            case VARIANT_U32:
               apx::packLE<std::uint32_t>(p, static_cast<std::uint32_t>(element_size));
               p += UINT32_SIZE;
               break;
            default:
               assert(0);
               return APX_VALUE_ERROR;
            }
            assert((p > encoded_size.data()) && ((p - encoded_size.data()) <= sizeof(encoded_size)));
            header.insert(header.end(), encoded_size.data(), p);
         }
         return APX_NO_ERROR;
      }

      apx::error_t decode_program_header(std::uint8_t const* program, ProgramHeader& header)
      {
         return APX_NOT_IMPLEMENTED_ERROR;
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