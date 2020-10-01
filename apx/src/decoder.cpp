#include "cpp-apx\decoder.h"

namespace apx
{
   namespace vm
   {
      apx::error_t Decoder::select_program(std::uint8_t const* begin, std::uint8_t const* end)
      {
         if ((begin == nullptr) || (end == nullptr) || (begin > end))
         {
            return APX_INVALID_ARGUMENT_ERROR;
         }
         m_program_next = begin;
         m_program_end = end;
         return APX_NO_ERROR;
      }

      apx::error_t Decoder::parse_next_operation(OperationType& operation_type)
      {
         if ((m_program_next == nullptr) || (m_program_end == nullptr))
         {
            return APX_NULL_PTR_ERROR;
         }
         if (m_program_next == m_program_end)
         {
            operation_type = OperationType::ProgramEnd;
         }
         else
         {
            apx::error_t result = decode_next_instruction_internal();
            if (result == APX_NO_ERROR)
            {
               operation_type = m_operation_type;
            }
            else
            {
               return result;
            }
         }
         return APX_NO_ERROR;
      }

      apx::error_t Decoder::decode_next_instruction_internal()
      {
         std::uint8_t const instruction = *m_program_next++;
         std::uint8_t opcode{ 0u };
         std::uint8_t variant{ 0u };
         bool flag{ false };
         decode_instruction(instruction, opcode, variant, flag);
         switch (opcode)
         {
         case OPCODE_UNPACK:
            m_operation_type = OperationType::Unpack;
            if (variant > VARIANT_TYPE_CODE_LAST)
            {
               return APX_INVALID_INSTRUCTION_ERROR;
            }
            break;
         case OPCODE_PACK:
            m_operation_type = OperationType::Pack;
            if (variant > VARIANT_TYPE_CODE_LAST)
            {
               return APX_INVALID_INSTRUCTION_ERROR;
            }
            break;
         case OPCODE_DATA_CTRL:
            if (variant == VARIANT_RECORD_SELECT)
            {
               m_operation_type = OperationType::RecordSelect;
            }
            else if (variant <= VARIANT_LIMIT_CHECK_LAST)
            {
               switch (variant)
               {
               case VARIANT_LIMIT_CHECK_U8:
               case VARIANT_LIMIT_CHECK_U16:
               case VARIANT_LIMIT_CHECK_U32:
                  m_operation_type = OperationType::LimitCheckUInt32;
                  break;
               case VARIANT_LIMIT_CHECK_U64:
                  m_operation_type = OperationType::LimitCheckUInt64;
                  break;
               case VARIANT_LIMIT_CHECK_S8:
               case VARIANT_LIMIT_CHECK_S16:
               case VARIANT_LIMIT_CHECK_S32:
                  m_operation_type = OperationType::LimitCheckInt32;
                  break;
               case VARIANT_LIMIT_CHECK_S64:
                  m_operation_type = OperationType::LimitCheckInt64;
                  break;
               }
            }
            else
            {
               return APX_INVALID_INSTRUCTION_ERROR;
            }
            break;
         case OPCODE_FLOW_CTRL:
            if (variant == VARIANT_ARRAY_NEXT)
            {
               m_operation_type = OperationType::ArrayNext;
            }
            else
            {
               return APX_INVALID_INSTRUCTION_ERROR;
            }
            break;
         default:
            return APX_INVALID_INSTRUCTION_ERROR;
         }
         if ((opcode == OPCODE_UNPACK) || (opcode == OPCODE_PACK))
         {
            TypeCode type_code = variant_to_type_code(variant);
            if (type_code == TypeCode::None)
            {
               return APX_INVALID_INSTRUCTION_ERROR;
            }
            reset_pack_unpack_info(type_code);
            if (flag)
            {
               return decode_array_size();
            }
         }
         else if (opcode == OPCODE_DATA_CTRL)
         {
            switch (variant)
            {
            case VARIANT_RECORD_SELECT:
               break;
            case VARIANT_LIMIT_CHECK_U8:
               return decode_limit_check_uint32(VARIANT_U8);
            case VARIANT_LIMIT_CHECK_U16:
               return decode_limit_check_uint32(VARIANT_U16);
            case VARIANT_LIMIT_CHECK_U32:
               return decode_limit_check_uint32(VARIANT_U32);
            case VARIANT_LIMIT_CHECK_U64:
               return decode_limit_check_uint64(VARIANT_U64);
            case VARIANT_LIMIT_CHECK_S8:
               return decode_limit_check_int32(VARIANT_S8);
            case VARIANT_LIMIT_CHECK_S16:
               return decode_limit_check_int32(VARIANT_S16);
            case VARIANT_LIMIT_CHECK_S32:
               return decode_limit_check_int32(VARIANT_S32);
            case VARIANT_LIMIT_CHECK_S64:
               return decode_limit_check_int64(VARIANT_S64);
            }
         }
         return APX_NO_ERROR;
      }

      void Decoder::reset_pack_unpack_info(apx::TypeCode type_code)
      {
         m_pack_unpack_info.array_size = 0u;
         m_pack_unpack_info.is_dynamic_array = false;
         m_pack_unpack_info.type_code = type_code;
      }

      apx::error_t Decoder::decode_array_size()
      {
         if (m_program_next < m_program_end)
         {
            std::uint8_t const instruction = *m_program_next++;
            std::uint8_t opcode{ 0u };
            std::uint8_t variant{ 0u };
            decode_instruction(instruction, opcode, variant, m_pack_unpack_info.is_dynamic_array);
            if (opcode == OPCODE_DATA_SIZE)
            {
               if (variant > VARIANT_ARRAY_SIZE_LAST)
               {
                  return APX_INVALID_INSTRUCTION_ERROR;
               }
               std::uint8_t const* result = parse_uint32_by_variant(m_program_next, m_program_end, variant, m_pack_unpack_info.array_size);
               if ((result > m_program_next) && (m_program_next <= m_program_end))
               {
                  m_program_next = result;
               }
               else
               {
                  APX_INVALID_INSTRUCTION_ERROR;
               }
            }
            return APX_NO_ERROR;
         }
         return APX_UNEXPECTED_END_ERROR;
      }

      apx::error_t Decoder::decode_limit_check_uint32(std::uint8_t variant)
      {
         std::size_t data_size = variant_to_size_integer(variant);
         if (m_program_next + (data_size * 2) <= m_program_end)
         {
            std::uint8_t const* result = parse_uint32_by_variant(m_program_next, m_program_end, variant, m_limit_check_uint32_info.lower_limit);
            if ((result > m_program_next) && (m_program_next <= m_program_end))
            {
               m_program_next = result;
            }
            else
            {
               APX_INVALID_INSTRUCTION_ERROR;
            }
            result = parse_uint32_by_variant(m_program_next, m_program_end, variant, m_limit_check_uint32_info.upper_limit);
            if ((result > m_program_next) && (m_program_next <= m_program_end))
            {
               m_program_next = result;
            }
            else
            {
               APX_INVALID_INSTRUCTION_ERROR;
            }
            return APX_NO_ERROR;
         }
         return APX_UNEXPECTED_END_ERROR;
      }

      apx::error_t Decoder::decode_limit_check_uint64(std::uint8_t variant)
      {
         std::size_t data_size = variant_to_size_integer(variant);
         if (m_program_next + (data_size * 2) <= m_program_end)
         {
            std::uint8_t const* result = parse_uint64_by_variant(m_program_next, m_program_end, variant, m_limit_check_uint64_info.lower_limit);
            if ((result > m_program_next) && (m_program_next <= m_program_end))
            {
               m_program_next = result;
            }
            else
            {
               APX_INVALID_INSTRUCTION_ERROR;
            }
            result = parse_uint64_by_variant(m_program_next, m_program_end, variant, m_limit_check_uint64_info.upper_limit);
            if ((result > m_program_next) && (m_program_next <= m_program_end))
            {
               m_program_next = result;
            }
            else
            {
               APX_INVALID_INSTRUCTION_ERROR;
            }
            return APX_NO_ERROR;
         }
         return APX_UNEXPECTED_END_ERROR;
      }

      apx::error_t Decoder::decode_limit_check_int32(std::uint8_t variant)
      {
         std::size_t data_size = variant_to_size_integer(variant);
         if (m_program_next + (data_size * 2) <= m_program_end)
         {
            std::uint8_t const* result = parse_int32_by_variant(m_program_next, m_program_end, variant, m_limit_check_int32_info.lower_limit);
            if ((result > m_program_next) && (m_program_next <= m_program_end))
            {
               m_program_next = result;
            }
            else
            {
               APX_INVALID_INSTRUCTION_ERROR;
            }
            result = parse_int32_by_variant(m_program_next, m_program_end, variant, m_limit_check_int32_info.upper_limit);
            if ((result > m_program_next) && (m_program_next <= m_program_end))
            {
               m_program_next = result;
            }
            else
            {
               APX_INVALID_INSTRUCTION_ERROR;
            }
            return APX_NO_ERROR;
         }
         return APX_UNEXPECTED_END_ERROR;
      }

      apx::error_t Decoder::decode_limit_check_int64(std::uint8_t variant)
      {
         std::size_t data_size = variant_to_size_integer(variant);
         if (m_program_next + (data_size * 2) <= m_program_end)
         {
            std::uint8_t const* result = parse_int64_by_variant(m_program_next, m_program_end, variant, m_limit_check_int64_info.lower_limit);
            if ((result > m_program_next) && (m_program_next <= m_program_end))
            {
               m_program_next = result;
            }
            else
            {
               APX_INVALID_INSTRUCTION_ERROR;
            }
            result = parse_int64_by_variant(m_program_next, m_program_end, variant, m_limit_check_int64_info.upper_limit);
            if ((result > m_program_next) && (m_program_next <= m_program_end))
            {
               m_program_next = result;
            }
            else
            {
               APX_INVALID_INSTRUCTION_ERROR;
            }
            return APX_NO_ERROR;
         }
         return APX_UNEXPECTED_END_ERROR;
      }
   }
}

