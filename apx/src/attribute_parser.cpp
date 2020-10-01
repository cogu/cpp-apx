#include <string_view>
#include <cassert>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <algorithm>

#include "cpp-apx/attribute_parser.h"
#include "bstr.hpp"


namespace apx
{
   const char* AttributeParser::parse_initializer(const char* begin, const char* end, dtl::DynamicValue& dv)
   {
      reset();
      m_state = new State;
      auto next = parse_initializer_list(begin, end);
      if (next > begin)
      {
         assert(next <= end);
         if (m_state->initializer_list.use_count() > 0)
         {
            dv = std::dynamic_pointer_cast<dtl::Value>(m_state->initializer_list);
         }
         else if (m_state->sv.use_count() > 0)
         {
            dv = std::dynamic_pointer_cast<dtl::Value>(m_state->sv);
         }
      }
      delete m_state;
      m_state = nullptr;
      return next;
   }

   const char* AttributeParser::parse_port_attributes(const char* begin, const char* end, apx::PortAttributes& attr)
   {
      const char* next = begin;
      if ((end < begin) || (begin == nullptr) || (end == nullptr))
      {
         set_error(APX_INVALID_ARGUMENT_ERROR);
         return nullptr;
      }
      while (next < end)
      {
         next = bstr::lstrip(next, end);
         if (next == end)
         {
            break;
         }
         const char* result = parse_single_port_attribute(next, end, attr);
         if (result > next)
         {
            next = bstr::lstrip(result, end);
            if (next == end)
            {
               break;
            }
            if (*next == ',')
            {
               next++;
            }
            else
            {
               set_error(APX_PARSE_ERROR, next);
               next = nullptr;
               break;
            }
         }
         else
         {
            break;
         }
      }
      return next;
   }

   const char* AttributeParser::parse_type_attributes(const char* begin, const char* end, apx::TypeAttributes& attr)
   {
      const char* next = begin;
      if ((end < begin) || (begin == nullptr) || (end == nullptr))
      {
         set_error(APX_INVALID_ARGUMENT_ERROR);
         return nullptr;
      }
      while (next < end)
      {
         next = bstr::lstrip(next, end);
         if (next == end)
         {
            break;
         }
         const char* result = parse_single_type_attribute(next, end, attr);
         if (result > next)
         {
            next = bstr::lstrip(result, end);
            if (next == end)
            {
               break;
            }
            if (*next == ',')
            {
               next++;
            }
            else
            {
               set_error(APX_PARSE_ERROR, next);
               next = nullptr;
               break;
            }
         }
         else
         {
            break;
         }
      }
      return next;
   }

   const char* AttributeParser::parse_single_port_attribute(const char* begin, const char* end, apx::PortAttributes& attr)
   {
      const char* next = begin;
      char c = *next;
      PortAttributeType attr_type = PortAttributeType::None;
      switch (c)
      {
      case '=':
         attr_type = PortAttributeType::InitValue;
         break;
      case 'P':
         attr_type = PortAttributeType::Parameter;
         break;
      case 'Q':
         attr_type = PortAttributeType::QueueLength;
         break;
      default:
         attr_type = PortAttributeType::None;
      }
      if (attr_type != PortAttributeType::None)
      {
         const char* result = nullptr;
         switch (attr_type)
         {
         case PortAttributeType::None:
            assert(0);
         case PortAttributeType::InitValue:
            result = parse_initializer(next + 1, end, attr.init_value);
            break;
         case PortAttributeType::Parameter:
            result = next + 1;
            attr.is_parameter = true;
            break;
         case PortAttributeType::QueueLength:
            result = parse_array_length(next + 1, end, attr.queue_length);
            break;
         }
         if (result > next)
         {
            next = result;
         }
         else
         {
            set_error(APX_PARSE_ERROR, next);
            next = nullptr;
         }
      }
      return next;
   }

   const char* AttributeParser::parse_single_type_attribute(const char* begin, const char* end, apx::TypeAttributes& attr)
   {
      const char* next = begin;
      const char* result;
      apx::ValueTable* vt = nullptr;
      apx::RationalScaling* rs = nullptr;
      apx::Computation* computation = nullptr;
      TypeAttributeType attr_type = TypeAttributeType::None;
      if (result = bstr::match_str<char>(next, end, "VT"); result > begin)
      {
         attr_type = TypeAttributeType::ValueTable;
         next += 2;
      }
      else if (result = bstr::match_str<char>(next, end, "RS"); result > begin)
      {
         attr_type = TypeAttributeType::RationalScaling;
         next += 2;
      }
      else if (*next == 'D')
      {
         attr_type = TypeAttributeType::DynamicLength;
         next++;
      }
      switch (attr_type)
      {
      case TypeAttributeType::None:
         result = nullptr;
      case TypeAttributeType::ValueTable:
         result = parse_value_table(next, end, vt);
         if (result > next)
         {
            computation = dynamic_cast<apx::Computation*>(vt);
         }
         break;
      case TypeAttributeType::RationalScaling:
         result = parse_rational_scaling(next, end, rs);
         if (result > next)
         {
            computation = dynamic_cast<apx::Computation*>(rs);
         }
         break;
      case TypeAttributeType::DynamicLength:
         result = parse_array_length(next, end, attr.dynamic_length);
         break;
      }
      if (result > next)
      {
         assert(result <= end);
         next = result;
         if (computation != nullptr)
         {
            attr.computations.push_back(std::unique_ptr<apx::Computation>{computation});
         }
      }
      else
      {
         if (computation != nullptr)
         {
            delete computation;
         }
         set_error(APX_PARSE_ERROR, next);
         next = nullptr;
      }
      return next;
   }

   const char* AttributeParser::parse_initializer_list(const char* begin, const char* end)
   {
      const char* next = begin;
      while (next < end)
      {
         next = bstr::lstrip(next, end);
         char c = *next;

         if (c == '{')
         {
            next++;
            m_state->initializer_list = dtl::make_av();
            m_stack.push(m_state);
            State* child_state = new State;
            child_state->parent = m_state;
            std::swap(m_state, child_state);
            continue;
         }
         else
         {
            auto result = parse_scalar(next, end);
            if (result > next)
            {
               next = result;
            }
            else if (result == nullptr)
            {
               return result;
            }
         }
      POST_VALUE_HANDLER:
         if (is_list_initializer())
         {
            next = bstr::lstrip(next, end);
            c = *next;
            if (m_state->has_value())
            {
               if (!push_value_to_parent())
               {
                  return nullptr;
               }
            }
            if (bool is_empty = m_state->parent->initializer_list->is_empty(); !is_empty && (c == ','))
            {
               next++;
            }
            else if (c == '}')
            {
               next++;
               if (m_stack.size() > 0)
               {
                  delete m_state;
                  m_state = m_stack.top();
                  m_stack.pop();
                  if (m_stack.size() == 0u)
                  {
                     break; //Reached top of stack
                  }
                  else
                  {
                     goto POST_VALUE_HANDLER;
                  }
               }
               else
               {
                  return nullptr;
               }
            }
            else
            {
               //unexpected character
               return nullptr;
            }
         }
         else
         {
            break;
         }
      }
      next = bstr::lstrip(next, end);
      return next;
   }

   bool AttributeParser::push_value_to_parent()
   {
      if (m_state->parent != nullptr)
      {
         if (m_state->sv.use_count() > 0)
         {
            m_state->parent->initializer_list->push(std::dynamic_pointer_cast<dtl::Value>(m_state->sv));
            m_state->sv.reset();
         }
         else if (m_state->initializer_list.use_count() > 0)
         {
            m_state->parent->initializer_list->push(std::dynamic_pointer_cast<dtl::Value>(m_state->initializer_list));
            m_state->initializer_list.reset();
         }
         else
         {
            return false;
         }
      }
      return true;
   }

   const char* AttributeParser::parse_scalar(const char* begin, const char* end)
   {
      auto next = begin;
      char c = *next;
      if (c > 0)
      {
         if (std::isdigit(c))
         {
            m_state->sv = dtl::make_sv();
            auto result = parse_integer_literal(next, end, false, m_state->sv);
            if (result > next)
            {
               assert(result <= end);
               next = result;
            }
         }
         else if (c == '-')
         {
            next++;
            if (next < end)
            {
               m_state->sv = dtl::make_sv();
               const char* result = parse_integer_literal(next, end, true, m_state->sv);
               if (result > next)
               {
                  assert(result <= end);
                  next = result;
               }
            }
            else
            {
               return nullptr; //Nothing after the minus sign?
            }
         }
         else if (c == '"')
         {
            m_state->sv = dtl::make_sv();
            auto result = parse_string_literal(next, end, m_state->sv);
            if (result > next)
            {
               assert(result <= end);
               next = result;
            }
         }
      }
      return next;
   }

   const char* AttributeParser::parse_integer_literal(const char* next, const char* end, bool unary_minus, dtl::ScalarValue& sv)
   {
      char* end_ptr = nullptr;
      unsigned long value = std::strtoul(next, &end_ptr, 0);
      if ((end_ptr > next) && (end_ptr <= end))
      {
         next = end_ptr;
         if (unary_minus && (value == 0x80000000UL))
         {
            sv->set(INT32_MIN);
         }
         else if (value > INT32_MAX)
         {
            sv->set(static_cast<uint32_t>(value));
         }
         else
         {
            int32_t tmp = static_cast<int32_t>(value);
            sv->set(unary_minus ? -tmp : tmp);
         }
      }
      else
      {
         return nullptr;
      }
      return next;
   }

   const char* AttributeParser::parse_string_literal(const char* begin, const char* end, dtl::ScalarValue& sv)
   {
      assert(begin < end);
      const char* next = bstr::match_pair<char>(begin, end, '"', '"', '\\');
      if (next > begin)
      {
         assert(next < end);
         sv->set(begin + 1, next);
         next++; //skip past ending '"' character
      }
      return next;
   }



   const char* AttributeParser::parse_array_length(const char* begin, const char* end, uint32_t& length)
   {
      const char* next = begin;
      if (next < end)
      {
         if (*next++ == '[')
         {
            next = bstr::lstrip(next, end);
            if (next < end)
            {
               const char* result = parse_u32(next, end, length);
               if (result > next)
               {
                  next = bstr::lstrip(result, end);
                  if (next < end)
                  {
                     if (*next++ == ']')
                     {
                        return next;
                     }
                     else
                     {
                        return nullptr;
                     }
                  }
               }
               else
               {
                  return nullptr;
               }
            }
         }
         else
         {
            return nullptr;
         }
      }
      return begin;
   }

   const char* AttributeParser::parse_value_table(const char* begin, const char* end, apx::ValueTable*& vt)
   {
      const char* next = begin;
      if (next < end)
      {
         if (*next++ != '(')
         {
            return nullptr;
         }
         ValueTableState state;
         while (next < end)
         {
            next = bstr::lstrip(next, end);
            const char* result = parse_value_table_arg(next, end, state);
            if (result > next)
            {
               assert(result <= end);
               next = result;
            }
            else
            {
               next = nullptr;
               break;
            }
            next = bstr::lstrip(next, end);
            const char c = *next;
            if (c == ',')
            {
               //prepare for parsing next argument
               next++;
            }
            else if (c == ')')
            {
               //that was the last argument
               vt = create_value_table_from_state(state);
               return next + 1;
            }
         }
      }
      return begin; //not enough characters in stream
   }

   const char* AttributeParser::parse_rational_scaling(const char* begin, const char* end, apx::RationalScaling*& rs)
   {
      const char* next = begin;
      if (next < end)
      {
         if (*next++ != '(')
         {
            return nullptr;
         }
         RationalScalingState state;
         while (next < end)
         {
            next = bstr::lstrip(next, end);
            const char* result = parse_rational_scaling_arg(next, end, state);
            if (result > next)
            {
               assert(result <= end);
               next = result;
            }
            else
            {
               next = nullptr;
               break;
            }
            next = bstr::lstrip(next, end);
            const char c = *next;
            if (c == ',')
            {
               //prepare for parsing next argument
               next++;
            }
            else if (c == ')')
            {
               //that was the last argument
               rs = create_rational_scaling_from_state(state);
               return next + 1;
            }
         }
      }
      return begin; //not enough characters in stream
   }

   const char* AttributeParser::parse_value_table_arg(const char* begin, const char* end, ValueTableState& vts)
   {
      const char* next = begin;
      if (next < end)
      {
         ValueTableArgType arg_type = ValueTableArgType::Invalid;
         const char* result = nullptr;
         bool unary_minus = false;

         if (!vts.last_was_string)
         {
            //Determing if next character is a string or integer literal
            char c = *next;
            if (c == '"')
            {
               arg_type = ValueTableArgType::StringLiteral;
            }
            else if (c == '-')
            {
               unary_minus = true;
               next++;
               arg_type = ValueTableArgType::Integer;
            }
            else if (std::isdigit(c))
            {
               arg_type = ValueTableArgType::Integer;
            }
         }
         else
         {
            if (*next == '"')
            {
               arg_type = ValueTableArgType::StringLiteral;
            }
         }
         switch (arg_type)
         {
         case ValueTableArgType::Invalid:
            next = nullptr;
            break;
         case ValueTableArgType::Integer:
            if (vts.num_count == 0)
            {
               result = parse_lower_range(next, end, unary_minus, dynamic_cast<ComputationState*>(&vts));
            }
            else if (vts.num_count == 1)
            {
               result = parse_upper_range(next, end, unary_minus, dynamic_cast<ComputationState*>(&vts));
            }
            else
            {
               result = nullptr; //Three consecutive numbers not allowed
            }
            if (result > next)
            {
               next = result;
            }
            vts.num_count++;
            break;
         case ValueTableArgType::StringLiteral:
            vts.last_was_string = true;
            result = bstr::match_pair<char>(next, end, '"', '"', '\\');
            if (result > next)
            {
               std::string tmp{ next + 1, static_cast<size_t>(std::distance(next + 1, result)) };
               vts.values.push_back(tmp);
               next = result + 1; //skip past ending '"' character
            }
            break;
         }
      }
      return next;
   }

   const char* AttributeParser::parse_rational_scaling_arg(const char* begin, const char* end, RationalScalingState& rss)
   {
      const char* next = begin;
      if (next < end)
      {
         const char* result = nullptr;
         bool unary_minus = false;
         double d = 0.0;
         int32_t i32 = 0;
         std::string unit;
         switch (rss.arg_index)
         {
         case 0u: //LOWER RANGE LIMIT (integer)
            if (*next == '-')
            {
               unary_minus = true;
               next++;
            }
            result = parse_lower_range(next, end, unary_minus, dynamic_cast<ComputationState*>(&rss));
            break;
         case 1: //UPPER RANGE LIMIT (integer)
            if (*next == '-')
            {
               unary_minus = true;
               next++;
            }
            result = parse_upper_range(next, end, unary_minus, dynamic_cast<ComputationState*>(&rss));
            break;
         case 2: //OFFSET (double)
            result = parse_double(next, end, d);
            if (result > next)
            {
               rss.offset = d;
            }
            break;
         case 3: //NUMERATOR (integer)
            result = parse_i32(next, end, i32);
            if (result > next)
            {
               rss.numerator = i32;
            }
            break;
         case 4: //DENOMINATOR (integer)
            result = parse_i32(next, end, i32);
            if (result > next)
            {
               rss.denominator = i32;
            }
            break;
         case 5:
            result = bstr::match_pair<char>(next, end, '"', '"', '\\');
            if (result > next)
            {
               rss.unit.assign(next + 1, static_cast<std::size_t>(std::distance(next + 1, result)));
               result++;
            }
            break;
         default:
            result = nullptr;
         }
         next = result;
         rss.arg_index++;
      }
      return next;
   }

   const char* AttributeParser::parse_lower_range(const char* begin, const char* end, bool unary_minus, ComputationState* state)
   {
      const char* next = begin;
      char* end_ptr;
      uint32_t tmp = (uint32_t)std::strtoul(next, &end_ptr, 0);
      if ((end_ptr > next) && (end_ptr <= end))
      {
         next = end_ptr;
         if (unary_minus)
         {
            if (tmp > 0x80000000UL)
            {
               return nullptr; //value out of range
            }
            state->i32_range.first = -static_cast<int32_t>(tmp);
            state->is_signed_range = true;
         }
         else
         {
            state->u32_range.first = tmp;
         }
      }
      else
      {
         next = nullptr;
      }
      return next;
   }

   const char* AttributeParser::parse_upper_range(const char* begin, const char* end, bool unary_minus, ComputationState* state)
   {
      const char* next = begin;
      char* end_ptr;
      uint32_t tmp = (uint32_t)std::strtoul(next, &end_ptr, 0);
      if ((end_ptr > next) && (end_ptr <= end))
      {
         next = end_ptr;
         if (unary_minus)
         {
            if (!state->is_signed_range)
            {
               next = nullptr; //lower range is positive while upper range is negative?
            }
            else
            {
               if (tmp > 0x80000000UL)
               {
                  return nullptr; //value out of range
               }
               state->i32_range.second = -static_cast<int32_t>(tmp);
            }
         }
         else
         {
            state->u32_range.second = tmp;
         }
      }
      else
      {
         next = nullptr;
      }
      return next;
   }

   const char* AttributeParser::parse_double(const char* begin, const char* end, double& d)
   {
      const char* retval = nullptr;
      char* end_ptr;
      d = std::strtod(begin, &end_ptr);
      if ((end_ptr > begin) && (end_ptr < end))
      {
         retval = end_ptr;
      }
      return retval;
   }

   const char* AttributeParser::parse_i32(const char* begin, const char* end, int32_t& v)
   {
      const char* retval = nullptr;
      char* end_ptr;
      v = (int32_t)std::strtol(begin, &end_ptr, 0);
      if ((end_ptr > begin) && (end_ptr < end))
      {
         retval = end_ptr;
      }
      return retval;
   }

   const char* AttributeParser::parse_u32(const char* begin, const char* end, uint32_t& v)
   {
      const char* retval = nullptr;
      char* end_ptr;
      v = (uint32_t)std::strtoul(begin, &end_ptr, 0);
      if ((end_ptr > begin) && (end_ptr < end))
      {
         retval = end_ptr;
      }
      return retval;
   }

   void AttributeParser::reset()
   {
      while (!m_stack.empty())
      {
         delete m_stack.top();
         m_stack.pop();
      }
      if (m_state != nullptr) delete m_state;
   }
   apx::ValueTable* AttributeParser::create_value_table_from_state(ValueTableState& vts)
   {
      auto vt = std::make_unique<apx::ValueTable>();
      bool auto_upper_range = false;
      if (vts.num_count > 0)
      {
         if (vts.num_count == 1)
         {
            auto_upper_range = true;
         }
         if (vts.is_signed_range)
         {
            vt->set_range(vts.i32_range.first, auto_upper_range ? vts.i32_range.first : vts.i32_range.second);
         }
         else
         {
            vt->set_range(vts.u32_range.first, auto_upper_range ? vts.u32_range.first : vts.u32_range.second);
         }
      }
      else
      {
         auto_upper_range = true;
      }
      int32_t i32_index = vt->lower_range.i32;
      uint32_t u32_index = vt->lower_range.i32;
      for (auto it = vts.values.begin(); it != vts.values.end(); it++)
      {
         vt->values.push_back(*it);
         if (vt->is_signed_range)
         {
            if (auto_upper_range && (vt->upper_range.i32 < i32_index))
            {
               vt->upper_range.i32 = i32_index;
            }
            i32_index++;
         }
         else
         {
            if (auto_upper_range && (vt->upper_range.u32 < u32_index))
            {
               vt->upper_range.u32 = u32_index;
            }
            u32_index++;
         }
      }
      return vt.release();
   }
   apx::RationalScaling* AttributeParser::create_rational_scaling_from_state(RationalScalingState& vts)
   {
      auto rs = std::make_unique<apx::RationalScaling>(vts.offset, vts.numerator, vts.denominator, vts.unit);
      if (vts.is_signed_range)
      {
         rs->set_range(vts.i32_range.first, vts.i32_range.second);
      }
      else
      {
         rs->set_range(vts.u32_range.first, vts.u32_range.second);
      }
      return rs.release();
   }

}