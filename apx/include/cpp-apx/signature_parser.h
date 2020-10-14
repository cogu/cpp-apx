#pragma once

#include <string>
#include <stack>
#include <memory>
#include "cpp-apx/types.h"
#include "cpp-apx/error.h"
#include "cpp-apx/data_element.h"

namespace apx
{
   class SignatureParser
   {
      struct State
      {
         State() = default;
         State(bool record) :is_record{ record } {}
         bool is_record = false;
         std::unique_ptr<DataElement> data_element;
      };
   public:
      SignatureParser() : m_state{ nullptr }, m_last_error{ APX_NO_ERROR }, m_error_next{ nullptr } {};
      ~SignatureParser() { reset(); }
      const char* parse_data_signature(const char* begin, const char* end);
      apx::error_t get_last_error(const char*& error_next) { error_next = m_error_next;  return m_last_error; }
      DataElement* take_data_element() { return m_state == nullptr ? nullptr : m_state->data_element.release(); }
   protected:
      void reset();
      void set_error(apx::error_t error, const char* error_next = nullptr) { m_last_error = error; m_error_next = error_next; }
      const char* parse_data_element(const char* begin, const char* end);
      const char* parse_limits_i32(const char* begin, const char* end);
      const char* parse_limits_u32(const char* begin, const char* end);
      const char* parse_limits_i64(const char* begin, const char* end);
      const char* parse_limits_u64(const char* begin, const char* end);
      const char* parse_i32(const char* begin, const char* end, std::int32_t& v);
      const char* parse_u32(const char* begin, const char* end, std::uint32_t& v);
      const char* parse_i64(const char* begin, const char* end, std::int64_t& v);
      const char* parse_u64(const char* begin, const char* end, std::uint64_t& v);
      const char* parse_type_reference(const char* begin, const char* end);
      const char* parse_array(const char* begin, const char* end);

      std::stack<State*> m_stack;
      State* m_state;
      apx::error_t m_last_error;
      const char* m_error_next;
   };
}

