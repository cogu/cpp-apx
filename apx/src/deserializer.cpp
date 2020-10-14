#include <cassert>
#include "cpp-apx/deserializer.h"
#include "cpp-apx/pack.h"

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

      Deserializer::~Deserializer()
      {
         reset_state();
         if (m_state != nullptr)
         {
            delete m_state;
         }
      }

      apx::error_t Deserializer::unpack_uint8(std::size_t array_len, apx::SizeType dynamic_size, dtl::ValueType& value_type)
      {
         if (!is_valid_buffer())
         {
            return APX_MISSING_BUFFER_ERROR;
         }
         reset_state();
         m_state->type_code = TypeCode::UInt8;
         m_state->element_size = UINT8_SIZE;
         apx::error_t result = APX_NO_ERROR;
         if (array_len > 0u)
         {
            result = prepare_for_array(array_len, dynamic_size);
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
         value_type = m_state->value_type;
         return result;
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
            return APX_NO_ERROR;
         }
         return APX_INVALID_ARGUMENT_ERROR;
      }

      void Deserializer::reset_buffer(std::uint8_t const* buf, std::size_t len)
      {
         m_buffer.begin = buf;
         m_buffer.end = buf + len;
         m_buffer.next = buf;
         m_buffer.adjusted_next = nullptr;
      }

      bool Deserializer::is_valid_buffer()
      {
         return ((m_buffer.next != nullptr) && (m_buffer.end != nullptr) && (m_buffer.next <= m_buffer.end));
      }

      apx::error_t Deserializer::prepare_for_array(std::size_t array_size, apx::SizeType dynamic_size)
      {
         (void)array_size;
         (void)dynamic_size;
         return APX_NOT_IMPLEMENTED_ERROR;
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
   }
}