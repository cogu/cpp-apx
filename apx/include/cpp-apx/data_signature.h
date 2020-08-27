#pragma once

#include <memory>
#include "cpp-apx/data_element.h"

namespace apx
{
   struct DataSignature
   {
      std::unique_ptr<apx::DataElement> element;
   };
}