#include "pch.h"
#include "cpp-apx/vm.h"

using namespace apx::vm;

namespace apx_test
{
   TEST(VirtualMachine, ParseSimpleHeader)
   {
      Program const program{ 'A', 'P', 'X', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT8_SIZE };
      apx::VirtualMachine vm;
      ASSERT_EQ(vm.select_program(program), APX_NOT_IMPLEMENTED_ERROR);
   }
}
