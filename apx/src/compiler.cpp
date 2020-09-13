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
      m_program = std::make_unique<vm::Program>();
      if (m_program.get() == nullptr)
      {
         set_error(error_code, APX_NULL_PTR_ERROR);
         return std::unique_ptr<vm::Program>();
      }
      else
      {
         m_last_error = init_program_header(port, program_type);
         if (m_last_error == APX_NO_ERROR)
         {
            auto data_element = port->get_const_data_element();
            if (data_element != nullptr)
            {
               m_last_error = compile_data_element(data_element, program_type);
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
      m_last_error = program_finalize();
      if (m_last_error != APX_NO_ERROR)
      {
         return std::unique_ptr<vm::Program>();
      }
      return std::move(m_program);
   }

   void Compiler::reset()
   {
      m_last_error = APX_NO_ERROR;
      m_queue_length = 0u;
      m_queue_size_type = apx::SizeType::None;
      m_is_dynamic = false;
      m_data_offset = 0u;
   }

   apx::error_t Compiler::init_program_header(apx::Port const *port, apx::ProgramType program_type)
   {
      const std::uint8_t program_byte{ program_type == apx::ProgramType::Pack ? vm::HEADER_PROG_TYPE_PACK : vm::HEADER_PROG_TYPE_UNPACK };
      auto result = vm::init_program_header(*m_program, program_byte);
      if (result == APX_NO_ERROR)
      {
         if (auto port_attributes = port->get_attributes(); port_attributes != nullptr)
         {
            if (port_attributes->queue_length > 0u)
            {
               m_queue_length = port_attributes->queue_length;
            }
         }
      }
      if (m_queue_length > 0u)
      {
         if (m_queue_length > UINT16_MAX)
         {
            m_queue_size_type = apx::SizeType::UInt32;
         }
         else if (m_queue_length > UINT8_MAX)
         {
            m_queue_size_type = apx::SizeType::UInt16;
         }
         else
         {
            m_queue_size_type = apx::SizeType::UInt8;
         }
         vm::reserve_elem_size_instruction(*m_program, m_queue_size_type);
      }
      return result;
   }

   apx::error_t Compiler::compile_data_element(apx::DataElement const* data_element, apx::ProgramType program_type)
   {
      assert(data_element != nullptr);
      apx::error_t retval = APX_NO_ERROR;
      std::uint32_t elem_size = 0u;
      std::uint8_t data_variant = 0u;
      std::uint32_t array_length = data_element->get_array_length();
      std::uint8_t limit_check_variant = vm::VARIANT_LIMIT_CHECK_NONE;
      bool const is_array = array_length > 0u;
      bool const is_dynamic_array = data_element->is_dynamic_array();
      bool const has_limits = data_element->has_limits();
      bool const is_pack_prog = program_type == apx::ProgramType::Pack ? true : false;
      bool is_signed_type = false;
      std::uint8_t const opcode = is_pack_prog ? vm::OPCODE_PACK : vm::OPCODE_UNPACK;
      apx::TypeCode const type_code = data_element->get_type_code();
      switch (type_code)
      {
      case apx::TypeCode::UInt8:
         data_variant = vm::VARIANT_U8;
         limit_check_variant = vm::VARIANT_LIMIT_CHECK_U8;
         elem_size = vm::UINT8_SIZE;
         break;
      case apx::TypeCode::Int8:
         data_variant = vm::VARIANT_S8;
         limit_check_variant = vm::VARIANT_LIMIT_CHECK_S8;
         elem_size = vm::INT8_SIZE;
         is_signed_type = true;
         break;
      default:
         retval = APX_ELEMENT_TYPE_ERROR;
         break;
      }
      if (retval == APX_NO_ERROR)
      {
         if (has_limits && is_pack_prog)
         {
            retval = compile_limit_instruction(data_element, is_signed_type, is_array, limit_check_variant);
            if (retval != APX_NO_ERROR)
            {
               return retval;
            }
         }
         std::uint8_t instruction = vm::encode_instruction(opcode, data_variant, is_array);
         m_program->push_back(instruction);
         if (is_array)
         {
            retval = compile_array_size_instruction(array_length, is_dynamic_array);
            if (retval != APX_NO_ERROR)
            {
               return retval;
            }
            elem_size *= array_length;
         }
      }
      if (retval == APX_NO_ERROR)
      {
         if (elem_size > 0u)
         {
            m_data_offset += elem_size;
         }
         else
         {
            retval = APX_ELEMENT_TYPE_ERROR;
         }
      }
      return retval;
   }

   apx::error_t Compiler::compile_limit_instruction(apx::DataElement const* data_element, bool is_signed_type, bool is_array, std::uint8_t limit_variant)
   {
      if ((limit_variant != vm::VARIANT_LIMIT_CHECK_NONE) )
      {
         apx::error_t retval = APX_NO_ERROR;
         if ((limit_variant == vm::VARIANT_LIMIT_CHECK_U64) || (limit_variant == vm::VARIANT_LIMIT_CHECK_S64))
         {
            retval = APX_NOT_IMPLEMENTED_ERROR; //64-bit limits not yet implemented
         }
         else
         {
            std::uint8_t const opcode = vm::OPCODE_DATA_CTRL;
            std::uint8_t instruction_header = vm::encode_instruction(opcode, limit_variant, is_array);
            m_program->push_back(instruction_header);
            if (is_signed_type)
            {
               auto const limits = data_element->get_limits_signed();
               retval = compile_limit_values(limit_variant, limits.first, limits.second);
            }
            else
            {
               auto const limits = data_element->get_limits_unsigned();
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
         retval = APX_ELEMENT_TYPE_ERROR;
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
         retval = APX_ELEMENT_TYPE_ERROR;
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


   apx::error_t Compiler::program_finalize()
   {
      uint8_t* raw_program = m_program->data();
      if (raw_program != nullptr)
      {
         apx::packLE<std::uint32_t>(&raw_program[vm::HEADER_SIZE_OFFSET], m_data_offset);
      }
      else
      {
         return APX_NULL_PTR_ERROR;
      }
      return APX_NO_ERROR;
   }
}