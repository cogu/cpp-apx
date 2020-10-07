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
         retval |= (data_size_variant & HEADER_DATA_VARIANT_MASK);
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

      std::uint8_t const* parse_uint32_by_variant(std::uint8_t const* begin, std::uint8_t const* end, std::uint8_t variant, std::uint32_t& number)
      {
         std::uint8_t const* next = begin;
         std::size_t const unpack_size = variant_to_size_integer(variant);
         if ( (unpack_size == 0) || (next + unpack_size) > end)
         {
            return nullptr;
         }
         switch (variant)
         {
         case VARIANT_U8:
            number = static_cast<std::uint32_t>(unpackLE<std::uint8_t>(next));
            break;
         case VARIANT_U16:
            number = static_cast<std::uint32_t>(unpackLE<std::uint16_t>(next));
            break;
         case VARIANT_U32:
            number = static_cast<std::uint32_t>(unpackLE<std::uint32_t>(next));
            break;
         default:
            return nullptr; //VARIANT ERROR
         }
         return next + unpack_size;
      }

      std::uint8_t const* parse_uint64_by_variant(std::uint8_t const* begin, std::uint8_t const* end, std::uint8_t variant, std::uint64_t& number)
      {
         std::uint8_t const* next = begin;
         std::size_t const unpack_size = variant_to_size_integer(variant);
         if ((unpack_size == 0) || (next + unpack_size) > end)
         {
            return nullptr;
         }
         switch (variant)
         {
         case VARIANT_U8:
            number = static_cast<std::uint64_t>(unpackLE<std::uint8_t>(next));
            break;
         case VARIANT_U16:
            number = static_cast<std::uint64_t>(unpackLE<std::uint16_t>(next));
            break;
         case VARIANT_U32:
            number = static_cast<std::uint64_t>(unpackLE<std::uint32_t>(next));
            break;
         case VARIANT_U64:
            number = static_cast<std::uint64_t>(unpackLE<std::uint64_t>(next));
            break;
         default:
            return nullptr; //VARIANT ERROR
         }
         return next + unpack_size;
      }

      std::uint8_t const* parse_int32_by_variant(std::uint8_t const* begin, std::uint8_t const* end, std::uint8_t variant, std::int32_t& number)
      {
         std::uint8_t const* next = begin;
         std::size_t const unpack_size = variant_to_size_integer(variant);
         if ((unpack_size == 0) || (next + unpack_size) > end)
         {
            return nullptr;
         }
         switch (variant)
         {
         case VARIANT_S8:
            number = static_cast<std::int32_t>(unpackLE<std::int8_t>(next));
            break;
         case VARIANT_S16:
            number = static_cast<std::int32_t>(unpackLE<std::int16_t>(next));
            break;
         case VARIANT_S32:
            number = static_cast<std::int32_t>(unpackLE<std::int32_t>(next));
            break;
         default:
            return nullptr; //VARIANT ERROR
         }
         return next + unpack_size;
      }

      std::uint8_t const* parse_int64_by_variant(std::uint8_t const* begin, std::uint8_t const* end, std::uint8_t variant, std::int64_t& number)
      {
         std::uint8_t const* next = begin;
         std::size_t const unpack_size = variant_to_size_integer(variant);
         if ((unpack_size == 0) || (next + unpack_size) > end)
         {
            return nullptr;
         }
         switch (variant)
         {
         case VARIANT_S8:
            number = static_cast<std::int64_t>(unpackLE<std::int8_t>(next));
            break;
         case VARIANT_S16:
            number = static_cast<std::int64_t>(unpackLE<std::int16_t>(next));
            break;
         case VARIANT_S32:
            number = static_cast<std::int64_t>(unpackLE<std::int32_t>(next));
            break;
         case VARIANT_S64:
            number = static_cast<std::int64_t>(unpackLE<std::int32_t>(next));
            break;
         default:
            return nullptr; //VARIANT ERROR
         }
         return next + unpack_size;
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
            p = encoded_size.data();
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
               return APX_VALUE_TYPE_ERROR;
            }
            assert((p > encoded_size.data()) && ((p - encoded_size.data()) <= sizeof(encoded_size)));
            header.insert(header.end(), encoded_size.data(), p);
         }
         return APX_NO_ERROR;
      }

      apx::error_t decode_program_header(std::uint8_t const* begin, std::uint8_t const* end, std::uint8_t const*& next, ProgramHeader& header)
      {
         if ((begin != nullptr && end != nullptr) && (begin < end))
         {
            header.element_size = 0u;
            header.is_dynamic_data = false;
            header.queue_length = 0u;
            std::size_t size = (end - begin);
            if (size < FIXED_HEADER_SIZE)
            {
               return APX_LENGTH_ERROR;
            }
            if ((begin[0] != HEADER_MAGIC_NUMBER_0) ||
               (begin[1] != HEADER_MAGIC_NUMBER_1) ||
               (begin[2] != HEADER_MAGIC_NUMBER_2))
            {
               return APX_INVALID_HEADER_ERROR;
            }
            header.major_version = begin[3];
            header.minor_version = begin[4];
            if (header.major_version != MAJOR_VERSION || header.minor_version != MINOR_VERSION)
            {
               return APX_NOT_IMPLEMENTED_ERROR;
            }
            std::uint8_t const data_variant = begin[5] & HEADER_DATA_VARIANT_MASK;
            header.prog_type = ((begin[5] & HEADER_PROG_TYPE_PACK) == HEADER_PROG_TYPE_PACK) ? ProgramType::Pack : ProgramType::Unpack;
            bool const is_queued_data = ((begin[5] & HEADER_FLAG_QUEUED_DATA) == HEADER_FLAG_QUEUED_DATA);
            header.is_dynamic_data = ((begin[5] & HEADER_FLAG_DYNAMIC_DATA) == HEADER_FLAG_DYNAMIC_DATA);
            next = begin + FIXED_HEADER_SIZE;
            if (auto result = parse_uint32_by_variant(next, end, data_variant, header.data_size); (result > next) && (result <= end) )
            {
               next = result;
            }
            else
            {
               return APX_PARSE_ERROR;
            }

            if (is_queued_data)
            {
               if (next >= end)
               {
                  return APX_PARSE_ERROR;
               }
               std::uint8_t opcode;
               std::uint8_t variant;
               bool flag;
               std::uint8_t element_variant;
               std::uint8_t queued_variant;
               decode_instruction(*next, opcode, variant, flag);
               if ((opcode != OPCODE_DATA_SIZE) || (variant < VARIANT_ELEMENT_SIZE_U8_BASE) || (variant > VARIANT_ELEMENT_SIZE_LAST))
               {
                  return APX_PARSE_ERROR;
               }
               next++;
               if (variant < VARIANT_ELEMENT_SIZE_U16_BASE) //variant is between 3..5
               {
                  element_variant = VARIANT_U8;
                  queued_variant = variant - VARIANT_ELEMENT_SIZE_U8_BASE;
               }
               else if (variant < VARIANT_ELEMENT_SIZE_U32_BASE) //variant is between 6..8
               {
                  element_variant = VARIANT_U16;
                  queued_variant = variant - VARIANT_ELEMENT_SIZE_U16_BASE;
               }
               else //variant is between 9..11
               {
                  element_variant = VARIANT_U32;
                  queued_variant = variant - VARIANT_ELEMENT_SIZE_U32_BASE;
               }
               auto const queued_elem_size = variant_to_size_integer(queued_variant);
               if ((queued_elem_size == 0u) || (queued_elem_size > header.data_size))
               {
                  return APX_PARSE_ERROR;
               }
               if (auto result = parse_uint32_by_variant(next, end, element_variant, header.element_size); (result > next) && (result <= end))
               {
                  next = result;
               }
               else
               {
                  return APX_PARSE_ERROR;
               }
               //Calculate element size by subtracting queued_elem_size from header.data_size then dividing by header.element_size
               if (header.element_size == 0)
               {
                  return APX_PARSE_ERROR;
               }
               std::uint32_t const tmp = header.data_size - static_cast<std::uint32_t>(queued_elem_size);
               if (tmp % header.element_size != 0)
               {
                  return APX_INVALID_HEADER_ERROR;
               }
               header.queue_length = tmp / header.element_size;
            }
            return APX_NO_ERROR;
         }
         return APX_INVALID_ARGUMENT_ERROR;
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

      void decode_instruction(std::uint8_t instruction, std::uint8_t& opcode, std::uint8_t& variant, bool& flag)
      {
         opcode = instruction & INST_OPCODE_MASK;
         variant = (instruction >> INST_VARIANT_SHIFT) & INST_VARIANT_MASK;
         flag = (instruction & INST_FLAG) == INST_FLAG? true : false;
      }

      std::size_t variant_to_size_full(std::uint8_t variant)
      {
         std::size_t retval{ 0 };
         switch (variant)
         {
         case VARIANT_U8:
         case VARIANT_BOOL:
         case VARIANT_BYTE:
         case VARIANT_ASCII_CHAR:
         case VARIANT_CHAR8:
            retval = UINT8_SIZE;
            break;
         case VARIANT_U16:
         case VARIANT_CHAR16:
            retval = UINT16_SIZE;
            break;
         case VARIANT_U32:
         case VARIANT_CHAR32:
            retval = UINT32_SIZE;
            break;
         case VARIANT_U64:
            retval = UINT64_SIZE;
            break;
         case VARIANT_S8:
            retval = UINT8_SIZE;
            break;
         case VARIANT_S16:
            retval = UINT16_SIZE;
            break;
         case VARIANT_S32:
            retval = UINT32_SIZE;
            break;
         case VARIANT_S64:
            retval = UINT64_SIZE;
            break;
         }
         return retval;
      }

      std::size_t variant_to_size_integer(std::uint8_t variant)
      {
         std::size_t retval{ 0 };
         switch (variant)
         {
         case VARIANT_U8:
            retval = UINT8_SIZE;
            break;
         case VARIANT_U16:
            retval = UINT16_SIZE;
            break;
         case VARIANT_U32:
            retval = UINT32_SIZE;
            break;
         case VARIANT_U64:
            retval = UINT64_SIZE;
            break;
         case VARIANT_S8:
            retval = INT8_SIZE;
            break;
         case VARIANT_S16:
            retval = INT16_SIZE;
            break;
         case VARIANT_S32:
            retval = UINT32_SIZE;
            break;
         case VARIANT_S64:
            retval = INT64_SIZE;
            break;
         }
         return retval;
      }

      TypeCode variant_to_type_code(std::uint8_t variant)
      {
         TypeCode retval;
         switch (variant)
         {
         case VARIANT_U8:
            retval = TypeCode::UInt8;
            break;
         case VARIANT_U16:
            retval = TypeCode::UInt16;
            break;
         case VARIANT_U32:
            retval = TypeCode::UInt32;
            break;
         case VARIANT_U64:
            retval = TypeCode::UInt64;
            break;
         case VARIANT_S8:
            retval = TypeCode::Int8;
            break;
         case VARIANT_S16:
            retval = TypeCode::Int16;
            break;
         case VARIANT_S32:
            retval = TypeCode::Int32;
            break;
         case VARIANT_S64:
            retval = TypeCode::Int64;
            break;
         case VARIANT_BOOL:
            retval = TypeCode::Bool;
            break;
         case VARIANT_BYTE:
            retval = TypeCode::Byte;
            break;
         case VARIANT_RECORD:
            retval = TypeCode::Record;
            break;
         case VARIANT_ASCII_CHAR:
            retval = TypeCode::Char;
            break;
         case VARIANT_CHAR8:
            retval = TypeCode::Char8;
            break;
         case VARIANT_CHAR16:
            retval = TypeCode::Char16;
            break;
         case VARIANT_CHAR32:
            retval = TypeCode::Char32;
            break;
         default:
            retval = TypeCode::None;
         }
         return retval;
      }
   }
}