#include <cassert>
#include <typeinfo>
#include "cpp-apx/deserializer.h"
#include "cpp-apx/pack.h"
#include "cpp-apx/program.h"

namespace apx
{
   namespace vm
   {
      void Deserializer::State::clear()
      {
         switch (value_type)
         {
         case dtl::ValueType::NoneType:
            break;
         case dtl::ValueType::Scalar:
            sv.reset();
            break;
         case dtl::ValueType::Array:
            av.reset();
            break;
         case dtl::ValueType::Hash:
            hv.reset();
            break;
         }
         value_type = dtl::ValueType::NoneType;
         field_name.clear();
         index = 0u;
         array_len = 0u;
         max_array_len = 0u;
         type_code = TypeCode::None;
         element_size = 0u;
         is_last_field = false;
         range_check_state = RangeCheckState::NotChecked;
         scalar_type = ScalarStorageType::None;
         dynamic_size_type = SizeType::None;
      }

      void Deserializer::State::init_scalar_value()
      {
         sv = dtl::make_sv();
         value_type = dtl::ValueType::Scalar;
      }

      void Deserializer::State::init_array_value()
      {
         av = dtl::make_av();
         value_type = dtl::ValueType::Array;
      }

      void Deserializer::State::init_record_value()
      {
         hv = dtl::make_hv();
         value_type = dtl::ValueType::Hash;
      }

      apx::error_t Deserializer::State::read_scalar_value(TypeCode type_code_arg)
      {
         if (value_type != dtl::ValueType::Scalar)
         {
            return APX_VALUE_TYPE_ERROR;
         }
         return read_scalar_value(sv.get(), type_code_arg);

      }

      apx::error_t Deserializer::State::read_scalar_value(std::size_t index_arg, TypeCode type_code_arg)
      {
         if (value_type != dtl::ValueType::Array)
         {
            return APX_VALUE_TYPE_ERROR;
         }
         dtl::DynamicValue dv = av->at(index_arg);
         if (dv->dv_type() != dtl::ValueType::Scalar)
         {
            return APX_VALUE_TYPE_ERROR;
         }
         dtl::Scalar* child_sv = dynamic_cast<dtl::Scalar*>(dv.get());
         if (child_sv == nullptr)
         {
            return APX_NULL_PTR_ERROR;
         }
         return read_scalar_value(child_sv, type_code_arg);
      }

      apx::error_t Deserializer::State::read_scalar_value(dtl::Scalar const* sv_arg, TypeCode type_code_arg)
      {
         apx::error_t retval = APX_NO_ERROR;
         bool ok = false;
         assert(sv_arg != 0);
         switch (type_code_arg)
         {
         case TypeCode::UInt8:
         case TypeCode::UInt16:
         case TypeCode::UInt32:
            scalar_value = sv_arg->to_u32(ok);
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
            scalar_value = sv_arg->to_u64(ok);
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
            scalar_value = sv_arg->to_i32(ok);
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
            scalar_value = sv_arg->to_i64(ok);
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
            scalar_value = sv_arg->to_bool(ok);
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

      Deserializer::~Deserializer()
      {
         reset_state();
         if (m_state != nullptr)
         {
            delete m_state;
         }
      }

      dtl::ScalarValue Deserializer::take_sv()
      {
         assert(m_state != nullptr);
         if (m_state->value_type != dtl::ValueType::Scalar)
         {
            throw std::bad_typeid();
         }
         dtl::ScalarValue tmp;
         tmp.swap(m_state->sv);
         return tmp;
      }

      dtl::ArrayValue Deserializer::take_av()
      {
         assert(m_state != nullptr);
         if (m_state->value_type != dtl::ValueType::Array)
         {
            throw std::bad_typeid();
         }
         dtl::ArrayValue tmp;
         tmp.swap(m_state->av);
         return tmp;
      }

      dtl::HashValue Deserializer::take_hv()
      {
         assert(m_state != nullptr);
         if (m_state->value_type != dtl::ValueType::Hash)
         {
            throw std::bad_typeid();
         }
         dtl::HashValue tmp;
         tmp.swap(m_state->hv);
         return tmp;
      }

      void Deserializer::clear_value()
      {
         assert(m_state != nullptr);
         switch (m_state->value_type)
         {
         case dtl::ValueType::NoneType:
            break;
         case dtl::ValueType::Scalar:
            m_state->sv.reset();
            break;
         case dtl::ValueType::Array:
            m_state->av.reset();
            break;
         case dtl::ValueType::Hash:
            m_state->hv.reset();
            break;
         }
         m_state->value_type = dtl::ValueType::NoneType;
      }

      apx::error_t Deserializer::unpack_uint8(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         auto result = prepare_for_buffer_read();
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         reset_state();
         m_state->type_code = TypeCode::UInt8;
         m_state->element_size = UINT8_SIZE;
         if (array_len > 0u)
         {
            result = prepare_for_array(array_len, dynamic_size_type);
            if (result != APX_NO_ERROR)
            {
               return result;
            }
            m_state->init_array_value();
            result = unpack_array_of_scalar();
         }
         else
         {
            m_state->init_scalar_value();
            result = unpack_scalar_value(m_state->sv.get());
         }
         return result;
      }

      apx::error_t Deserializer::unpack_uint16(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         auto result = prepare_for_buffer_read();
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         reset_state();
         m_state->type_code = TypeCode::UInt16;
         m_state->element_size = UINT16_SIZE;
         if (array_len > 0u)
         {
            result = prepare_for_array(array_len, dynamic_size_type);
            if (result != APX_NO_ERROR)
            {
               return result;
            }
            m_state->init_array_value();
            result = APX_NOT_IMPLEMENTED_ERROR;
         }
         else
         {
            m_state->init_scalar_value();
            result = unpack_scalar_value(m_state->sv.get());
         }
         return result;
      }

      apx::error_t Deserializer::unpack_uint32(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         auto result = prepare_for_buffer_read();
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         reset_state();
         m_state->type_code = TypeCode::UInt32;
         m_state->element_size = UINT32_SIZE;
         if (array_len > 0u)
         {
            result = prepare_for_array(array_len, dynamic_size_type);
            if (result != APX_NO_ERROR)
            {
               return result;
            }
            m_state->init_array_value();
            result = APX_NOT_IMPLEMENTED_ERROR;
         }
         else
         {
            m_state->init_scalar_value();
            result = unpack_scalar_value(m_state->sv.get());
         }
         return result;
      }

      apx::error_t Deserializer::unpack_int8(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         auto result = prepare_for_buffer_read();
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         reset_state();
         m_state->type_code = TypeCode::Int8;
         m_state->element_size = INT8_SIZE;
         if (array_len > 0u)
         {
            result = prepare_for_array(array_len, dynamic_size_type);
            if (result != APX_NO_ERROR)
            {
               return result;
            }
            m_state->init_array_value();
            result = APX_NOT_IMPLEMENTED_ERROR;
         }
         else
         {
            m_state->init_scalar_value();
            result = unpack_scalar_value(m_state->sv.get());
         }
         return result;
      }

      apx::error_t Deserializer::unpack_int16(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         auto result = prepare_for_buffer_read();
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         reset_state();
         m_state->type_code = TypeCode::Int16;
         m_state->element_size = INT16_SIZE;
         if (array_len > 0u)
         {
            result = prepare_for_array(array_len, dynamic_size_type);
            if (result != APX_NO_ERROR)
            {
               return result;
            }
            m_state->init_array_value();
            result = APX_NOT_IMPLEMENTED_ERROR;
         }
         else
         {
            m_state->init_scalar_value();
            result = unpack_scalar_value(m_state->sv.get());
         }
         return result;
      }

      apx::error_t Deserializer::unpack_int32(std::size_t array_len, apx::SizeType dynamic_size_type)
      {
         auto result = prepare_for_buffer_read();
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         reset_state();
         m_state->type_code = TypeCode::Int32;
         m_state->element_size = INT32_SIZE;
         if (array_len > 0u)
         {
            result = prepare_for_array(array_len, dynamic_size_type);
            if (result != APX_NO_ERROR)
            {
               return result;
            }
            m_state->init_array_value();
            result = APX_NOT_IMPLEMENTED_ERROR;
         }
         else
         {
            m_state->init_scalar_value();
            result = unpack_scalar_value(m_state->sv.get());
         }
         return result;
      }

      apx::error_t Deserializer::check_value_range_int32(std::int32_t lower_limit, std::int32_t upper_limit)
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
            std::size_t length = m_state->av->length();
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

      apx::error_t Deserializer::check_value_range_uint32(std::uint32_t lower_limit, std::uint32_t upper_limit)
      {
         apx::error_t retval = APX_NO_ERROR;
         if (m_state->value_type == dtl::ValueType::Scalar)
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
            std::size_t length = m_state->av->length();
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

      apx::error_t Deserializer::check_value_range_int64(std::int64_t lower_limit, std::int64_t upper_limit)
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
            std::size_t length = m_state->av->length();
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

      apx::error_t Deserializer::check_value_range_uint64(std::uint64_t lower_limit, std::uint64_t upper_limit)
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
            std::size_t length = m_state->av->length();
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

      void Deserializer::reset_state()
      {
         while (m_stack.size() > 0)
         {
            delete m_state;
            m_state = m_stack.top();
            assert(m_state != nullptr);
            m_stack.pop();
         }
         assert(m_state != nullptr);
         m_state->clear();
      }

      apx::error_t Deserializer::set_read_buffer(std::uint8_t const* buf, std::size_t len)
      {
         if ((buf != nullptr) && (len > 0))
         {
            reset_buffer(buf, len);
            m_state->clear();
            return APX_NO_ERROR;
         }
         return APX_INVALID_ARGUMENT_ERROR;
      }

      void Deserializer::reset_buffer(std::uint8_t const* buf, std::size_t len)
      {
         m_buffer.begin = buf;
         m_buffer.end = buf + len;
         m_buffer.next = buf;
         m_buffer.padded_next = nullptr;
      }

      bool Deserializer::is_valid_buffer()
      {
         return ((m_buffer.next != nullptr) && (m_buffer.end != nullptr) && (m_buffer.next <= m_buffer.end));
      }

      apx::error_t Deserializer::prepare_for_array(std::size_t array_size, apx::SizeType dynamic_size_type)
      {
         if (array_size > 0)
         {
            if (dynamic_size_type != apx::SizeType::None)
            {

               std::size_t length_size = size_type_to_size(dynamic_size_type);
               assert(length_size > 0u);
               m_state->max_array_len = array_size;
               auto result = read_array_size_from_buffer(dynamic_size_type, m_state->array_len);
               if (result != APX_NO_ERROR)
               {
                  return result;
               }
               if (m_state->array_len > m_state->max_array_len)
               {
                  return APX_VALUE_LENGTH_ERROR;
               }
               m_state->dynamic_size_type = dynamic_size_type;
               assert(m_state->element_size != 0);
               m_buffer.padded_next = m_buffer.next + (m_state->max_array_len * m_state->element_size);
            }
            else
            {
               m_state->array_len = array_size;
            }
         }
         return APX_NO_ERROR;
      }

      apx::error_t Deserializer::unpack_scalar_value(dtl::Scalar* sv)
      {
         if ( (m_buffer.next + m_state->element_size) <= m_buffer.end)
         {
            std::int32_t i32;
            std::uint32_t u32;
            std::int64_t i64;
            std::uint64_t u64;
            switch (m_state->type_code)
            {
            case apx::TypeCode::UInt8:
               u32 = static_cast<std::uint32_t>(apx::unpackLE<std::uint8_t>(m_buffer.next));
               sv->set(u32);
               break;
            case apx::TypeCode::UInt16:
               u32 = static_cast<std::uint32_t>(apx::unpackLE<std::uint16_t>(m_buffer.next));
               sv->set(u32);
               break;
            case apx::TypeCode::UInt32:
               u32 = static_cast<std::uint32_t>(apx::unpackLE<std::uint32_t>(m_buffer.next));
               sv->set(u32);
               break;
            case apx::TypeCode::UInt64:
               u64 = static_cast<std::uint64_t>(apx::unpackLE<std::uint64_t>(m_buffer.next));
               sv->set(u64);
               break;
            case apx::TypeCode::Int8:
               i32 = static_cast<std::int32_t>(apx::unpackLE<std::int8_t>(m_buffer.next));
               sv->set(i32);
               break;
            case apx::TypeCode::Int16:
               i32 = static_cast<std::int32_t>(apx::unpackLE<std::int16_t>(m_buffer.next));
               sv->set(i32);
               break;
            case apx::TypeCode::Int32:
               i32 = static_cast<std::int32_t>(apx::unpackLE<std::int32_t>(m_buffer.next));
               sv->set(i32);
               break;
            case apx::TypeCode::Int64:
               i64 = static_cast<std::int64_t>(apx::unpackLE<std::int64_t>(m_buffer.next));
               sv->set(i64);
               break;
            default:
               return APX_UNSUPPORTED_ERROR;
            }
            m_buffer.next += m_state->element_size;
         }
         else
         {
            return APX_BUFFER_BOUNDARY_ERROR;
         }
         return APX_NO_ERROR;
      }

      apx::error_t Deserializer::unpack_array_of_scalar()
      {
         if (m_state->value_type != dtl::ValueType::Array)
         {
            return APX_VALUE_TYPE_ERROR;
         }
         for (std::size_t i = 0; i < m_state->array_len; i++)
         {
            auto sv = dtl::make_sv();
            auto result = unpack_scalar_value(sv.get());
            if (result != APX_NO_ERROR)
            {
               return result;
            }
            m_state->av->push(dtl::dv_cast(sv));
         }
         return APX_NO_ERROR;
      }

      apx::error_t Deserializer::value_in_range_i32(std::int32_t value, std::int32_t lower_limit, std::int32_t upper_limit)
      {
         if (((lower_limit > INT32_MIN) && (value < lower_limit)) ||
            ((upper_limit < INT32_MAX) && (value > upper_limit)))
         {
            return APX_VALUE_RANGE_ERROR;
         }
         return APX_NO_ERROR;
      }

      apx::error_t Deserializer::value_in_range_u32(std::uint32_t value, std::uint32_t lower_limit, std::uint32_t upper_limit)
      {
         if (((lower_limit > 0u) && (value < lower_limit)) ||
            ((upper_limit < UINT32_MAX) && (value > upper_limit)))
         {
            return APX_VALUE_RANGE_ERROR;
         }
         return APX_NO_ERROR;
      }

      apx::error_t Deserializer::value_in_range_i64(std::int64_t value, std::int64_t lower_limit, std::int64_t upper_limit)
      {
         if (((lower_limit > INT64_MIN) && (value < lower_limit)) ||
            ((upper_limit < INT64_MAX) && (value > upper_limit)))
         {
            return APX_VALUE_RANGE_ERROR;
         }
         return APX_NO_ERROR;
      }

      apx::error_t Deserializer::value_in_range_u64(std::uint64_t value, std::uint64_t lower_limit, std::uint64_t upper_limit)
      {
         if (((lower_limit > 0u) && (value < lower_limit)) ||
            ((upper_limit < UINT64_MAX) && (value > upper_limit)))
         {
            return APX_VALUE_RANGE_ERROR;
         }
         return APX_NO_ERROR;
      }

      /*
      * Perform actions that depends on outcome of previous instruction(s)
      */
      apx::error_t Deserializer::prepare_for_buffer_read()
      {
         if (!is_valid_buffer())
         {
            return APX_MISSING_BUFFER_ERROR;
         }
         if (m_buffer.padded_next != nullptr)
         {
            if ((m_buffer.padded_next < m_buffer.begin) || (m_buffer.padded_next > m_buffer.end))
            {
               return APX_BUFFER_BOUNDARY_ERROR;
            }
            m_buffer.next = m_buffer.padded_next;
            m_buffer.padded_next = nullptr;
         }
         return APX_NO_ERROR;
      }

      apx::error_t Deserializer::read_array_size_from_buffer(SizeType size_type, std::size_t& array_size)
      {
         if (size_type == SizeType::None)
         {
            return APX_INVALID_ARGUMENT_ERROR;
         }
         std::uint32_t value{ 0u };
         std::uint8_t const* result = parse_uint32_by_size_type(m_buffer.next, m_buffer.end, size_type, value);
         if ((result > m_buffer.next) && (result <= m_buffer.end))
         {
            array_size = static_cast<std::uint32_t>(value);
            m_buffer.next = result;
         }
         else
         {
            return APX_BUFFER_BOUNDARY_ERROR;
         }
         return APX_NO_ERROR;
      }
   }
}