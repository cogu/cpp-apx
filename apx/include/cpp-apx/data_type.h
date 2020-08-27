#pragma once

#include <string>
#include "cpp-apx/data_signature.h"
#include "cpp-apx/type_attribute.h"

namespace apx
{
   struct DataType
   {
      DataType() = delete;
      DataType(std::string const &type_name):name{ type_name }{}
      apx::DataElement* get_data_element() { return dsg.element.get(); }
      apx::TypeAttributes* get_attributes() { return attr.get(); }

      std::string name;
      apx::DataSignature dsg;
      std::unique_ptr <apx::TypeAttributes> attr;
      type_id_t type_id{ INVALID_TYPE_ID };
   };
}