#pragma once

#include <string>
#include "cpp-apx/types.h"
#include "cpp-apx/data_signature.h"
#include "cpp-apx/port_attribute.h"

namespace apx
{
   struct Port
   {
      Port() = delete;
      Port(apx::PortType type, std::string const& type_name) : port_type{ type }, name { type_name } {}

      apx::DataElement* get_data_element() { return dsg.element.get(); }
      apx::PortAttributes* get_attributes() { return attr.get(); }

      apx::PortType port_type;
      std::string name;
      apx::DataSignature dsg;
      std::unique_ptr <apx::PortAttributes> attr;
      port_id_t port_id{ INVALID_PORT_ID };
   };
}

