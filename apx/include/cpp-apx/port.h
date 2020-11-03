#pragma once

#include <string>
#include <vector>
#include <map>
#include "cpp-apx/types.h"
#include "cpp-apx/data_signature.h"
#include "cpp-apx/data_type.h"
#include "cpp-apx/port_attribute.h"
#include "cpp-apx/error.h"
#include "dtl/dtl.hpp"

namespace apx
{
   struct Port
   {
      Port() = delete;
      Port(apx::PortType type, std::string const& type_name, std::int32_t lineno) : port_type{ type }, name{ type_name }, line_number{ lineno } {}

      apx::DataElement* get_data_element() const { return dsg.element.get(); }
      apx::DataElement const *get_const_data_element() const { return dsg.element.get(); }
      apx::DataElement const* get_effective_data_element() const { return dsg.effective_element.get(); }

      apx::PortAttributes* get_attributes() const { return attr.get(); }
      apx::TypeAttributes* get_referenced_type_attributes() const; //Will return nullptr if this port isn't referencing a data type
      apx::error_t derive_types(const std::vector<std::unique_ptr<apx::DataType>>& type_list, const std::map<std::string, apx::DataType*>& type_map);
      apx::error_t derive_proper_init_value();
      bool is_queued() const;
      bool is_parameter() const;
      std::uint32_t get_queue_length() const;
      apx::error_t flatten_data_element();

      apx::PortType port_type;
      std::string name;
      apx::DataSignature dsg;
      std::unique_ptr <apx::PortAttributes> attr;
      std::int32_t line_number;
      port_id_t port_id{ INVALID_PORT_ID };
      dtl::DynamicValue proper_init_value{ nullptr };
   protected:

      apx::error_t derive_data_element(apx::DataElement*& data_element, apx::DataElement*& parent) const;
      apx::error_t derive_data_element(apx::DataElement const*& data_element, apx::DataElement const*& parent) const;

   };
}

