#pragma once

#include <stack>
#include <string>
#include <iterator>
#include <variant>
#include "cpp-apx/vmdefs.h"
#include "cpp-apx/error.h"
#include "dtl/dtl.hpp"

namespace apx
{
   namespace vm
   {
      class Deserializer
      {
      public:
         struct State
         {
            State() {}
            ~State() {}
            dtl::ValueType value_type{ dtl::ValueType::NoneType };

            dtl::ScalarValue sv; //Valid when value_type==ValueType::Scalar
            dtl::ArrayValue av;  //Valid when value_type==ValueType::Array
            dtl::HashValue hv;   //Valid when value_type==ValueType::Hash
            State* parent{ nullptr };
            std::string field_name;
            std::size_t index{ 0u };
            std::size_t array_len{ 0u };
            std::size_t max_array_len{ 0u }; //Needed for dynamic arrays
            TypeCode type_code{ TypeCode::None };
            std::size_t element_size{ 0 };
            bool is_last_field{ false };

            void clear();
            bool is_scalar_type() { return ((type_code >= TypeCode::UInt8) && (type_code <= TypeCode::Int64)) || type_code == TypeCode::Bool; }
            bool is_string_type() {
               return (type_code == TypeCode::Char) ||
                  (type_code == TypeCode::Char8) ||
                  (type_code == TypeCode::Char16) ||
                  (type_code == TypeCode::Char32);
            }
            bool is_byte_type() { return type_code == TypeCode::Byte; }
            void set_field_name(const char* name, bool is_last) { field_name = name;  is_last_field = is_last; }
            void init_scalar_value();
            void init_array_value();
            void init_record_value();
         protected:

         };

         struct ReadBuffer
         {
            std::uint8_t const* begin{ nullptr };
            std::uint8_t const* end{ nullptr };
            std::uint8_t const* next{ nullptr };
            std::uint8_t const* adjusted_next{ nullptr }; //Needed for dynamic arrays
         };
         Deserializer() { m_state = new State(); }
         ~Deserializer();

         apx::error_t set_read_buffer(std::uint8_t const* buf, std::size_t len);
         std::size_t bytes_read() { return std::distance(m_buffer.begin, m_buffer.next); }
         apx::error_t unpack_uint8(std::size_t array_len, apx::SizeType dynamic_size, dtl::ValueType &value_type);
         dtl::Scalar const* get_scalar() { return m_state->sv.get(); }
         dtl::Array const* get_array() { return m_state->av.get(); }
         dtl::Hash const* get_hash() { return m_state->hv.get(); }


      protected:
         ReadBuffer m_buffer;
         State* m_state{ nullptr };
         std::stack<State*> m_stack;
         void reset_buffer(std::uint8_t const* buf, std::size_t len);
         bool is_valid_buffer();
         void reset_state();
         apx::error_t prepare_for_array(std::size_t array_size, apx::SizeType dynamic_size);
         apx::error_t unpack_scalar_value(dtl::Scalar* sv);
      };
   }
}