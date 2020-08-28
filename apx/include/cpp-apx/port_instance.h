#pragma once
#include <memory>
#include <cstddef>
#include <vector>
#include <cstdint>
#include "cpp-apx/types.h"
#include "cpp-apx/data_element.h"

namespace apx
{
   struct PortInstance
   {
      PortInstance() = delete;
      PortInstance(apx::PortType pt) : port_type{ pt } {}

      apx::PortType port_type;
      apx::ArrayType queue_len_type = apx::ArrayType::None;
      bool is_dynamic = false;
      apx::port_id_t port_id = INVALID_PORT_ID;
      size_t data_offset = 0u;
      size_t data_size = 0u;
      std::vector<uint8_t> pack_program;
      std::vector<uint8_t> unpack_program;
      std::unique_ptr<apx::DataElement> data_element;
      std::string data_signature;
   };
}