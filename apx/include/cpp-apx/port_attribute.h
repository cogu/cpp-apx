#pragma once

#include <memory>
#include <cstdint>
#include "dtl/dtl.hpp"

namespace apx
{
   enum class PortAttributeType {
      None,
      InitValue,
      Parameter,
      QueueLength
   };
   struct PortAttributes
   {
      bool has_init_value() { return init_value.get() != nullptr; }
      dtl::Value* get_init_value() { return init_value.get(); }
      dtl::DynamicValue get_shared_init_value() { return init_value; }

      dtl::DynamicValue init_value{ nullptr };
      bool is_parameter = false;
      std::uint32_t queue_length = 0u;
   };
}