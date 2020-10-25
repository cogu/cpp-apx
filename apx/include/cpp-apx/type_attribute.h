#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cstdint>
#include <stdexcept>
#include "cpp-apx/computation.h"

namespace apx
{
   enum class TypeAttributeType
   {
      None,
      ValueTable,
      RationalScaling
   };

   struct TypeAttributes
   {
      std::vector<std::unique_ptr<Computation>> computations;
   };

}