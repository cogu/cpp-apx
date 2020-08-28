#include "pch.h"
#include "port.h"

apx::error_t apx::Port::derive_types(const std::vector<std::unique_ptr<apx::DataType>>& type_list, const std::map<std::string, apx::DataType*>& type_map)
{
   auto data_element = get_data_element();
   if (data_element == nullptr)
   {
      return APX_NULL_PTR_ERROR;
   }   
   return derive_types_on_element(data_element, type_list, type_map);
}

apx::error_t apx::Port::derive_types_on_element(apx::DataElement *data_element, const std::vector<std::unique_ptr<apx::DataType>>& type_list, const std::map<std::string, apx::DataType*>& type_map)
{
   const auto type_code = data_element->get_type_code();
   if (type_code == apx::TypeCode::Record)
   {
      return APX_NOT_IMPLEMENTED_ERROR;
   }
   else if (type_code == apx::TypeCode::TypeRefId)
   {
      uint32_t type_index = data_element->get_typeref_id();
      uint32_t num_types = (uint32_t)type_list.size();
      if (type_index >= num_types)
      {
         return APX_INVALID_TYPE_REF_ERROR;
      }
      apx::DataType* data_type = type_list[type_index].get();
      if (data_type == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      data_element->set_typeref(data_type);      
   }
   else if (type_code == apx::TypeCode::TypeRefName)
   {
      return APX_NOT_IMPLEMENTED_ERROR;
   }
   else
   {
      //All other types are already derived
   }
   return APX_NO_ERROR;
}


