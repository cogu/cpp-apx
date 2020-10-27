#include <cassert>
#include <stdexcept>
#include "cpp-apx/serializer.h"
#include "cpp-apx/pack.h"
#include "cpp-apx/program.h"

namespace apx
{
   namespace vm
   {
      void Serializer::State::reset(dtl::ValueType vt)
      {
         value_type = vt;
         field_name.clear();
         index = 0u;
         array_len = 0u;
         max_array_len = 0u;
         type_code = TypeCode::None;
         element_size = 0u;
         is_last_field = false;
         range_check_state = RangeCheckState::NotChecked;
         scalar_value = static_cast<std::int32_t>(0);
         scalar_type = ScalarStorageType::None;
         dynamic_size_type = apx::SizeType::None;
      }

      void Serializer::State::set_value(dtl::Value const* dv)
      {
         reset(dv->dv_type());
         switch (value_type)
         {
         case dtl::ValueType::NoneType:
            value.dv = nullptr;
            break;
         case dtl::ValueType::Scalar:
            value.sv = dynamic_cast<dtl::Scalar const*>(dv);
            break;
         case dtl::ValueType::Array:
            value.av = dynamic_cast<dtl::Array const*>(dv);
            break;
         case dtl::ValueType::Hash:
            value.hv = dynamic_cast<dtl::Hash const*>(dv);
            break;
         }
      }

      apx::error_t Serializer::State::read_scalar_value(TypeCode type_code_arg)
      {
         if (value_type != dtl::ValueType::Scalar)
         {
            return APX_VALUE_TYPE_ERROR;
         }
         return read_scalar_value(value.sv, type_code_arg);
      }

      apx::error_t Serializer::State::read_scalar_value(std::size_t index_arg, TypeCode type_code_arg)
      {
         if (value_type != dtl::ValueType::Array)
         {
            return APX_VALUE_TYPE_ERROR;
         }
         dtl::DynamicValue dv;
         dv = value.av->at(index_arg);
         if (dv->dv_type() != dtl::ValueType::Scalar)
         {
            return APX_VALUE_TYPE_ERROR;
         }
         dtl::Scalar* sv = dynamic_cast<dtl::Scalar*>(dv.get());
         if (sv == nullptr)
         {
            return APX_NULL_PTR_ERROR;
         }
         return read_scalar_value(sv, type_code_arg);
      }

      dtl::Value const* Serializer::State::get_child_value(const char* key)
      {
         if (value_type == dtl::ValueType::Hash)
         {
            return value.hv->const_get(key);
         }
         return nullptr;
      }

      apx::error_t Serializer::State::determine_array_length_from_value()
      {
         dtl::ScalarType sv_type;
         switch (value_type)
         {
         case dtl::ValueType::Scalar:
            sv_type = value.sv->sv_type();
            if (sv_type == dtl::ScalarType::String)
            {
               auto ok{ false };
               auto tmp = value.sv->to_string(ok);
               if (ok)
               {
                  array_len = tmp.length();
               }
               else
               {
                  return APX_CONNECTION_ERROR;
               }
            }
            else if (sv_type == dtl::ScalarType::ByteArray)
            {
               auto tmp = value.sv->get_byte_array();
               array_len = tmp.size();
            }
            break;
         case dtl::ValueType::Array:
            array_len = value.av->length();
            break;
         case dtl::ValueType::NoneType: //NOT ARRAY-COMPATIBLE
         case dtl::ValueType::Hash:     //NOT ARRAY-COMPATIBLE
         default:
            return APX_VALUE_TYPE_ERROR;
         }
         return APX_NO_ERROR;
      }

      apx::error_t Serializer::State::read_scalar_value(dtl::Scalar const* sv, TypeCode type_code_arg)
      {
         apx::error_t retval = APX_NO_ERROR;
         bool ok = false;
         assert(sv != 0);
         switch (type_code_arg)
         {
         case TypeCode::UInt8:
         case TypeCode::UInt16:
         case TypeCode::UInt32:
            scalar_value = sv->to_u32(ok);
            if (ok)
            {
               scalar_type = ScalarStorageType::UInt32;
            }
            else
            {
               retval = APX_VALUE_CONVERSION_ERROR;
            }
            break;
         case TypeCode::UInt64:
            scalar_value = sv->to_u64(ok);
            if (ok)
            {
               scalar_type = ScalarStorageType::UInt64;
            }
            else
            {
               retval = APX_VALUE_CONVERSION_ERROR;
            }
            break;
         case TypeCode::Int8:
         case TypeCode::Int16:
         case TypeCode::Int32:
            scalar_value = sv->to_i32(ok);
            if (ok)
            {
               scalar_type = ScalarStorageType::Int32;
            }
            else
            {
               retval = APX_VALUE_CONVERSION_ERROR;
            }
            break;
         case TypeCode::Int64:
            scalar_value = sv->to_i64(ok);
            if (ok)
            {
               scalar_type = ScalarStorageType::Int64;
            }
            else
            {
               retval = APX_VALUE_CONVERSION_ERROR;
            }
            break;
         case TypeCode::Bool:
            scalar_value = sv->to_bool(ok);
            if (ok)
            {
               scalar_type = ScalarStorageType::Bool;
            }
            else
            {
               retval = APX_VALUE_CONVERSION_ERROR;
            }
            break;
         default:
            retval = APX_UNSUPPORTED_ERROR;
         }
         return retval;
      }

      Serializer::~Serializer()
      {
         reset();
         if (m_state != nullptr)
         {
            delete m_state;
         }
      }

      void Serializer::reset()
      {
         while (m_stack.size() > 0)
         {
            delete m_state;
            m_state = m_stack.top();
            assert(m_state != nullptr);
            m_stack.pop();
         }
         clear_value();
      }

      apx::error_t Serializer::set_write_buffer(std::uint8_t* buf, std::size_t len)
      {
         if ((buf != nullptr) && (len > 0))
         {
            reset_buffer(buf, len);
            return APX_NO_ERROR;
         }
         return APX_INVALID_ARGUMENT_ERROR;
      }

      apx::error_t Serializer::set_value(dtl::Value const* dv)
      {
         if (dv != nullptr)
         {
            assert(m_state != nullptr);
            m_state->set_value(dv);
            return APX_NO_ERROR;
         }
         return APX_INVALID_ARGUMENT_ERROR;
      }

      apx::error_t Serializer::set_value(dtl::ScalarValue sv)
      {
         return set_value(sv.get());
      }

      apx::error_t Serializer::set_value(dtl::ArrayValue av)
      {
         return set_value(av.get());
      }

      apx::error_t Serializer::set_value(dtl::HashValue hv)
      {
         return set_value(hv.get());
      }

      void Serializer::clear_value()
      {
         m_state->value_type = dtl::ValueType::NoneType;
         m_state->value.dv = nullptr;
      }

      apx::error_t Serializer::pack_uint8(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         auto result = prepare_for_buffer_write();
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         m_state->type_code = TypeCode::UInt8;
         m_state->element_size = UINT8_SIZE;
         result = prepare_for_array(array_len, dynamic_size_type);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         return pack_value();
      }

      apx::error_t Serializer::pack_uint16(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         auto result = prepare_for_buffer_write();
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         m_state->type_code = TypeCode::UInt16;
         m_state->element_size = UINT16_SIZE;
         result = prepare_for_array(array_len, dynamic_size_type);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         return pack_value();
      }

      apx::error_t Serializer::pack_uint32(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         auto result = prepare_for_buffer_write();
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         m_state->type_code = TypeCode::UInt32;
         m_state->element_size = UINT32_SIZE;
         result = prepare_for_array(array_len, dynamic_size_type);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         return pack_value();
      }

      apx::error_t Serializer::pack_uint64(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         auto result = prepare_for_buffer_write();
         if (result != APX_NO_ERROR)
         {
            return result;
         }         m_state->type_code = TypeCode::UInt64;
         m_state->element_size = UINT64_SIZE;
         result = prepare_for_array(array_len, dynamic_size_type);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         return pack_value();
      }

      apx::error_t Serializer::pack_int8(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         auto result = prepare_for_buffer_write();
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         m_state->type_code = TypeCode::Int8;
         m_state->element_size = INT8_SIZE;
         result = prepare_for_array(array_len, dynamic_size_type);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         return pack_value();
      }

      apx::error_t Serializer::pack_int16(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         auto result = prepare_for_buffer_write();
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         m_state->type_code = TypeCode::Int16;
         m_state->element_size = INT16_SIZE;
         result = prepare_for_array(array_len, dynamic_size_type);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         return pack_value();
      }

      apx::error_t Serializer::pack_int32(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         auto result = prepare_for_buffer_write();
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         m_state->type_code = TypeCode::Int32;
         m_state->element_size = INT32_SIZE;
         result = prepare_for_array(array_len, dynamic_size_type);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         return pack_value();
      }

      apx::error_t Serializer::pack_int64(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         auto result = prepare_for_buffer_write();
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         m_state->type_code = TypeCode::Int64;
         m_state->element_size = INT64_SIZE;
         result = prepare_for_array(array_len, dynamic_size_type);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         return pack_value();
      }

      apx::error_t Serializer::pack_bool(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         auto result = prepare_for_buffer_write();
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         m_state->type_code = TypeCode::Bool;
         m_state->element_size = UINT8_SIZE;
         result = prepare_for_array(array_len, dynamic_size_type);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         return pack_value();
      }

      apx::error_t Serializer::pack_byte_array(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         if (array_len == 0)
         {
            return APX_INVALID_ARGUMENT_ERROR; //The array must be at least 1 byte
         }
         auto result = prepare_for_buffer_write();
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         m_state->type_code = TypeCode::Byte;
         m_state->element_size = BYTE_SIZE;
         result = prepare_for_array(array_len, dynamic_size_type);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         return pack_value();
      }

      apx::error_t Serializer::pack_char(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         auto result = prepare_for_buffer_write();
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         m_state->type_code = TypeCode::Char;
         m_state->element_size = CHAR_SIZE;
         result = prepare_for_array(array_len, dynamic_size_type);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         return pack_value();
      }

      apx::error_t Serializer::pack_char8(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         auto result = prepare_for_buffer_write();
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         m_state->type_code = TypeCode::Char8;
         m_state->element_size = CHAR8_SIZE;
         result = prepare_for_array(array_len, dynamic_size_type);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         return pack_value();
      }

      apx::error_t Serializer::pack_record(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         auto result = prepare_for_buffer_write();
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         m_state->type_code = TypeCode::Record;
         result = prepare_for_array(array_len, dynamic_size_type);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         return pack_value();


/*
         if ( (array_len == 0) && (dynamic_size_type == apx::SizeType::None) )
         {
            if (m_state->value_type != dtl::ValueType::Hash)
            {
               return APX_VALUE_TYPE_ERROR;
            }
         }
         else
         {
            return APX_NOT_IMPLEMENTED_ERROR;
         }
         return APX_NO_ERROR;
*/
      }

      apx::error_t Serializer::check_value_range_int32(std::int32_t lower_limit, std::int32_t upper_limit)
      {
         apx::error_t retval = APX_NO_ERROR;
         if (m_state->value_type == dtl::ValueType::Scalar)
         {
            retval = m_state->read_scalar_value(TypeCode::Int32);
            if (retval == APX_NO_ERROR)
            {
               std::int32_t value = std::get<std::int32_t>(m_state->scalar_value);
               retval = value_in_range_i32(value, lower_limit, upper_limit);
               if (retval == APX_NO_ERROR)
               {
                  m_state->range_check_state = RangeCheckState::CheckOK;
               }
               else
               {
                  m_state->range_check_state = RangeCheckState::CheckFail;
               }
            }
            else
            {
               m_state->range_check_state = RangeCheckState::CheckFail;
            }
         }
         else if (m_state->value_type == dtl::ValueType::Array)
         {
            std::size_t length = m_state->value.av->length();
            m_state->range_check_state = RangeCheckState::CheckOK;
            for (std::size_t i = 0; i < length; i++)
            {
               retval = m_state->read_scalar_value(i, TypeCode::Int32);
               if (retval == APX_NO_ERROR)
               {
                  std::int32_t value = std::get<std::int32_t>(m_state->scalar_value);
                  retval = value_in_range_i32(value, lower_limit, upper_limit);
                  if (retval != APX_NO_ERROR)
                  {
                     m_state->range_check_state = RangeCheckState::CheckFail;
                     break;
                  }
               }
               else
               {
                  m_state->range_check_state = RangeCheckState::CheckFail;
                  break;
               }
            }
         }
         else
         {
            retval = APX_UNSUPPORTED_ERROR;
         }
         return retval;
      }

      apx::error_t Serializer::check_value_range_uint32(std::uint32_t lower_limit, std::uint32_t upper_limit)
      {
         apx::error_t retval = APX_NO_ERROR;
         if ( m_state->value_type == dtl::ValueType::Scalar )
         {
            retval = m_state->read_scalar_value(TypeCode::UInt32);
            if (retval == APX_NO_ERROR)
            {
               std::uint32_t value = std::get<std::uint32_t>(m_state->scalar_value);
               retval = value_in_range_u32(value, lower_limit, upper_limit);
               if (retval == APX_NO_ERROR)
               {
                  m_state->range_check_state = RangeCheckState::CheckOK;
               }
               else
               {
                  m_state->range_check_state = RangeCheckState::CheckFail;
               }
            }
            else
            {
               m_state->range_check_state = RangeCheckState::CheckFail;
            }
         }
         else if (m_state->value_type == dtl::ValueType::Array)
         {
            std::size_t length = m_state->value.av->length();
            m_state->range_check_state = RangeCheckState::CheckOK;
            for (std::size_t i = 0; i < length; i++)
            {
               retval = m_state->read_scalar_value(i, TypeCode::UInt32);
               if (retval == APX_NO_ERROR)
               {
                  std::uint32_t value = std::get<std::uint32_t>(m_state->scalar_value);
                  retval = value_in_range_u32(value, lower_limit, upper_limit);
                  if (retval != APX_NO_ERROR)
                  {
                     m_state->range_check_state = RangeCheckState::CheckFail;
                     break;
                  }
               }
               else
               {
                  m_state->range_check_state = RangeCheckState::CheckFail;
                  break;
               }
            }
         }
         else
         {
            retval = APX_UNSUPPORTED_ERROR;
         }
         return retval;
      }

      apx::error_t Serializer::check_value_range_int64(std::int64_t lower_limit, std::int64_t upper_limit)
      {
         apx::error_t retval = APX_NO_ERROR;
         TypeCode const type_code = TypeCode::Int64;
         if (m_state->value_type == dtl::ValueType::Scalar)
         {
            retval = m_state->read_scalar_value(type_code);
            if (retval == APX_NO_ERROR)
            {
               std::int64_t value = std::get<std::int64_t>(m_state->scalar_value);
               retval = value_in_range_i64(value, lower_limit, upper_limit);
               if (retval == APX_NO_ERROR)
               {
                  m_state->range_check_state = RangeCheckState::CheckOK;
               }
               else
               {
                  m_state->range_check_state = RangeCheckState::CheckFail;
               }
            }
            else
            {
               m_state->range_check_state = RangeCheckState::CheckFail;
            }
         }
         else if (m_state->value_type == dtl::ValueType::Array)
         {
            std::size_t length = m_state->value.av->length();
            m_state->range_check_state = RangeCheckState::CheckOK;
            for (std::size_t i = 0; i < length; i++)
            {
               retval = m_state->read_scalar_value(i, type_code);
               if (retval == APX_NO_ERROR)
               {
                  std::int64_t value = std::get<std::int64_t>(m_state->scalar_value);
                  retval = value_in_range_i64(value, lower_limit, upper_limit);
                  if (retval != APX_NO_ERROR)
                  {
                     m_state->range_check_state = RangeCheckState::CheckFail;
                     break;
                  }
               }
               else
               {
                  m_state->range_check_state = RangeCheckState::CheckFail;
                  break;
               }
            }
         }
         else
         {
            retval = APX_UNSUPPORTED_ERROR;
         }
         return retval;
      }

      apx::error_t Serializer::check_value_range_uint64(std::uint64_t lower_limit, std::uint64_t upper_limit)
      {
         apx::error_t retval = APX_NO_ERROR;
         TypeCode const type_code = TypeCode::UInt64;
         if (m_state->value_type == dtl::ValueType::Scalar)
         {
            retval = m_state->read_scalar_value(type_code);
            if (retval == APX_NO_ERROR)
            {
               std::uint64_t value = std::get<std::uint64_t>(m_state->scalar_value);
               retval = value_in_range_u64(value, lower_limit, upper_limit);
               if (retval == APX_NO_ERROR)
               {
                  m_state->range_check_state = RangeCheckState::CheckOK;
               }
               else
               {
                  m_state->range_check_state = RangeCheckState::CheckFail;
               }
            }
            else
            {
               m_state->range_check_state = RangeCheckState::CheckFail;
            }
         }
         else if (m_state->value_type == dtl::ValueType::Array)
         {
            std::size_t length = m_state->value.av->length();
            m_state->range_check_state = RangeCheckState::CheckOK;
            for (std::size_t i = 0; i < length; i++)
            {
               retval = m_state->read_scalar_value(i, type_code);
               if (retval == APX_NO_ERROR)
               {
                  std::uint64_t value = std::get<std::uint64_t>(m_state->scalar_value);
                  retval = value_in_range_u64(value, lower_limit, upper_limit);
                  if (retval != APX_NO_ERROR)
                  {
                     m_state->range_check_state = RangeCheckState::CheckFail;
                     break;
                  }
               }
               else
               {
                  m_state->range_check_state = RangeCheckState::CheckFail;
                  break;
               }
            }
         }
         else
         {
            retval = APX_UNSUPPORTED_ERROR;
         }
         return retval;
      }

      apx::error_t Serializer::record_select(const char* key, bool is_last_field)
      {
         if (key != nullptr)
         {
            if (m_state->value_type == dtl::ValueType::Hash)
            {
               auto const* child_value = m_state->get_child_value(key);
               if (child_value == nullptr)
               {
                  return APX_NOT_FOUND_ERROR;
               }
               m_state->set_field_name(key, is_last_field);
               enter_new_child_state();
               m_state->set_value(child_value); //m_state on this line is the newly entered child_state
               return APX_NO_ERROR;
            }
            return APX_VALUE_TYPE_ERROR;
         }
         return APX_INVALID_ARGUMENT_ERROR;
      }

      apx::error_t Serializer::queued_write_begin(std::uint32_t element_size, std::uint32_t max_length, bool clear_queue)
      {
         assert(m_state != nullptr);
         m_queued_write.element_size = element_size;
         m_queued_write.length_ptr = m_buffer.next;
         m_queued_write.max_length = max_length;
         m_queued_write.is_enabled = true;
         m_queued_write.size_type = size_to_size_type(max_length);
         std::size_t const value_size = size_type_to_size(m_queued_write.size_type);
         if (clear_queue)
         {
            m_queued_write.current_length = 0u;
         }
         else
         {
            std::size_t tmp;
            apx::error_t result = read_dynamic_value_from_buffer(m_queued_write.length_ptr,
               m_queued_write.length_ptr + value_size, tmp, value_size);
            if (result != APX_NO_ERROR)
            {
               return result;
            }
            m_queued_write.current_length = (std::uint32_t) tmp;
         }
         m_buffer.next += value_size;
         return APX_NO_ERROR;
      }

      apx::error_t Serializer::queued_write_end()
      {
         if (!m_queued_write.is_enabled)
         {
            return APX_INTERNAL_ERROR;
         }
         m_queued_write.is_enabled = false;
         std::size_t const value_size = size_type_to_size(m_queued_write.size_type);
         apx::error_t retval = write_dynamic_value_to_buffer(m_queued_write.length_ptr,
            m_queued_write.length_ptr + value_size, m_queued_write.current_length, value_size);
         return retval;
      }

      apx::error_t Serializer::array_next(bool& is_last)
      {
         is_last = false;
         if (m_state->value_type == dtl::ValueType::Array)
         {
            if (m_state->array_len > 0)
            {
               if (++m_state->index == m_state->array_len)
               {
                  is_last = true;
               }
               else
               {
                  if (m_state->type_code == apx::TypeCode::Record)
                  {
                     auto tmp = m_state->value.av->at(m_state->index);
                     auto const* child_value = tmp.get();
                     if (child_value == nullptr)
                     {
                        return APX_NULL_PTR_ERROR;
                     }
                     enter_new_child_state();
                     m_state->set_value(child_value); //m_state on this line is the newly entered child_state
                  }
                  else
                  {
                     return APX_NOT_IMPLEMENTED_ERROR;
                  }
               }
            }
            else
            {
               return APX_INTERNAL_ERROR;
            }
         }
         else
         {
            return APX_VALUE_TYPE_ERROR;
         }
         return APX_NO_ERROR;
      }

      void Serializer::reset_buffer(std::uint8_t* buf, std::size_t len)
      {
         m_buffer.begin = buf;
         m_buffer.end = buf + len;
         m_buffer.next = buf;
         m_buffer.padded_next = nullptr;
      }

      bool Serializer::is_valid_buffer()
      {
         return ((m_buffer.next != nullptr) && (m_buffer.end != nullptr) && (m_buffer.next <= m_buffer.end));
      }

      apx::error_t Serializer::prepare_for_array(std::size_t array_size, apx::SizeType dynamic_size_type)
      {
         if (array_size > 0)
         {
            if (dynamic_size_type != apx::SizeType::None)
            {
               std::size_t length_size = size_type_to_size(dynamic_size_type);
               assert(length_size > 0u);
               m_state->max_array_len = array_size;
               auto result = m_state->determine_array_length_from_value();
               if (result != APX_NO_ERROR)
               {
                  return result;
               }
               //on success, the value m_state->array_len has been successfully updated.
               if (m_state->array_len > m_state->max_array_len)
               {
                  return APX_VALUE_LENGTH_ERROR;
               }
               m_state->dynamic_size_type = dynamic_size_type;
               assert(m_state->element_size != 0);
               m_buffer.padded_next = m_buffer.next + length_size + (m_state->max_array_len * m_state->element_size);
               if (m_buffer.padded_next > m_buffer.end)
               {
                  return APX_BUFFER_BOUNDARY_ERROR;
               }
            }
            else
            {
               m_state->array_len = array_size;
            }
         }
         return APX_NO_ERROR;
      }

      apx::error_t Serializer::pack_value()
      {
         apx::error_t retval = APX_NO_ERROR;
         assert(m_state != nullptr);
         auto do_pop_state = true;
         if (m_queued_write.is_enabled)
         {
            if (m_queued_write.current_length >= m_queued_write.max_length)
            {
               return APX_QUEUE_FULL_ERROR;
            }
         }
         if (m_state->array_len == 0)
         {
            if (m_state->dynamic_size_type != apx::SizeType::None)
            {
               //This is a zero-length array. Just write the array-size.
               retval = write_dynamic_value_to_buffer(m_state->array_len, m_state->dynamic_size_type);
            }
            else
            {
               if (m_state->is_scalar_type())
               {
                  retval = pack_scalar_value();
               }
               else if (m_state->is_record_type())
               {
                  retval = pack_record_value(do_pop_state);
               }
               else
               {
                  retval = APX_NOT_IMPLEMENTED_ERROR;
               }
            }
         }
         else
         {
            if (m_state->dynamic_size_type != apx::SizeType::None)
            {
               retval = write_dynamic_value_to_buffer(m_state->array_len, m_state->dynamic_size_type);
            }
            if (retval == APX_NO_ERROR)
            {
               if (m_state->is_scalar_type())
               {
                  retval = pack_array_of_scalar();
               }
               else if (m_state->is_string_type())
               {
                  retval = pack_string();
               }
               else if (m_state->is_bytes_type())
               {
                  retval = pack_byte_array_internal();
               }
               else if (m_state->is_record_type())
               {
                  retval = pack_record_value(do_pop_state);
               }
               else
               {
                  retval = APX_NOT_IMPLEMENTED_ERROR;
               }
            }
         }
         if (retval == APX_NO_ERROR)
         {
            if (m_queued_write.is_enabled)
            {
               m_queued_write.current_length++;
            }
            if (do_pop_state)
            {
               pop_state();
            }
         }
         return retval;
      }
      apx::error_t Serializer::pack_scalar_value()
      {
         apx::error_t retval = APX_NO_ERROR;
         if (m_state->value_type != dtl::ValueType::Scalar)
         {
            retval = APX_VALUE_TYPE_ERROR;
         }
         else
         {
            assert(m_state->value.sv != nullptr);
            if (m_state->range_check_state == RangeCheckState::NotChecked)
            {
               retval = default_range_check_value();
            }
            else if (m_state->range_check_state == RangeCheckState::CheckFail)
            {
               retval = APX_VALUE_RANGE_ERROR;
            }
            if (retval == APX_NO_ERROR)
            {
               return pack_scalar_value_internal();
            }
         }
         return retval;
      }

      apx::error_t Serializer::pack_array_of_scalar()
      {
         if (m_state->value_type != dtl::ValueType::Array)
         {
            return APX_VALUE_TYPE_ERROR;
         }
         assert(m_state->value.av != nullptr);
         if ( (m_state->dynamic_size_type == apx::SizeType::None) && (m_state->value.av->length() != m_state->array_len))
         {
            return APX_VALUE_LENGTH_ERROR; //For non-dynamic arrays the array length of the value must match exactly.
         }
         if (m_state->range_check_state == RangeCheckState::NotChecked)
         {
            auto result = default_range_check_value();
            if (result != APX_NO_ERROR)
            {
               return result;
            }
         }
         else if (m_state->range_check_state == RangeCheckState::CheckFail)
         {
            return APX_VALUE_RANGE_ERROR;
         }
         std::size_t length = m_state->value.av->length();
         for (std::size_t i = 0; i < length; i++)
         {
            auto result = m_state->read_scalar_value(i, m_state->type_code);
            if (result != APX_NO_ERROR)
            {
               return result;
            }
            result = pack_scalar_value_internal();
            if (result != APX_NO_ERROR)
            {
               return result;
            }
         }
         return APX_NO_ERROR;
      }

      apx::error_t Serializer::pack_scalar_value_internal()
      {
         apx::error_t retval = APX_NO_ERROR;
         if ((m_buffer.next + m_state->element_size) <= m_buffer.end)
         {
            int32_t i32_val;
            int64_t i64_val;
            uint32_t u32_val;
            uint64_t u64_val;
            bool b_val;
            switch (m_state->type_code)
            {
            case TypeCode::UInt8:
               assert(m_state->scalar_type == ScalarStorageType::UInt32);
               u32_val = std::get<std::uint32_t>(m_state->scalar_value);
               packLE<std::uint8_t>(m_buffer.next, static_cast<std::uint8_t>(u32_val));
               break;
            case TypeCode::UInt16:
               assert(m_state->scalar_type == ScalarStorageType::UInt32);
               u32_val = std::get<std::uint32_t>(m_state->scalar_value);
               packLE<std::uint16_t>(m_buffer.next, static_cast<std::uint16_t>(u32_val));
               break;
            case TypeCode::UInt32:
               assert(m_state->scalar_type == ScalarStorageType::UInt32);
               u32_val = std::get<std::uint32_t>(m_state->scalar_value);
               packLE<std::uint32_t>(m_buffer.next, u32_val);
               break;
            case TypeCode::UInt64:
               assert(m_state->scalar_type == ScalarStorageType::UInt64);
               u64_val = std::get<std::uint64_t>(m_state->scalar_value);
               packLE<std::uint64_t>(m_buffer.next, u64_val);
               break;
            case TypeCode::Int8:
               assert(m_state->scalar_type == ScalarStorageType::Int32);
               i32_val = std::get<std::int32_t>(m_state->scalar_value);
               packLE<std::uint8_t>(m_buffer.next, static_cast<std::int8_t>(i32_val));
               break;
            case TypeCode::Int16:
               assert(m_state->scalar_type == ScalarStorageType::Int32);
               i32_val = std::get<std::int32_t>(m_state->scalar_value);
               packLE<std::uint16_t>(m_buffer.next, static_cast<std::int16_t>(i32_val));
               break;
            case TypeCode::Int32:
               assert(m_state->scalar_type == ScalarStorageType::Int32);
               i32_val = std::get<std::int32_t>(m_state->scalar_value);
               packLE<std::int32_t>(m_buffer.next, i32_val);
               break;
            case TypeCode::Int64:
               assert(m_state->scalar_type == ScalarStorageType::Int64);
               i64_val = std::get<std::int64_t>(m_state->scalar_value);
               packLE<std::int64_t>(m_buffer.next, i64_val);
               break;
            case TypeCode::Bool:
               assert(m_state->scalar_type == ScalarStorageType::Bool);
               b_val = std::get<bool>(m_state->scalar_value);
               packLE<std::uint8_t>(m_buffer.next, b_val? 1u : 0u);
               break;

            default:
               retval = APX_UNSUPPORTED_ERROR;
            }
            m_buffer.next += m_state->element_size;
         }
         else
         {
            retval = APX_BUFFER_BOUNDARY_ERROR;
         }
         return retval;
      }

      apx::error_t Serializer::pack_string()
      {
         assert(m_state != nullptr);
         if (m_state->value_type != dtl::ValueType::Scalar)
         {
            return APX_VALUE_TYPE_ERROR;
         }

         bool ok{ false };
         auto const value = m_state->value.sv->to_string(ok);
         if (!ok)
         {
            return APX_VALUE_CONVERSION_ERROR;
         }
         std::size_t const target_string_size{ m_state->element_size * m_state->array_len };
         apx::error_t retval{ APX_NO_ERROR };
         if ((m_buffer.next + target_string_size) <= m_buffer.end)
         {
            std::memset(m_buffer.next, 0, target_string_size);
            switch (m_state->type_code)
            {
            case TypeCode::Char:
               retval = pack_char_string(value, target_string_size);
               break;
            case TypeCode::Char8:
               // We assume for now that the user has already prepared a valid UTF-8 string.
               ///TODO: check and encode user-string to utf-8 encoding
               retval = pack_char_string(value, target_string_size);
               break;
            default:
               retval = APX_NOT_IMPLEMENTED_ERROR;
            }
            if (retval == APX_NO_ERROR)
            {
               m_buffer.next += target_string_size;
            }
         }
         else
         {
            return APX_BUFFER_BOUNDARY_ERROR;
         }
         return retval;
      }

      apx::error_t Serializer::pack_char_string(std::string const& str, std::size_t max_target_size)
      {
         auto const required_string_size = str.size();
         if (required_string_size > max_target_size)
         {
            return APX_BUFFER_BOUNDARY_ERROR;
         }
         std::memcpy(m_buffer.next, str.data(), required_string_size);
         return APX_NO_ERROR;
      }

      apx::error_t Serializer::pack_byte_array_internal()
      {
         assert(m_state != nullptr);
         if ( (m_state->value_type != dtl::ValueType::Scalar) ||
              (m_state->value.sv->sv_type() != dtl::ScalarType::ByteArray))
         {
            return APX_VALUE_TYPE_ERROR;
         }
         auto const array_len{ m_state->array_len };
         auto const byte_array = m_state->value.sv->get_byte_array();
         if ( (m_state->dynamic_size_type == apx::SizeType::None) && (array_len > 0u) )
         {
            if (array_len != byte_array.size()) //For non-dynamic arrays the length of the value must match exactly.
            {
               return APX_VALUE_LENGTH_ERROR;
            }
         }
         apx::error_t retval{ APX_NO_ERROR };
         if ( (m_buffer.next + array_len) <= m_buffer.end)
         {
            std::memcpy(m_buffer.next, byte_array.data(), array_len);
            m_buffer.next += array_len;
         }
         else
         {
            retval = APX_BUFFER_BOUNDARY_ERROR;
         }
         return retval;
      }

      apx::error_t Serializer::pack_record_value(bool& do_pop_state)
      {
         std::size_t const expected_array_len{ m_state->array_len };
         do_pop_state = false;
         if (expected_array_len > 0)
         {
            if (m_state->value_type == dtl::ValueType::Array)
            {
               if (m_state->dynamic_size_type == apx::SizeType::None)
               {
                  //For non-dynamic arrays the length of the value must match exactly.
                  std::size_t const value_array_length{ m_state->value.av->length() };
                  if (expected_array_len != value_array_length)
                  {
                     return APX_VALUE_LENGTH_ERROR;
                  }
               }
               //select the first array value in a new child state.
               assert(m_state->index == 0u);
               auto tmp = m_state->value.av->at(m_state->index);
               auto const* child_value = tmp.get();
               if (child_value == nullptr)
               {
                  return APX_NULL_PTR_ERROR;
               }
               enter_new_child_state();
               m_state->set_value(child_value); //m_state on this line is the newly entered child_state
            }
            else
            {
               return APX_VALUE_TYPE_ERROR;
            }
         }
         if (m_state->value_type != dtl::ValueType::Hash) //This check applies to both top-level state as well as (potentially newly entered) child-state
         {
            return APX_VALUE_TYPE_ERROR;
         }
         return APX_NO_ERROR;
      }

      apx::error_t Serializer::default_range_check_value()
      {
         apx::error_t retval = APX_NO_ERROR;
         if (m_state->value_type == dtl::ValueType::Scalar)
         {
            retval = m_state->read_scalar_value(m_state->type_code);
            if (retval == APX_NO_ERROR)
            {
               retval = default_range_check_scalar();
            }
         }
         else if (m_state->value_type == dtl::ValueType::Array)
         {
            std::size_t len = m_state->value.av->length();
            for (std::size_t i = 0u; i < len; i++)
            {
               retval = m_state->read_scalar_value(i, m_state->type_code);
               if (retval != APX_NO_ERROR)
               {
                  retval = default_range_check_scalar();
                  if (retval != APX_NO_ERROR)
                  {
                     break;
                  }
               }
            }
         }
         else
         {
            retval = APX_UNSUPPORTED_ERROR;
         }
         return retval;
      }

      apx::error_t apx::vm::Serializer::default_range_check_scalar()
      {
         if (m_state->type_code == TypeCode::Bool)
         {
            return APX_NO_ERROR; //A Bool can fit into any data type, no need to check.
         }
         std::uint32_t u32_lower_limit{ 0u };
         std::uint32_t u32_upper_limit{ 0u };
         std::int32_t i32_lower_limit{ 0 };
         std::int32_t i32_upper_limit{ 0 };
         std::uint64_t u64_lower_limit{ 0u };
         std::uint64_t u64_upper_limit{ 0u };
         std::int64_t i64_lower_limit{ 0 };
         std::int64_t i64_upper_limit{ 0 };
         apx::error_t retval = APX_NO_ERROR;
         ScalarStorageType expected_storage_type = ScalarStorageType::None;

         if (retval == APX_NO_ERROR)
         {
            switch (m_state->type_code)
            {
            case TypeCode::UInt8:
               u32_lower_limit = 0u;
               u32_upper_limit = UINT8_MAX;
               expected_storage_type = ScalarStorageType::UInt32;
               break;
            case TypeCode::UInt16:
               u32_lower_limit = 0u;
               u32_upper_limit = UINT16_MAX;
               expected_storage_type = ScalarStorageType::UInt32;
               break;
            case TypeCode::UInt32:
               u32_lower_limit = 0u;
               u32_upper_limit = UINT32_MAX;
               expected_storage_type = ScalarStorageType::UInt32;
               break;
            case TypeCode::UInt64:
               u64_lower_limit = 0u;
               u64_upper_limit = UINT64_MAX;
               expected_storage_type = ScalarStorageType::UInt64;
               break;
            case TypeCode::Int8:
               i32_lower_limit = INT8_MIN;
               i32_upper_limit = INT8_MAX;
               expected_storage_type = ScalarStorageType::Int32;
               break;
            case TypeCode::Int16:
               i32_lower_limit = INT16_MIN;
               i32_upper_limit = INT16_MAX;
               expected_storage_type = ScalarStorageType::Int32;
               break;
            case TypeCode::Int32:
               i32_lower_limit = INT32_MIN;
               i32_upper_limit = INT32_MAX;
               expected_storage_type = ScalarStorageType::Int32;
               break;
            case TypeCode::Int64:
               i64_lower_limit = INT64_MIN;
               i64_upper_limit = INT64_MAX;
               expected_storage_type = ScalarStorageType::Int64;
               break;
            default:
               retval = APX_UNSUPPORTED_ERROR;
            }
            if ( (retval == APX_NO_ERROR) && (m_state->scalar_type == expected_storage_type) )
            {
               int32_t i32_val;
               uint32_t u32_val;
               int64_t i64_val;
               uint64_t u64_val;
               switch (expected_storage_type)
               {
               case ScalarStorageType::Int32:
                  i32_val = std::get<std::int32_t>(m_state->scalar_value);
                  retval = value_in_range_i32(i32_val, i32_lower_limit, i32_upper_limit);
                  break;
               case ScalarStorageType::UInt32:
                  u32_val = std::get<std::uint32_t>(m_state->scalar_value);
                  retval = value_in_range_u32(u32_val, u32_lower_limit, u32_upper_limit);
                  break;
               case ScalarStorageType::Int64:
                  i64_val = std::get<std::int64_t>(m_state->scalar_value);
                  retval = value_in_range_i64(i64_val, i64_lower_limit, i64_upper_limit);
                  break;
               case ScalarStorageType::UInt64:
                  u64_val = std::get<std::uint64_t>(m_state->scalar_value);
                  retval = value_in_range_u64(u64_val, u64_lower_limit, u64_upper_limit);
                  break;
               default:
                  retval = APX_UNSUPPORTED_ERROR;
               }
            }
            else
            {
               retval = APX_VALUE_TYPE_ERROR;
            }
         }
         return retval;
      }

      apx::error_t Serializer::value_in_range_i32(std::int32_t value, std::int32_t lower_limit, std::int32_t upper_limit)
      {
         if (((lower_limit > INT32_MIN) && (value < lower_limit)) ||
            ((upper_limit < INT32_MAX) && (value > upper_limit)))
         {
            return APX_VALUE_RANGE_ERROR;
         }
         return APX_NO_ERROR;
      }

      apx::error_t Serializer::value_in_range_u32(std::uint32_t value, std::uint32_t lower_limit, std::uint32_t upper_limit)
      {
         if (((lower_limit > 0u) && (value < lower_limit)) ||
            ((upper_limit < UINT32_MAX) && (value > upper_limit)))
         {
            return APX_VALUE_RANGE_ERROR;
         }
         return APX_NO_ERROR;
      }

      apx::error_t Serializer::value_in_range_i64(std::int64_t value, std::int64_t lower_limit, std::int64_t upper_limit)
      {
         if (((lower_limit > INT64_MIN) && (value < lower_limit)) ||
            ((upper_limit < INT64_MAX) && (value > upper_limit)))
         {
            return APX_VALUE_RANGE_ERROR;
         }
         return APX_NO_ERROR;
      }

      apx::error_t Serializer::value_in_range_u64(std::uint64_t value, std::uint64_t lower_limit, std::uint64_t upper_limit)
      {
         if (((lower_limit > 0u) && (value < lower_limit)) ||
            ((upper_limit < UINT64_MAX) && (value > upper_limit)))
         {
            return APX_VALUE_RANGE_ERROR;
         }
         return APX_NO_ERROR;
      }

      void Serializer::pop_state()
      {
         assert(m_state != nullptr);
         while (m_stack.size() > 0)
         {
            delete m_state;
            m_state = m_stack.top();
            assert(m_state != nullptr);
            m_stack.pop();
            if (!m_state->is_last_field)
            {
               break;
            }
         }
      }

      void Serializer::enter_new_child_state()
      {
         auto child_state = new Serializer::State();
         child_state->parent = m_state;
         m_stack.push(m_state);
         m_state = child_state;
      }

      apx::error_t Serializer::write_dynamic_value_to_buffer(std::size_t value, apx::SizeType size_type)
      {
         std::size_t value_size = size_type_to_size(size_type);
         auto retval = write_dynamic_value_to_buffer(m_buffer.next, m_buffer.end, value, value_size);
         if (retval == APX_NO_ERROR)
         {
            m_buffer.next += value_size;
         }
         return retval;
      }

      apx::error_t Serializer::write_dynamic_value_to_buffer(std::uint8_t* begin, std::uint8_t* end, std::size_t value, std::size_t value_size)
      {
         switch (value_size)
         {
         case UINT8_SIZE:
            if (value > static_cast<std::size_t>(UINT8_MAX))
            {
               return APX_LENGTH_ERROR;
            }
            break;
         case UINT16_SIZE:
            if (value > static_cast<std::size_t>(UINT16_MAX))
            {
               return APX_LENGTH_ERROR;
            }
            break;
         case UINT32_SIZE:
            if constexpr (sizeof(std::size_t) > sizeof(std::uint32_t))
            {
               if (value > static_cast<std::size_t>(UINT32_MAX))
               {
                  return APX_LENGTH_ERROR;
               }
            }
            break;
         default:
            return APX_INTERNAL_ERROR;
         }
         if (begin + value_size <= end)
         {
            switch (value_size)
            {
            case UINT8_SIZE:
               packLE<std::uint8_t>(begin, static_cast<std::uint8_t>(value));
               break;
            case UINT16_SIZE:
               packLE<std::uint16_t>(begin, static_cast<std::uint16_t>(value));
               break;
            case UINT32_SIZE:
               packLE<std::uint32_t>(begin, static_cast<std::uint32_t>(value));
               break;
            default:
               return APX_INTERNAL_ERROR;
            }
         }
         else
         {
            return APX_BUFFER_FULL_ERROR;
         }
         return APX_NO_ERROR;
      }

      apx::error_t Serializer::read_dynamic_value_from_buffer(std::uint8_t const* begin, std::uint8_t const* end, std::size_t& value, std::size_t value_size)
      {
         if (begin + value_size <= end)
         {
            switch (value_size)
            {
            case UINT8_SIZE:
               value = static_cast<std::size_t>(unpackLE<std::uint8_t>(begin));
               break;
            case UINT16_SIZE:
               value = static_cast<std::size_t>(unpackLE<std::uint16_t>(begin));
               break;
            case UINT32_SIZE:
               value = static_cast<std::size_t>(unpackLE<std::uint32_t>(begin));
               break;
            default:
               return APX_INTERNAL_ERROR;
            }
         }
         else
         {
            return APX_BUFFER_BOUNDARY_ERROR;
         }
         return APX_NO_ERROR;
      }

      /*
      * If more data follows after a dynamic array write we must move the write pointer to
      * the first byte after the dynamic array. Otherwise elements after the dynamic array
      * will start move around in the memory map.
      */
      apx::error_t Serializer::prepare_for_buffer_write()
      {
         if (!is_valid_buffer())
         {
            return APX_MISSING_BUFFER_ERROR;
         }
         if (m_buffer.padded_next != nullptr)
         {
            if ( (m_buffer.padded_next < m_buffer.begin) || (m_buffer.padded_next > m_buffer.end) )
            {
               return APX_BUFFER_BOUNDARY_ERROR;
            }
            m_buffer.next = m_buffer.padded_next;
            m_buffer.padded_next = nullptr;
         }
         return APX_NO_ERROR;
      }
   }
}