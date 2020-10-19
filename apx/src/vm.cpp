#include "cpp-apx/vm.h"

namespace apx
{
   apx::error_t VirtualMachine::select_program(apx::vm::Program const& program)
   {
      m_program_begin = m_program_next = program.data();
      m_program_end = m_program_begin + program.size();
      apx::error_t result = parse_program_header();
      if (result != APX_NO_ERROR)
      {
         return result;
      }
      return m_decoder.select_program(m_program_next, m_program_end);
   }

   apx::error_t VirtualMachine::set_write_buffer(std::uint8_t* data, std::size_t size)
   {
      return m_serializer.set_write_buffer(data, size);
   }

   apx::error_t VirtualMachine::set_read_buffer(std::uint8_t const* data, std::size_t size)
   {
      return m_deserializer.set_read_buffer(data, size);
   }

   apx::error_t VirtualMachine::pack_value(dtl::Value const* value)
   {
      if (m_program_header.prog_type != apx::ProgramType::Pack)
      {
         return APX_INVALID_PROGRAM_ERROR;
      }
      m_serializer.set_value(value);
      return run_pack_program();
   }

   apx::error_t VirtualMachine::pack_value(dtl::ScalarValue const& value)
   {
      if (m_program_header.prog_type != apx::ProgramType::Pack)
      {
         return APX_INVALID_PROGRAM_ERROR;
      }
      m_serializer.set_value(value);
      return run_pack_program();
   }

   apx::error_t VirtualMachine::unpack_value(dtl::ScalarValue& value)
   {
      if (m_program_header.prog_type != apx::ProgramType::Unpack)
      {
         return APX_INVALID_PROGRAM_ERROR;
      }
      auto retval = run_unpack_program();
      if (retval == APX_NO_ERROR)
      {
         if (m_deserializer.value_type() == dtl::ValueType::Scalar)
         {
            value = m_deserializer.take_sv();
         }
         else
         {
            retval = APX_VALUE_TYPE_ERROR;
            m_deserializer.clear_value();
         }
      }
      return retval;
   }

   apx::error_t VirtualMachine::parse_program_header()
   {
      return vm::decode_program_header(m_program_begin, m_program_end, m_program_next, m_program_header);
   }

   apx::error_t VirtualMachine::run_pack_program()
   {
      vm::OperationType operation_type = vm::OperationType::ProgramEnd;
      do
      {
         vm::PackUnpackOperationInfo pack_info;
         apx::error_t result = m_decoder.parse_next_operation(operation_type);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         switch(operation_type)
         {
         case vm::OperationType::Unpack:
            return APX_INVALID_INSTRUCTION_ERROR;
         case vm::OperationType::Pack:
            pack_info = m_decoder.get_pack_unpack_info();
            result = run_pack_instruction(pack_info.type_code, pack_info.array_length, pack_info.is_dynamic_array);
            break;
         case vm::OperationType::LimitCheckInt32:
            result = APX_NOT_IMPLEMENTED_ERROR;
            break;
         case vm::OperationType::LimitCheckUInt32:
            result = APX_NOT_IMPLEMENTED_ERROR;
            break;
         case vm::OperationType::LimitCheckInt64:
            result = APX_NOT_IMPLEMENTED_ERROR;
            break;
         case vm::OperationType::LimitCheckUInt64:
            result = APX_NOT_IMPLEMENTED_ERROR;
            break;
         case vm::OperationType::RecordSelect:
            result = APX_NOT_IMPLEMENTED_ERROR;
            break;
         case vm::OperationType::ArrayNext:
            result = APX_NOT_IMPLEMENTED_ERROR;
            break;
         case vm::OperationType::ProgramEnd:
            break;
         }
         if (result != APX_NO_ERROR)
         {
            return result;
         }
      } while (operation_type != vm::OperationType::ProgramEnd);
      return APX_NO_ERROR;
   }



   apx::error_t VirtualMachine::run_unpack_program()
   {
      vm::OperationType operation_type = vm::OperationType::ProgramEnd;
      do
      {
         vm::PackUnpackOperationInfo pack_info;
         apx::error_t result = m_decoder.parse_next_operation(operation_type);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         switch (operation_type)
         {
         case vm::OperationType::Unpack:
            pack_info = m_decoder.get_pack_unpack_info();
            result = run_unpack_instruction(pack_info.type_code, pack_info.array_length, pack_info.is_dynamic_array);
            break;
         case vm::OperationType::Pack:
            result = APX_INVALID_INSTRUCTION_ERROR;
            break;
         case vm::OperationType::LimitCheckInt32:
            result = APX_NOT_IMPLEMENTED_ERROR;
            break;
         case vm::OperationType::LimitCheckUInt32:
            result = APX_NOT_IMPLEMENTED_ERROR;
            break;
         case vm::OperationType::LimitCheckInt64:
            result = APX_NOT_IMPLEMENTED_ERROR;
            break;
         case vm::OperationType::LimitCheckUInt64:
            result = APX_NOT_IMPLEMENTED_ERROR;
            break;
         case vm::OperationType::RecordSelect:
            result = APX_NOT_IMPLEMENTED_ERROR;
            break;
         case vm::OperationType::ArrayNext:
            result = APX_NOT_IMPLEMENTED_ERROR;
            break;
         case vm::OperationType::ProgramEnd:
            break;
         }
         if (result != APX_NO_ERROR)
         {
            return result;
         }
      } while (operation_type != vm::OperationType::ProgramEnd);
      return APX_NO_ERROR;
   }

   apx::error_t VirtualMachine::run_pack_instruction(TypeCode type_code, std::size_t array_length, bool is_dynamic_array)
   {
      apx::error_t retval = APX_NOT_IMPLEMENTED_ERROR;
      SizeType dynamic_size_type = is_dynamic_array? vm::size_to_size_type(array_length) : SizeType::None;
      switch (type_code)
      {
      case TypeCode::UInt8:
         retval = m_serializer.pack_uint8(array_length, dynamic_size_type);
         break;
      case TypeCode::UInt16:
         retval = m_serializer.pack_uint16(array_length, dynamic_size_type);
         break;
      case TypeCode::UInt32:
         retval = m_serializer.pack_uint32(array_length, dynamic_size_type);
         break;
      }
      return retval;
   }

   apx::error_t VirtualMachine::run_unpack_instruction(TypeCode type_code, std::size_t array_length, bool is_dynamic_array)
   {
      apx::error_t retval = APX_NOT_IMPLEMENTED_ERROR;
      SizeType dynamic_size_type = is_dynamic_array ? vm::size_to_size_type(array_length) : SizeType::None;
      switch (type_code)
      {
      case TypeCode::UInt8:
         retval = m_deserializer.unpack_uint8(array_length, dynamic_size_type);
         break;
      }
      return retval;
   }
}
