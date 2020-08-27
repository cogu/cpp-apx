#include "pch.h"
#include "cpp-apx/port_instance.h"
#include <string>
#include <iostream>
#include <cstddef>

using namespace std::string_literals;


namespace apx_test
{
   TEST(PortInstance, CreateRequirePortInstance)
   {
      apx::PortInstance inst{ apx::PortType::RequirePort };
   }
}
