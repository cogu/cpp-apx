#include <iostream>
#include "cpp-apx/port.h"


apx::error_t apx::Port::derive_types(const std::vector<std::unique_ptr<apx::DataType>>& type_list, const std::map<std::string, apx::DataType*>& type_map)
{
   auto data_element = get_data_element();
   if (data_element == nullptr)
   {
      return APX_NULL_PTR_ERROR;
   }
   return data_element->derive_types_on_element(type_list, type_map);
}




