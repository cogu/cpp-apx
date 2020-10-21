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

            std::variant<
               std::int32_t,             //index 0
               std::uint32_t,            //index 1
               std::int64_t,             //index 2
               std::uint64_t,            //index 3
               bool                      //index 4
            > scalar_value;

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
            RangeCheckState range_check_state{ RangeCheckState::NotChecked };
            ScalarStorageType scalar_type{ ScalarStorageType::None };

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
            apx::error_t read_scalar_value(TypeCode type_code_arg);
            apx::error_t read_scalar_value(std::size_t index_arg, TypeCode type_code_arg);
         protected:
            apx::error_t read_scalar_value(dtl::Scalar const* sv_arg, TypeCode type_code_arg);
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
         dtl::ValueType value_type() { return m_state->value_type; }
         dtl::ScalarValue take_sv();
         dtl::ArrayValue take_av();
         dtl::HashValue take_hv();
         void clear_value();
         apx::error_t unpack_uint8(std::size_t array_len, apx::SizeType dynamic_size_type);
         apx::error_t unpack_uint16(std::size_t array_len, apx::SizeType dynamic_size_type);
         apx::error_t unpack_uint32(std::size_t array_len, apx::SizeType dynamic_size_type);
         apx::error_t check_value_range_int32(std::int32_t lower_limit, std::int32_t upper_limit);
         apx::error_t check_value_range_uint32(std::uint32_t lower_limit, std::uint32_t upper_limit);
         apx::error_t check_value_range_int64(std::int64_t lower_limit, std::int64_t upper_limit);
         apx::error_t check_value_range_uint64(std::uint64_t lower_limit, std::uint64_t upper_limit);
#ifdef UNIT_TEST
         dtl::Scalar const* get_sv_ptr() { return m_state->sv.get(); }
         dtl::Array const* get_av_ptr() { return m_state->av.get(); }
         dtl::Hash const* get_hv_ptr() { return m_state->hv.get(); }
#endif


      protected:
         ReadBuffer m_buffer;
         State* m_state{ nullptr };
         std::stack<State*> m_stack;
         void reset_buffer(std::uint8_t const* buf, std::size_t len);
         bool is_valid_buffer();
         void reset_state();
         apx::error_t prepare_for_array(std::size_t array_size, apx::SizeType dynamic_size);
         apx::error_t unpack_scalar_value(dtl::Scalar* sv);
         apx::error_t value_in_range_i32(std::int32_t value, std::int32_t lower_limit, std::int32_t upper_limit);
         apx::error_t value_in_range_u32(std::uint32_t value, std::uint32_t lower_limit, std::uint32_t upper_limit);
         apx::error_t value_in_range_i64(std::int64_t value, std::int64_t lower_limit, std::int64_t upper_limit);
         apx::error_t value_in_range_u64(std::uint64_t value, std::uint64_t lower_limit, std::uint64_t upper_limit);
      };
   }
}