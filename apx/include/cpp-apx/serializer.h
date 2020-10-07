#pragma once

#include <stack>
#include <string>
#include <iterator>
#include <variant>
#include "cpp-apx/vmdefs.h"
#include "cpp-apx/error.h"
#include "dtl.hpp"

namespace apx
{
   namespace vm
   {
      class Serializer
      {
      public:
         struct State
         {
            State() { reset(dtl::ValueType::NoneType); }
            ~State() {}
            dtl::ValueType value_type{ dtl::ValueType::NoneType };

            std::variant<
               std::int32_t,             //index 0
               std::uint32_t,            //index 1
               std::int64_t,             //index 2
               std::uint64_t             //index 3
            > scalar_value;

            union value_tag
            {
               dtl::Value const* dv{ nullptr }; //Only used for NoneType
               dtl::Scalar const* sv;
               dtl::Array const* av;
               dtl::Hash const* hv;
            } value;

            ScalarStorageType scalar_type;
            std::string field_name;
            std::size_t index{ 0u };
            std::size_t array_len{ 0u };
            std::size_t max_array_len{ 0u }; //Needed for dynamic arrays
            TypeCode type_code{ TypeCode::None };
            std::size_t element_size{ 0 };
            bool is_last_field{ false };
            RangeCheckState range_check_state{ RangeCheckState::NotChecked };

            bool is_empty() { return (value_type == dtl::ValueType::NoneType) && (value.dv == nullptr); }
            void set_value(dtl::Value const* dv);
            void reset(dtl::ValueType vt);
            apx::error_t read_scalar_value(TypeCode type_code_arg);
            apx::error_t read_scalar_value(std::size_t index, TypeCode type_code_arg);
            bool is_scalar_type() { return ((type_code >= TypeCode::UInt8) && (type_code <= TypeCode::Int64)) || type_code == TypeCode::Bool; }
            bool is_string_type() { return (type_code == TypeCode::Char) ||
               (type_code == TypeCode::Char8) ||
               (type_code == TypeCode::Char16) ||
               (type_code == TypeCode::Char32); }
            bool is_bytes_type() { return type_code == TypeCode::Byte; }
         protected:
            apx::error_t read_scalar_value(dtl::Scalar const* sv, TypeCode type_code_arg);
         };

         struct WriteBuffer
         {
            std::uint8_t* begin{ nullptr };
            std::uint8_t* end{ nullptr };
            std::uint8_t* next{ nullptr };
            std::uint8_t* adjusted_next{ nullptr }; //Needed for dynamic arrays
         };
         Serializer() { reset(); }
         ~Serializer();
         void reset();
         apx::error_t set_write_buffer(std::uint8_t* buf, std::size_t len);
         apx::error_t set_value(dtl::Value const* dv);
         apx::error_t set_value(dtl::ScalarValue sv);
         apx::error_t set_value(dtl::ArrayValue av);
         apx::error_t set_value(dtl::HashValue hv);
         apx::error_t pack_uint8(std::size_t array_len, apx::SizeType dynamic_size);
         std::size_t bytes_written() { return std::distance(m_buffer.begin, m_buffer.next); }
         apx::error_t check_value_range_uint32(std::uint32_t lower_limit, std::uint32_t upper_limit);

      protected:
         WriteBuffer m_buffer;
         State* m_state{ nullptr };
         std::stack<State*> m_stack;
         void reset_buffer(std::uint8_t* buf, std::size_t len);
         bool is_valid_buffer();
         apx::error_t prepare_for_array(std::size_t array_size, apx::SizeType dynamic_size);
         apx::error_t pack_value();
         apx::error_t pack_scalar_value();
         apx::error_t pack_array_of_scalar();
         apx::error_t pack_scalar_value_internal();
         apx::error_t default_range_check();
         apx::error_t default_range_check_scalar();
         apx::error_t check_value_range_uint32(std::uint32_t value, std::uint32_t lower_limit, std::uint32_t upper_limit);
      };
   }
}