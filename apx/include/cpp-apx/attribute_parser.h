#pragma once

#include <istream>
#include <cstdint>
#include <string>
#include <stack>
#include <utility>
#include "cpp-apx/types.h"
#include "dtl.hpp"
#include "cpp-apx/port_attribute.h"
#include "cpp-apx/type_attribute.h"
#include "cpp-apx/error.h"

namespace apx
{
   class AttributeParser
   {
   public:
      struct State
      {
         State* parent = nullptr;
         dtl::ScalarValue sv;
         dtl::ArrayValue initializer_list;
      };

      struct ComputationState
      {
         ComputationState():i32_range{0,0}, u32_range{0,0}{}
         std::pair<int32_t, int32_t> i32_range;
         std::pair<uint32_t, uint32_t> u32_range;
         bool is_signed_range = false;
      };

      struct ValueTableState : public ComputationState
      {
         std::vector<std::string> values;
         int num_count = 0;
         bool last_was_string = false;
      };

      enum class ValueTableArgType
      {
         Invalid,
         Integer,
         StringLiteral
      };

      struct RationalScalingState : public ComputationState
      {
         double offset = 0.0;
         int32_t numerator = 0;
         int32_t denominator = 0;
         unsigned int arg_index = 0u;
         std::string unit;
      };
      AttributeParser() :m_state{ nullptr }, m_last_error{ APX_NO_ERROR }, m_error_next{ nullptr } {};
      ~AttributeParser() { reset(); }
      const char* parse_initializer(const char* begin, const char* end, dtl::DynamicValue& dv);
      const char* parse_port_attributes(const char* begin, const char* end, apx::PortAttributes& attr);
      const char* parse_type_attributes(const char* begin, const char* end, apx::TypeAttributes& attr);
      apx::error_t get_last_error(const char*& error_next) { error_next = m_error_next;  return m_last_error; }
   protected:

      const char* parse_single_port_attribute(const char* begin, const char* end, apx::PortAttributes& attr);
      const char* parse_single_type_attribute(const char* begin, const char* end, apx::TypeAttributes& attr);
      const char* parse_initializer_list(const char* begin, const char* end);
      bool push_value_to_parent();
      const char* parse_scalar(const char* begin, const char* end);
      const char* parse_integer_literal(const char* next, const char* end, bool unary_minus, dtl::ScalarValue& sv);
      const char* parse_string_literal(const char* begin, const char* end, dtl::ScalarValue& sv);
      const char* parse_array_length(const char* begin, const char* end, uint32_t& length);
      const char* parse_value_table(const char* begin, const char* end, apx::ValueTable*& vt);
      const char* parse_rational_scaling(const char* begin, const char* end, apx::RationalScaling*& rs);
      const char* parse_value_table_arg(const char* begin, const char* end, ValueTableState& vts);
      const char* parse_rational_scaling_arg(const char* begin, const char* end, RationalScalingState& rss);
      const char* parse_lower_range(const char* begin, const char* end, bool unary_minus, ComputationState* state);
      const char* parse_upper_range(const char* begin, const char* end, bool unary_minus, ComputationState* state);
      const char* parse_double(const char* begin, const char* end, double& d);
      const char* parse_i32(const char* begin, const char* end, int32_t& v);
      const char* parse_u32(const char* begin, const char* end, uint32_t& v);
      void reset();
      bool is_list_initializer() { return m_state->parent != nullptr; }
      void set_error(apx::error_t error, const char* error_next = nullptr) { m_last_error = error; m_error_next = error_next; }

      apx::ValueTable* create_value_table_from_state(ValueTableState& vts);
      apx::RationalScaling* create_rational_scaling_from_state(RationalScalingState& vts);
      std::stack<State*> m_stack;
      State *m_state;
      apx::error_t m_last_error;
      const char* m_error_next;
   };
}