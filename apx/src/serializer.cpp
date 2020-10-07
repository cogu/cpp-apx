#include "cpp-apx/serializer.h"
#include "cpp-apx/pack.h"
#include <cassert>

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
         assert(scalar_value.index() == 0);
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
         if (m_state != nullptr)
         {
            m_state->reset(dtl::ValueType::NoneType);
         }
         else
         {
            m_state = new State();
         }
         while (m_stack.size() > 0)
         {
            auto state = m_stack.top();
            assert(state != nullptr);
            delete state;
            m_stack.pop();
         }
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

      apx::error_t Serializer::pack_uint8(std::size_t array_len, apx::SizeType dynamic_size)
      {
         if (!is_valid_buffer())
         {
            return APX_MISSING_BUFFER_ERROR;
         }
         m_state->type_code = TypeCode::UInt8;
         m_state->element_size = UINT8_SIZE;
         auto result = prepare_for_array(array_len, dynamic_size);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         return pack_value();
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
               retval = check_value_range_uint32(value, lower_limit, upper_limit);
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
                  retval = check_value_range_uint32(value, lower_limit, upper_limit);
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

      void Serializer::reset_buffer(std::uint8_t* buf, std::size_t len)
      {
         m_buffer.begin = buf;
         m_buffer.end = buf + len;
         m_buffer.next = buf;
         m_buffer.adjusted_next = nullptr;
      }

      bool Serializer::is_valid_buffer()
      {
         return ((m_buffer.next != nullptr) && (m_buffer.end != nullptr) && (m_buffer.next <= m_buffer.end));
      }

      apx::error_t Serializer::prepare_for_array(std::size_t array_size, apx::SizeType dynamic_size)
      {
         if (array_size > 0)
         {
            if (dynamic_size != apx::SizeType::None)
            {
               m_state->max_array_len = array_size;
               return APX_NOT_IMPLEMENTED_ERROR;
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
         if (m_state->array_len == 0)
         {
            retval = pack_scalar_value();
         }
         else if (m_state->array_len > 0)
         {
            if (m_state->is_scalar_type())
            {
               retval = pack_array_of_scalar();
            }
            else if (m_state->is_string_type())
            {
               retval = APX_NOT_IMPLEMENTED_ERROR;
            }
            else if (m_state->is_bytes_type())
            {
               retval = APX_NOT_IMPLEMENTED_ERROR;
            }
         }
         else
         {
            retval = APX_NOT_IMPLEMENTED_ERROR;
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
               retval = default_range_check();
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
         if (m_state->max_array_len > 0)
         {
            return APX_NOT_IMPLEMENTED_ERROR;
         }
         else
         {
            if (m_state->value.av->length() != m_state->array_len)
            {
               return APX_VALUE_LENGTH_ERROR;
            }
         }
         if (m_state->range_check_state == RangeCheckState::NotChecked)
         {
            auto result = default_range_check();
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
            uint32_t u32_val;
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

      apx::error_t Serializer::default_range_check()
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
         std::uint32_t u32_lower_limit = 0u;
         std::uint32_t u32_upper_limit = UINT32_MAX;
         apx::error_t retval = APX_NO_ERROR;
         ScalarStorageType expected_storage_type = ScalarStorageType::None;

         if (retval == APX_NO_ERROR)
         {
            switch (m_state->type_code)
            {
            case TypeCode::UInt8:
               u32_upper_limit = UINT8_MAX;
               expected_storage_type = ScalarStorageType::UInt32;
               break;
            case TypeCode::UInt16:
               u32_upper_limit = UINT16_MAX;
               expected_storage_type = ScalarStorageType::UInt32;
               break;
            case TypeCode::UInt32:
               expected_storage_type = ScalarStorageType::UInt32;
               break;
            default:
               retval = APX_UNSUPPORTED_ERROR;
            }
            if (m_state->scalar_type == expected_storage_type)
            {
               uint32_t u32_val;
               switch (expected_storage_type)
               {
               case ScalarStorageType::UInt32:
                  u32_val = std::get<std::uint32_t>(m_state->scalar_value);
                  retval = check_value_range_uint32(u32_val, u32_lower_limit, u32_upper_limit);
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

      apx::error_t Serializer::check_value_range_uint32(std::uint32_t value, std::uint32_t lower_limit, std::uint32_t upper_limit)
      {
         if (((lower_limit > 0u) && (value < lower_limit)) ||
            ((upper_limit < UINT32_MAX) && (value > upper_limit)))
         {
            return APX_VALUE_RANGE_ERROR;
         }
         return APX_NO_ERROR;
      }
   }
}