#include <cassert>
#include "cpp-apx/signature_parser.h"
#include "bstr/bstr.hpp"

namespace apx
{
   const char* SignatureParser::parse_data_signature(const char* begin, const char* end)
   {
      reset();
      m_state = new State();
      return parse_data_element(begin, end);
   }

   void SignatureParser::reset()
   {
      while (!m_stack.empty())
      {
         delete m_stack.top();
         m_stack.pop();
      }
      if (m_state != nullptr)
      {
         delete m_state;
      }
   }

   const char* SignatureParser::parse_data_element(const char* begin, const char* end)
   {
      const char* next = begin;
      std::string element_name;
      if (m_state->is_record)
      {
         const auto result = bstr::match_pair(next, end, '"', '"', '\\');
         if (result > next)
         {
            element_name.assign(next + 1, result);
            next = result + 1;
         }
         else
         {
            set_error(APX_PARSE_ERROR, next);
            return nullptr;
         }
      }
      if (next >= end)
      {
         return begin;
      }
      const char c = *next;
      bool is_signed_type = true;
      bool check_limits = true;
      apx::TypeCode type_code = apx::TypeCode::None;
      apx::TokenClass token_class = apx::TokenClass::DataElement;
      switch (c)
      {
      case '{':
         type_code = apx::TypeCode::Record;
         check_limits = false;
         break;
      case '[':
         token_class = apx::TokenClass::GroupDeclaration;
         check_limits = false;
         break;
      case '(':
         token_class = apx::TokenClass::FunctionDeclaration;
         check_limits = false;
         break;
      case 'a':
         type_code = apx::TypeCode::Char;
         check_limits = false;
         break;
      case 'b':
         type_code = apx::TypeCode::Bool;
         check_limits = false;
         break;
      case 'B':
         type_code = apx::TypeCode::Byte;
         is_signed_type = false;
         break;
      case 'c':
         type_code = apx::TypeCode::Int8;
         break;
      case 'C':
         type_code = apx::TypeCode::UInt8;
         is_signed_type = false;
         break;
      case 'l':
         type_code = apx::TypeCode::Int32;
         break;
      case 'L':
         type_code = apx::TypeCode::UInt32;
         is_signed_type = false;
         break;
      case 's':
         type_code = apx::TypeCode::Int16;
         break;
      case 'S':
         type_code = apx::TypeCode::UInt16;
         is_signed_type = false;
         break;
      case 'T':
         type_code = apx::TypeCode::TypeRefId; //Initial guess, might change while parsing data inside brackets
         check_limits = false;
         break;
      case 'u':
         type_code = apx::TypeCode::Int64;
         break;
      case 'U':
         type_code = apx::TypeCode::UInt64;
         is_signed_type = false;
         break;
      }
      if (token_class == apx::TokenClass::DataElement)
      {
         if (type_code == apx::TypeCode::None)
         {
            set_error(APX_PARSE_ERROR, next);
            return nullptr;
         }
         else
         {
            m_state->data_element = std::make_unique<apx::DataElement>(type_code);
            next++;
            if (element_name.length() > 0)
            {
               m_state->data_element->set_name(std::move(element_name));
            }
         }
         if (type_code == apx::TypeCode::TypeRefId)
         {
            auto result = parse_type_reference(next, end);
            if (result > next)
            {
               next = result;
            }
            else
            {
               set_error(APX_PARSE_ERROR, next);
               return nullptr;
            }
         }
         else if (type_code == apx::TypeCode::Record)
         {
            auto parent = m_state;
            m_state = new State(true);
            while (next < end)
            {
               auto result = parse_data_element(next, end);
               if (result > next)
               {
                  next = result;
                  assert(parent->data_element.get() != nullptr);
                  assert(m_state->data_element.get() != nullptr);
                  parent->data_element->append(std::move(m_state->data_element));
                  assert(m_state->data_element.get() == nullptr);
               }
               else
               {
                  set_error(APX_PARSE_ERROR, next);
                  delete m_state;
                  m_state = parent;
                  return nullptr;
               }
               if (next >= end)
               {
                  set_error(APX_PARSE_ERROR, next);
                  delete m_state;
                  m_state = parent;
                  return nullptr;
               }
               if (*next == '}')
               {
                  next++;
                  delete m_state;
                  m_state = parent;
                  break;
               }
            }
         }
         if (check_limits)
         {
            auto result = parse_limits(next, end, is_signed_type);
            if (result > next)
            {
               next = result;
            }
            else if (result == nullptr)
            {
               set_error(APX_PARSE_ERROR, next);
               return nullptr;
            }
         }
         auto result = parse_array(next, end);
         if (result > next)
         {
            next = result;
         }
         else if (result == nullptr)
         {
            set_error(APX_PARSE_ERROR, next);
            return nullptr;
         }
      }
      else
      {
         set_error(APX_NOT_IMPLEMENTED_ERROR, next);
         return nullptr;
      }
      return next;
   }
   const char* SignatureParser::parse_limits(const char* begin, const char* end, bool is_signed_type)
   {
      const char* next = begin;
      if (next < end)
      {
         if (*next++ != '(')
         {
            return begin; //This is not the beginning of a limit expression
         }
         next = bstr::lstrip(next, end);
         if (next >= end)
         {
            return nullptr;
         }
         if (is_signed_type)
         {
            int32_t lower_limit, upper_limit;
            const char* result = parse_i32(next, end, lower_limit);
            if (result > next)
            {
               next = result;
            }
            else
            {
               return nullptr;
            }
            next = bstr::lstrip(next, end);
            if (next >= end)
            {
               return nullptr;
            }
            if (*next++ != ',')
            {
               return nullptr;
            }
            next = bstr::lstrip(next, end);
            if (next >= end)
            {
               return nullptr;
            }
            result = parse_i32(next, end, upper_limit);
            if (result > next)
            {
               next = result;
            }
            else
            {
               return nullptr;
            }
            m_state->data_element->set_limits(lower_limit, upper_limit);
         }
         else
         {
            uint32_t lower_limit, upper_limit;
            const char* result = parse_u32(next, end, lower_limit);
            if (result > next)
            {
               next = result;
            }
            else
            {
               return nullptr;
            }
            next = bstr::lstrip(next, end);
            if (next >= end)
            {
               return nullptr;
            }
            if (*next++ != ',')
            {
               return nullptr;
            }
            next = bstr::lstrip(next, end);
            if (next >= end)
            {
               return nullptr;
            }
            result = parse_u32(next, end, upper_limit);
            if (result > next)
            {
               next = result;
            }
            else
            {
               return nullptr;
            }
            m_state->data_element->set_limits(lower_limit, upper_limit);
         }
         next = bstr::lstrip(next, end);
         if (next >= end)
         {
            return nullptr;
         }
         if (*next++ != ')')
         {
            return nullptr;
         }
         return next;
      }
      return begin;
   }

   const char* SignatureParser::parse_i32(const char* begin, const char* end, int32_t& v)
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

   const char* SignatureParser::parse_u32(const char* begin, const char* end, uint32_t& v)
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

   const char* SignatureParser::parse_type_reference(const char* begin, const char* end)
   {
      const char* next = begin;
      if (next < end)
      {
         if (*next++ != '[') return begin;
         next = bstr::lstrip(next, end);
         if (next >= end) return nullptr;
         char c = *next;
         if (c == '"')
         {
            auto result = bstr::match_pair(next, end, '"', '"', '\\');
            if (result > next)
            {
               std::string tmp{ next + 1, result };
               m_state->data_element->set_typeref(tmp.c_str());
               next = result + 1;
            }
            else
            {
               return nullptr;
            }
         }
         else
         {
            uint32_t type_id;
            auto result = parse_u32(next, end, type_id);
            if (result > next)
            {
               next = result;
               m_state->data_element->set_typeref(type_id);
            }
            else
            {
               return nullptr;
            }
         }
         next = bstr::lstrip(next, end);
         if (next >= end) return nullptr;
         if (*next++ != ']') return nullptr;
      }
      return next;
   }

   const char* SignatureParser::parse_array(const char* begin, const char* end)
   {
      const char* next = begin;
      if (next < end)
      {
         if (*next++ != '[') return begin;
         next = bstr::lstrip(next, end);
         if (next >= end) return nullptr;
         uint32_t array_len;
         auto result = parse_u32(next, end, array_len);
         if (result > next)
         {
            next = result;
            m_state->data_element->set_array_length(array_len);
         }
         else
         {
            return nullptr;
         }
         next = bstr::lstrip(next, end);
         if (next >= end) return nullptr;
         if (*next == '*')
         {
            m_state->data_element->set_dynamic_array();
            next = bstr::lstrip(next+1, end);
            if (next >= end) return nullptr;
         }
         if (*next++ != ']') return nullptr;
      }
      return next;
   }
}