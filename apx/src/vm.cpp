#include "cpp-apx/vm.h"

namespace apx
{
   apx::error_t VirtualMachine::select_program(apx::vm::Program const& program)
   {
      m_begin = m_next = program.data();
      m_end = m_begin + program.size();
      return parse_program_header();
   }

   apx::error_t VirtualMachine::parse_program_header()
   {
      return APX_NOT_IMPLEMENTED_ERROR;
   }

}
