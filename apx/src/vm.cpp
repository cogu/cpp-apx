#include "cpp-apx/vm.h"

namespace apx
{
   apx::error_t VirtualMachine::select_program(apx::vm::Program const& program)
   {
      std::uint8_t const* program_begin = program.data();
      std::uint8_t const* program_end = program_begin + program.size();
      apx::error_t result = m_decoder.select_program(program_begin, program_end);
      if (result == APX_NO_ERROR)
      {
         result = m_decoder.parse_program_header(m_program_header);
      }
      return result;
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

   apx::error_t VirtualMachine::run_pack_program()
   {
      vm::OperationType operation_type = vm::OperationType::ProgramEnd;
      do
      {
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
            result = run_pack_instruction();
            break;
         case vm::OperationType::LimitCheckInt32:
            result = run_range_check_pack_int32();
            break;
         case vm::OperationType::LimitCheckUInt32:
            result = run_range_check_pack_uint32();
            break;
         case vm::OperationType::LimitCheckInt64:
            result = run_range_check_pack_int64();
            break;
         case vm::OperationType::LimitCheckUInt64:
            result = run_range_check_pack_uint64();
            break;
         case vm::OperationType::RecordSelect:
            result = run_pack_record_select();
            break;
         case vm::OperationType::ArrayNext:
            result = run_array_next();
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
         apx::error_t result = m_decoder.parse_next_operation(operation_type);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         switch (operation_type)
         {
         case vm::OperationType::Unpack:
            result = run_unpack_instruction();
            break;
         case vm::OperationType::Pack:
            result = APX_INVALID_INSTRUCTION_ERROR;
            break;
         case vm::OperationType::LimitCheckInt32:
            result = run_range_check_unpack_int32();
            break;
         case vm::OperationType::LimitCheckUInt32:
            result = run_range_check_unpack_uint32();
            break;
         case vm::OperationType::LimitCheckInt64:
            result = run_range_check_unpack_int64();
            break;
         case vm::OperationType::LimitCheckUInt64:
            result = run_range_check_unpack_uint64();
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

   apx::error_t VirtualMachine::run_pack_instruction()
   {
      apx::error_t retval = APX_NOT_IMPLEMENTED_ERROR;
      vm::PackUnpackOperationInfo const &operation = m_decoder.get_pack_unpack_info();
      SizeType const dynamic_size_type = operation.is_dynamic_array? vm::size_to_size_type(operation.array_length) : SizeType::None;
      switch (operation.type_code)
      {
      case TypeCode::UInt8:
         retval = m_serializer.pack_uint8(operation.array_length, dynamic_size_type);
         break;
      case TypeCode::UInt16:
         retval = m_serializer.pack_uint16(operation.array_length, dynamic_size_type);
         break;
      case TypeCode::UInt32:
         retval = m_serializer.pack_uint32(operation.array_length, dynamic_size_type);
         break;
      case TypeCode::UInt64:
         retval = m_serializer.pack_uint64(operation.array_length, dynamic_size_type);
         break;
      case TypeCode::Int8:
         retval = m_serializer.pack_int8(operation.array_length, dynamic_size_type);
         break;
      case TypeCode::Int16:
         retval = m_serializer.pack_int16(operation.array_length, dynamic_size_type);
         break;
      case TypeCode::Int32:
         retval = m_serializer.pack_int32(operation.array_length, dynamic_size_type);
         break;
      case TypeCode::Int64:
         retval = m_serializer.pack_int64(operation.array_length, dynamic_size_type);
         break;
      case TypeCode::Char:
         retval = m_serializer.pack_char(operation.array_length, dynamic_size_type);
         break;
      case TypeCode::Char8:
         retval = m_serializer.pack_char8(operation.array_length, dynamic_size_type);
         break;
      case TypeCode::Record:
         retval = m_serializer.pack_record(operation.array_length, dynamic_size_type);
         if (operation.array_length > 0u)
         {
            m_decoder.save_program_position();
         }
         break;
      }
      return retval;
   }

   apx::error_t VirtualMachine::run_unpack_instruction()
   {
      apx::error_t retval = APX_NOT_IMPLEMENTED_ERROR;
      vm::PackUnpackOperationInfo const& operation = m_decoder.get_pack_unpack_info();
      SizeType const dynamic_size_type = operation.is_dynamic_array ? vm::size_to_size_type(operation.array_length) : SizeType::None;
      switch (operation.type_code)
      {
      case TypeCode::UInt8:
         retval = m_deserializer.unpack_uint8(operation.array_length, dynamic_size_type);
         break;
      }
      return retval;
   }

   apx::error_t VirtualMachine::run_range_check_pack_int32()
   {
      auto const& operation = m_decoder.get_range_check_int32();
      return m_serializer.check_value_range_int32(operation.lower_limit, operation.upper_limit);
   }

   apx::error_t VirtualMachine::run_range_check_pack_uint32()
   {
      auto const& operation = m_decoder.get_range_check_uint32();
      return m_serializer.check_value_range_uint32(operation.lower_limit, operation.upper_limit);
   }

   apx::error_t VirtualMachine::run_range_check_pack_int64()
   {
      auto const& operation = m_decoder.get_range_check_int64();
      return m_serializer.check_value_range_int64(operation.lower_limit, operation.upper_limit);
   }

   apx::error_t VirtualMachine::run_range_check_pack_uint64()
   {
      auto const& operation = m_decoder.get_range_check_uint64();
      return m_serializer.check_value_range_uint64(operation.lower_limit, operation.upper_limit);
   }

   apx::error_t VirtualMachine::run_range_check_unpack_int32()
   {
      auto const& operation = m_decoder.get_range_check_int32();
      return m_deserializer.check_value_range_int32(operation.lower_limit, operation.upper_limit);
   }

   apx::error_t VirtualMachine::run_range_check_unpack_uint32()
   {
      auto const& operation = m_decoder.get_range_check_uint32();
      return m_deserializer.check_value_range_uint32(operation.lower_limit, operation.upper_limit);
   }

   apx::error_t VirtualMachine::run_range_check_unpack_int64()
   {
      auto const& operation = m_decoder.get_range_check_int64();
      return m_deserializer.check_value_range_int64(operation.lower_limit, operation.upper_limit);
   }

   apx::error_t VirtualMachine::run_range_check_unpack_uint64()
   {
      auto const& operation = m_decoder.get_range_check_uint64();
      return m_deserializer.check_value_range_uint64(operation.lower_limit, operation.upper_limit);
   }

   apx::error_t VirtualMachine::run_pack_record_select()
   {
      auto const& field_name = m_decoder.get_field_name();
      return m_serializer.record_select(field_name.c_str(), m_decoder.is_last_field());
   }

   apx::error_t VirtualMachine::run_unpack_record_select()
   {
      auto const& field_name = m_decoder.get_field_name();
      return m_deserializer.record_select(field_name.c_str(), m_decoder.is_last_field());
   }

   apx::error_t VirtualMachine::run_array_next()
   {
      bool is_last_index{ false };
      apx::error_t result = is_pack_prog() ? m_serializer.array_next(is_last_index) : m_deserializer.array_next(is_last_index);
      if (result != APX_NO_ERROR)
      {
         return result;
      }
      if (!is_last_index)
      {
         if (m_decoder.has_saved_program_position())
         {
            m_decoder.recall_program_position();
         }
         else
         {
            return APX_INVALID_INSTRUCTION_ERROR;
         }
      }
      return APX_NO_ERROR;
   }



}
