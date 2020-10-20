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
               std::uint64_t,            //index 3
               bool                      //index 4
            > scalar_value;

            union value_tag
            {
               dtl::Value const* dv{ nullptr }; //Only used for NoneType
               dtl::Scalar const* sv;
               dtl::Array const* av;
               dtl::Hash const* hv;
            } value;

            State* parent{ nullptr };
            ScalarStorageType scalar_type;
            std::string field_name;
            std::size_t index{ 0u };
            std::size_t array_len{ 0u };
            std::size_t max_array_len{ 0u }; //Needed for dynamic arrays
            TypeCode type_code{ TypeCode::None };
            std::size_t element_size{ 0 };
            bool is_last_field{ false };
            RangeCheckState range_check_state{ RangeCheckState::NotChecked };
            apx::SizeType dynamic_size_type{ apx::SizeType::None };

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
            dtl::Value const* get_child_value(const char* key);
            void set_field_name(const char* name, bool is_last) { field_name = name;  is_last_field = is_last; }
            apx::error_t determine_array_length_from_value();
         protected:
            apx::error_t read_scalar_value(dtl::Scalar const* sv, TypeCode type_code_arg);
         };

         struct WriteBuffer
         {
            std::uint8_t* begin{ nullptr };
            std::uint8_t* end{ nullptr };
            std::uint8_t* next{ nullptr };
            std::uint8_t* padded_next{ nullptr }; //Used when serializing dynamic arrays
            std::uint8_t* mark{ nullptr };        //Used to verify correctness of queued writes
         };

         struct QueuedWriteState
         {
            std::uint32_t max_length{ 0u };
            std::uint32_t current_length{ 0u };
            std::uint32_t element_size{ 0u };
            std::uint8_t* length_ptr{ nullptr };
            apx::SizeType size_type{ apx::SizeType::None };
            bool is_enabled{ false };
         };

         Serializer() { m_state = new Serializer::State(); }
         ~Serializer();
         void reset();
         apx::error_t set_write_buffer(std::uint8_t* buf, std::size_t len);
         std::size_t bytes_written() { return std::distance(m_buffer.begin, m_buffer.next); }
         apx::error_t set_value(dtl::Value const* dv);
         apx::error_t set_value(dtl::ScalarValue sv);
         apx::error_t set_value(dtl::ArrayValue av);
         apx::error_t set_value(dtl::HashValue hv);
         void clear_value();
         apx::error_t pack_uint8(std::size_t array_len, apx::SizeType dynamic_size_type);
         apx::error_t pack_uint16(std::size_t array_len, apx::SizeType dynamic_size_type);
         apx::error_t pack_uint32(std::size_t array_len, apx::SizeType dynamic_size_type);
         apx::error_t pack_uint64(std::size_t array_len, apx::SizeType dynamic_size_type);
         apx::error_t pack_int8(std::size_t array_len, apx::SizeType dynamic_size_type);
         apx::error_t pack_int16(std::size_t array_len, apx::SizeType dynamic_size_type);
         apx::error_t pack_int32(std::size_t array_len, apx::SizeType dynamic_size_type);
         apx::error_t pack_int64(std::size_t array_len, apx::SizeType dynamic_size_type);
         apx::error_t pack_bool(std::size_t array_len, apx::SizeType dynamic_size_type);
         apx::error_t pack_byte_array(std::size_t array_len, apx::SizeType dynamic_size_type);
         apx::error_t pack_char(std::size_t array_len, apx::SizeType dynamic_size_type);
         apx::error_t pack_char8(std::size_t array_len, apx::SizeType dynamic_size_type);
         apx::error_t pack_record(std::size_t array_len, apx::SizeType dynamic_size_type);
         apx::error_t check_value_range_int32(std::int32_t lower_limit, std::int32_t upper_limit);
         apx::error_t check_value_range_uint32(std::uint32_t lower_limit, std::uint32_t upper_limit);
         apx::error_t check_value_range_int64(std::int64_t lower_limit, std::int64_t upper_limit);
         apx::error_t check_value_range_uint64(std::uint64_t lower_limit, std::uint64_t upper_limit);
         apx::error_t record_select(const char* key, bool is_last_field);
         apx::error_t queued_write_begin(std::uint32_t element_size, std::uint32_t max_length, bool clear_queue);
         apx::error_t queued_write_end();

      protected:
         WriteBuffer m_buffer;
         QueuedWriteState m_queued_write;
         State* m_state{ nullptr };
         std::stack<State*> m_stack;
         void reset_buffer(std::uint8_t* buf, std::size_t len);
         bool is_valid_buffer();
         apx::error_t prepare_for_array(std::size_t array_size, apx::SizeType dynamic_size_type);
         apx::error_t pack_value();
         apx::error_t pack_scalar_value();
         apx::error_t pack_array_of_scalar();
         apx::error_t pack_scalar_value_internal();
         apx::error_t pack_string();
         apx::error_t pack_char_string(std::string const& str, std::size_t max_target_size);
         apx::error_t pack_byte_array_internal();
         apx::error_t default_range_check_value();
         apx::error_t default_range_check_scalar();
         apx::error_t value_in_range_i32(std::int32_t value, std::int32_t lower_limit, std::int32_t upper_limit);
         apx::error_t value_in_range_u32(std::uint32_t value, std::uint32_t lower_limit, std::uint32_t upper_limit);
         apx::error_t value_in_range_i64(std::int64_t value, std::int64_t lower_limit, std::int64_t upper_limit);
         apx::error_t value_in_range_u64(std::uint64_t value, std::uint64_t lower_limit, std::uint64_t upper_limit);
         void pop_state();
         apx::error_t write_dynamic_value_to_buffer(std::size_t value, apx::SizeType size_type);
         apx::error_t write_dynamic_value_to_buffer(std::uint8_t* begin, std::uint8_t* end, std::size_t value, std::size_t value_size);
         apx::error_t read_dynamic_value_from_buffer(std::uint8_t const* begin, std::uint8_t const* end, std::size_t &value, std::size_t value_size);
         apx::error_t prepare_for_buffer_write();
      };
   }
}