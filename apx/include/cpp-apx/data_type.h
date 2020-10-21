#pragma once

#include <string>
#include "cpp-apx/types.h"
#include "cpp-apx/data_signature.h"
#include "cpp-apx/type_attribute.h"

namespace apx
{
   struct DataType
   {
      DataType() = delete;
      DataType(std::string const& type_name, int32_t lineno) : name{ type_name }, line_number{ lineno }{}
      apx::DataElement* get_data_element() const { return dsg.element.get(); }
      apx::DataElement const* get_const_data_element() const { return dsg.element.get(); }
      apx::TypeAttributes* get_attributes() const { return attr.get(); }
      apx::error_t derive_data_element(apx::DataElement*& data_element) const;
      apx::error_t derive_data_element(apx::DataElement const*& data_element) const;
      apx::error_t derive_types_on_element(const std::vector<std::unique_ptr<apx::DataType>>& type_list, const std::map<std::string, apx::DataType*>& type_map);

      std::string name;
      apx::DataSignature dsg;
      std::unique_ptr <apx::TypeAttributes> attr;
      type_id_t type_id{ INVALID_TYPE_ID };
      int32_t line_number;
   };
}