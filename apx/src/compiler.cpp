#include "cpp-apx/compiler.h"
#include "cpp-apx/vmdefs.h"
#include "cpp-apx/pack.h"
#include <cassert>
#include <array>
#include <iterator>


namespace apx
{
   extern template void packBE<std::uint8_t>(std::uint8_t* p, std::uint8_t value);
   extern template void packLE<std::uint8_t>(std::uint8_t* p, std::uint8_t value);

   std::unique_ptr<vm::Program> Compiler::compile_port(apx::Port const *port, apx::ProgramType program_type, apx::error_t& error_code)
   {
      reset_internal_state();
      m_program = std::make_unique<vm::Program>();
      std::uint32_t element_size = 0u;
      if (m_program.get() == nullptr)
      {
         set_error(error_code, APX_NULL_PTR_ERROR);
         return std::unique_ptr<vm::Program>();
      }
      else
      {
         if (m_last_error == APX_NO_ERROR)
         {
            auto data_element = port->get_effective_data_element();
            if (data_element != nullptr)
            {
               m_last_error = compile_data_element(data_element, program_type, element_size);
            }
            else
            {
               m_last_error = APX_NULL_PTR_ERROR;
            }
         }
      }
      error_code = m_last_error;
      if (m_last_error != APX_NO_ERROR)
      {
         return std::unique_ptr<vm::Program>();
      }
      std::uint32_t const queue_length = port->get_queue_length();
      apx::vm::Program header;
      m_last_error = apx::vm::create_program_header(header, program_type, element_size, queue_length, m_is_dynamic);
      if (m_last_error != APX_NO_ERROR)
      {
         return std::unique_ptr<vm::Program>();
      }
      m_program->insert(m_program->cbegin(), header.cbegin(), header.cend());
      return std::move(m_program);
   }

   void Compiler::reset_internal_state()
   {
      m_last_error = APX_NO_ERROR;
      m_is_dynamic = false;
   }

   apx::error_t Compiler::compile_data_element(apx::DataElement const* data_element, apx::ProgramType program_type, std::uint32_t& elem_size)
   {
      assert(data_element != nullptr);
      apx::error_t retval = APX_NO_ERROR;
      std::uint8_t data_variant = 0u;
      std::uint32_t array_length = data_element->get_array_length();
      std::uint8_t limit_check_variant = vm::VARIANT_LIMIT_CHECK_NONE;
      bool const is_array = array_length > 0u;
      bool const is_dynamic_array = data_element->is_dynamic_array();
      bool const has_limits = data_element->has_limits();
      bool const is_pack_prog = program_type == apx::ProgramType::Pack ? true : false;
      bool is_signed_type = false;
      bool is_64_bit_type = false;
      bool is_record = false;
      std::uint8_t const opcode = is_pack_prog ? vm::OPCODE_PACK : vm::OPCODE_UNPACK;
      apx::TypeCode type_code = data_element->get_type_code();
      switch (type_code)
      {
      case apx::TypeCode::UInt8:
         data_variant = vm::VARIANT_U8;
         limit_check_variant = vm::VARIANT_LIMIT_CHECK_U8;
         elem_size = vm::UINT8_SIZE;
         break;
      case apx::TypeCode::UInt16:
         data_variant = vm::VARIANT_U16;
         limit_check_variant = vm::VARIANT_LIMIT_CHECK_U16;
         elem_size = vm::UINT16_SIZE;
         break;
      case apx::TypeCode::UInt32:
         data_variant = vm::VARIANT_U32;
         limit_check_variant = vm::VARIANT_LIMIT_CHECK_U32;
         elem_size = vm::UINT32_SIZE;
         break;
      case apx::TypeCode::UInt64:
         data_variant = vm::VARIANT_U64;
         limit_check_variant = vm::VARIANT_LIMIT_CHECK_U64;
         elem_size = vm::UINT64_SIZE;
         is_64_bit_type = true;
         break;
      case apx::TypeCode::Int8:
         data_variant = vm::VARIANT_S8;
         limit_check_variant = vm::VARIANT_LIMIT_CHECK_S8;
         elem_size = vm::INT8_SIZE;
         is_signed_type = true;
         break;
      case apx::TypeCode::Int16:
         data_variant = vm::VARIANT_S16;
         limit_check_variant = vm::VARIANT_LIMIT_CHECK_S16;
         elem_size = vm::INT16_SIZE;
         is_signed_type = true;
         break;
      case apx::TypeCode::Int32:
         data_variant = vm::VARIANT_S32;
         limit_check_variant = vm::VARIANT_LIMIT_CHECK_S32;
         elem_size = vm::INT32_SIZE;
         is_signed_type = true;
         break;
      case apx::TypeCode::Int64:
         data_variant = vm::VARIANT_S64;
         limit_check_variant = vm::VARIANT_LIMIT_CHECK_S64;
         elem_size = vm::INT64_SIZE;
         is_signed_type = true;
         is_64_bit_type = true;
         break;
      case apx::TypeCode::Byte:
         data_variant = vm::VARIANT_BYTE;
         limit_check_variant = vm::VARIANT_LIMIT_CHECK_U8;
         elem_size = vm::BYTE_SIZE;
         break;
      case apx::TypeCode::Char:
         data_variant = vm::VARIANT_ASCII_CHAR;
         elem_size = vm::UINT8_SIZE;
         break;
      case apx::TypeCode::Char8:
         data_variant = vm::VARIANT_CHAR8;
         elem_size = vm::UINT8_SIZE;
         break;
      case apx::TypeCode::Record:
         data_variant = vm::VARIANT_RECORD;
         is_record = true;
         break;
      default:
         retval = APX_ELEMENT_TYPE_ERROR;
         break;
      }

      if (retval == APX_NO_ERROR)
      {
         if (is_record)
         {
            auto const instruction = vm::encode_instruction(opcode, data_variant, is_array);
            m_program->push_back(instruction);
            if (is_array)
            {
               retval = compile_array_size_instruction(array_length, is_dynamic_array);
               if (retval != APX_NO_ERROR)
               {
                  return retval;
               }
               if (is_dynamic_array)
               {
                  m_is_dynamic = true;
               }
            }
            retval = compile_record_fields(data_element, program_type, elem_size);
            if ((retval == APX_NO_ERROR) && (is_array))
            {
               retval = compile_array_next_instruction();
            }
         }
         else
         {
            if (has_limits && is_pack_prog)
            {
               retval = compile_limit_instruction(data_element, is_signed_type, is_64_bit_type, is_array, limit_check_variant);
               if (retval != APX_NO_ERROR)
               {
                  return retval;
               }
            }
            auto const instruction = vm::encode_instruction(opcode, data_variant, is_array);
            m_program->push_back(instruction);
            if (is_array)
            {
               retval = compile_array_size_instruction(array_length, is_dynamic_array);
               if (retval != APX_NO_ERROR)
               {
                  return retval;
               }
               if (is_dynamic_array)
               {
                  m_is_dynamic = true;
               }
            }
            if (has_limits && !is_pack_prog)
            {
               retval = compile_limit_instruction(data_element, is_signed_type, is_64_bit_type, is_array, limit_check_variant);
               if (retval != APX_NO_ERROR)
               {
                  return retval;
               }
            }
         }
      }
      if (retval == APX_NO_ERROR)
      {
         if (elem_size > 0u)
         {
            if (is_array)
            {
               elem_size *= array_length;
               if (is_dynamic_array)
               {
                  elem_size += static_cast<uint32_t>(vm::size_type_to_size(vm::size_to_size_type(array_length)));
               }
            }
         }
         else
         {
            retval = APX_ELEMENT_TYPE_ERROR;
         }
      }
      return retval;
   }

   apx::error_t Compiler::compile_limit_instruction(apx::DataElement const* data_element, bool is_signed_type, bool is_64_bit_type, bool is_array, std::uint8_t limit_variant)
   {
      if ((limit_variant != vm::VARIANT_LIMIT_CHECK_NONE))
      {
         apx::error_t retval = APX_NO_ERROR;
         std::uint8_t const opcode = vm::OPCODE_DATA_CTRL;
         std::uint8_t instruction_header = vm::encode_instruction(opcode, limit_variant, is_array);
         m_program->push_back(instruction_header);
         if (is_64_bit_type)
         {
            if (is_signed_type)
            {
               auto const limits = data_element->get_limits_i64();
               retval = compile_limit_values(limit_variant, limits.first, limits.second);
            }
            else
            {
               auto const limits = data_element->get_limits_u64();
               retval = compile_limit_values(limit_variant, limits.first, limits.second);
            }
         }
         else
         {
            if (is_signed_type)
            {
               auto const limits = data_element->get_limits_i32();
               retval = compile_limit_values(limit_variant, limits.first, limits.second);
            }
            else
            {
               auto const limits = data_element->get_limits_u32();
               retval = compile_limit_values(limit_variant, limits.first, limits.second);
            }
         }

         return retval;
      }
      return APX_INVALID_ARGUMENT_ERROR;
   }

   apx::error_t Compiler::compile_limit_values(std::uint8_t limit_variant, std::int32_t lower_limit, std::int32_t upper_limit)
   {
      apx::error_t retval = APX_NO_ERROR;
      std::array<std::uint8_t, sizeof(std::int32_t)*2> buf;
      std::uint8_t* p = buf.data();
      std::size_t elem_size;
      assert(sizeof(buf) == 8);
      switch (limit_variant)
      {
      case vm::VARIANT_LIMIT_CHECK_S8:
         elem_size = sizeof(std::int8_t);
         apx::packLE<std::int8_t>(p, static_cast<std::int8_t>(lower_limit));
         p += elem_size;
         apx::packLE<std::int8_t>(p, static_cast<std::int8_t>(upper_limit));
         p += elem_size;
         m_program->insert(m_program->end(), buf.data(), p);
         break;
      case vm::VARIANT_LIMIT_CHECK_S16:
         elem_size = sizeof(std::int16_t);
         apx::packLE<std::int16_t>(p, static_cast<std::int16_t>(lower_limit));
         p += elem_size;
         apx::packLE<std::int16_t>(p, static_cast<std::int16_t>(upper_limit));
         p += elem_size;
         m_program->insert(m_program->end(), buf.data(), p);
         break;
      case vm::VARIANT_LIMIT_CHECK_S32:
         elem_size = sizeof(std::int32_t);
         apx::packLE<std::int32_t>(p, static_cast<std::int32_t>(lower_limit));
         p += elem_size;
         apx::packLE<std::int32_t>(p, static_cast<std::int32_t>(upper_limit));
         p += elem_size;
         m_program->insert(m_program->end(), buf.data(), p);
         break;
      default:
         retval = APX_UNSUPPORTED_ERROR;
      }
      return retval;
   }

   apx::error_t Compiler::compile_limit_values(std::uint8_t limit_variant, std::uint32_t lower_limit, std::uint32_t upper_limit)
   {
      apx::error_t retval = APX_NO_ERROR;
      std::array<std::uint8_t, sizeof(std::uint32_t) * 2> buf;
      std::uint8_t* p = buf.data();
      std::size_t elem_size;
      assert(sizeof(buf) == 8);
      switch (limit_variant)
      {
      case vm::VARIANT_LIMIT_CHECK_U8:
         elem_size = sizeof(std::uint8_t);
         apx::packLE<std::uint8_t>(p, static_cast<std::uint8_t>(lower_limit));
         p += elem_size;
         apx::packLE<std::uint8_t>(p, static_cast<std::uint8_t>(upper_limit));
         p += elem_size;
         m_program->insert(m_program->end(), buf.data(), p);
         break;
      case vm::VARIANT_LIMIT_CHECK_U16:
         elem_size = sizeof(std::uint16_t);
         apx::packLE<std::uint16_t>(p, static_cast<std::int16_t>(lower_limit));
         p += elem_size;
         apx::packLE<std::uint16_t>(p, static_cast<std::int16_t>(upper_limit));
         p += elem_size;
         m_program->insert(m_program->end(), buf.data(), p);
         break;
      case vm::VARIANT_LIMIT_CHECK_U32:
         elem_size = sizeof(std::uint32_t);
         apx::packLE<std::uint32_t>(p, static_cast<std::uint32_t>(lower_limit));
         p += elem_size;
         apx::packLE<std::uint32_t>(p, static_cast<std::uint32_t>(upper_limit));
         p += elem_size;
         m_program->insert(m_program->end(), buf.data(), p);
         break;
      default:
         retval = APX_UNSUPPORTED_ERROR;
      }
      return retval;
   }

   apx::error_t Compiler::compile_limit_values(std::uint8_t limit_variant, std::int64_t lower_limit, std::int64_t upper_limit)
   {
      apx::error_t retval = APX_NO_ERROR;
      std::array<std::uint8_t, sizeof(std::int64_t) * 2> buf;
      std::uint8_t* p = buf.data();
      std::size_t elem_size;
      assert(sizeof(buf) == 16);
      switch (limit_variant)
      {
      case vm::VARIANT_LIMIT_CHECK_S64:
         elem_size = sizeof(std::int64_t);
         apx::packLE<std::int64_t>(p, static_cast<std::int64_t>(lower_limit));
         p += elem_size;
         apx::packLE<std::int64_t>(p, static_cast<std::int64_t>(upper_limit));
         p += elem_size;
         m_program->insert(m_program->end(), buf.data(), p);
         break;
      default:
         retval = APX_UNSUPPORTED_ERROR;
      }
      return retval;
   }

   apx::error_t Compiler::compile_limit_values(std::uint8_t limit_variant, std::uint64_t lower_limit, std::uint64_t upper_limit)
   {
      apx::error_t retval = APX_NO_ERROR;
      std::array<std::uint8_t, sizeof(std::uint64_t) * 2> buf;
      std::uint8_t* p = buf.data();
      std::size_t elem_size;
      assert(sizeof(buf) == 16);
      switch (limit_variant)
      {
      case vm::VARIANT_LIMIT_CHECK_U64:
         elem_size = sizeof(std::uint32_t);
         apx::packLE<std::uint64_t>(p, static_cast<std::uint64_t>(lower_limit));
         p += elem_size;
         apx::packLE<std::uint64_t>(p, static_cast<std::uint64_t>(upper_limit));
         p += elem_size;
         m_program->insert(m_program->end(), buf.data(), p);
         break;
      default:
         retval = APX_UNSUPPORTED_ERROR;
      }
      return retval;
   }

   apx::error_t Compiler::compile_array_size_instruction(std::uint32_t array_size, bool is_dynamic_array)
   {
      std::uint8_t const opcode = vm::OPCODE_DATA_SIZE;
      std::array<std::uint8_t, sizeof(std::uint32_t)> buf;
      std::uint8_t variant;
      std::uint8_t* p = buf.data();
      if (array_size > UINT16_MAX)
      {
         variant = vm::VARIANT_ARRAY_SIZE_U32;
         apx::packLE<std::uint32_t>(p, array_size);
         p += sizeof(std::uint32_t);
      }
      else if (array_size > UINT8_MAX)
      {
         variant = vm::VARIANT_ARRAY_SIZE_U16;
         apx::packLE<std::uint16_t>(p, static_cast<std::uint16_t>(array_size));
         p += sizeof(std::uint16_t);
      }
      else
      {
         variant = vm::VARIANT_ARRAY_SIZE_U8;
         apx::packLE<std::uint8_t>(p, static_cast<std::uint8_t>(array_size));
         p += sizeof(std::uint8_t);
      }
      std::uint8_t instruction_header = vm::encode_instruction(opcode, variant, is_dynamic_array);
      m_program->push_back(instruction_header);
      m_program->insert(m_program->end(), buf.data(), p);
      return APX_NO_ERROR;
   }

   apx::error_t Compiler::compile_record_fields(apx::DataElement const* data_element, apx::ProgramType program_type, std::uint32_t& record_size)
   {
      assert(m_program.get() != nullptr);
      if (data_element->get_type_code() != apx::TypeCode::Record)
      {
         return APX_ELEMENT_TYPE_ERROR;
      }
      std::size_t num_children = data_element->get_num_child_elements();
      if (num_children == 0)
      {
         return APX_EMPTY_RECORD_ERROR;
      }
      for (std::size_t i = 0u; i < num_children; i++)
      {
         std::uint32_t child_size = 0u;
         apx::DataElement const* child_element = data_element->get_child_at(i);
         assert(child_element != nullptr);
         apx::error_t result = compile_record_select_instruction(child_element, (i == (num_children-1))? true : false );
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         auto derived_element = child_element;
         DataElement const* derived_parent = nullptr;
         result = data_element->derive_data_element(derived_element, &derived_parent);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         assert(derived_element != nullptr);
         result = compile_data_element(derived_element, program_type, child_size);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         if (child_size == 0u)
         {
            return APX_LENGTH_ERROR;
         }
         record_size += child_size;
      }
      return APX_NO_ERROR;
   }

   apx::error_t Compiler::compile_record_select_instruction(apx::DataElement const* data_element, bool is_last_field)
   {
      assert(m_program.get() != nullptr);
      std::string const& name = data_element->get_name();
      if (name.size() == 0)
      {
         return APX_NAME_MISSING_ERROR;
      }
      std::uint8_t const instruction = vm::encode_instruction(vm::OPCODE_DATA_CTRL, vm::VARIANT_RECORD_SELECT, is_last_field);
      m_program->push_back(instruction);
      m_program->insert(m_program->end(), name.begin(), name.end());
      m_program->push_back(0u); //null-terminator;
      return APX_NO_ERROR;
   }

   apx::error_t Compiler::compile_array_next_instruction()
   {
      assert(m_program.get() != nullptr);
      std::uint8_t const instruction = vm::encode_instruction(vm::OPCODE_FLOW_CTRL, vm::VARIANT_ARRAY_NEXT, false);
      m_program->push_back(instruction);
      return APX_NO_ERROR;
   }

}