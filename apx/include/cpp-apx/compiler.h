#pragma once

#include "cpp-apx/program.h"
#include "cpp-apx/port.h"
#include <cstddef>
#include <memory>
#include <stack>

namespace apx
{

   class Compiler
   {
   public:
      Compiler() {}
      std::unique_ptr<apx::vm::Program> compile_port(apx::Port const *port, apx::ProgramType program_type, apx::error_t& error_code);
   protected:
      void reset_internal_state();


      void set_error(apx::error_t error) { m_last_error = error; }
      void set_error(apx::error_t& storage, apx::error_t error) { storage = error; }
      apx::error_t compile_data_element(apx::DataElement const *data_element, apx::ProgramType program_type, std::uint32_t& elem_size);
      apx::error_t compile_limit_instruction(apx::DataElement const* data_element, bool is_signed_type, bool is_64_bit_type, bool is_array, std::uint8_t limit_variant);
      apx::error_t compile_limit_values(std::uint8_t limit_variant, std::int32_t lower_limit, std::int32_t upper_limit);
      apx::error_t compile_limit_values(std::uint8_t limit_variant, std::uint32_t lower_limit, std::uint32_t upper_limit);
      apx::error_t compile_limit_values(std::uint8_t limit_variant, std::int64_t lower_limit, std::int64_t upper_limit);
      apx::error_t compile_limit_values(std::uint8_t limit_variant, std::uint64_t lower_limit, std::uint64_t upper_limit);
      apx::error_t compile_array_size_instruction(std::uint32_t array_size, bool is_dynamic_array);
      apx::error_t compile_record_fields(apx::DataElement const* data_element, apx::ProgramType program_type, std::uint32_t &record_size);
      apx::error_t compile_record_select_instruction(apx::DataElement const* data_element, bool is_last_field);
      apx::error_t compile_array_next_instruction();

      std::unique_ptr<apx::vm::Program> m_program;
      apx::error_t m_last_error{ APX_NO_ERROR };
      bool m_is_dynamic = false;
   };
}