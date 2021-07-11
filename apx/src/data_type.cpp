#include <cassert>
#include "cpp-apx/data_type.h"

apx::error_t apx::DataType::derive_data_element(apx::DataElement*& data_element, apx::DataElement** parent) const
{
   apx::error_t retval = APX_NO_ERROR;
   std::uint16_t reference_follow_count = 0u;
   apx::TypeCode type_code;
   data_element = get_data_element();
   do
   {
      type_code = data_element->get_type_code();
      assert((type_code != apx::TypeCode::TypeRefName) && (type_code != apx::TypeCode::TypeRefId));
      if (type_code == apx::TypeCode::TypeRefPtr)
      {
         auto* data_type = data_element->get_typeref_ptr();
         if (data_type == nullptr)
         {
            retval = APX_NULL_PTR_ERROR;
         }
         else
         {
            if (parent != nullptr) *parent = data_element;
            data_element = data_type->get_data_element();
            if (data_element == nullptr)
            {
               retval = APX_NULL_PTR_ERROR;
            }
            reference_follow_count++;
         }
      }
   } while ((retval == APX_NO_ERROR) &&
      (type_code == apx::TypeCode::TypeRefPtr) &&
      (reference_follow_count <= MAX_TYPE_REF_FOLLOW_COUNT));
   return retval;
}

apx::error_t apx::DataType::derive_data_element(apx::DataElement const*& data_element, apx::DataElement const** parent) const
{
   apx::error_t retval = APX_NO_ERROR;
   std::uint16_t reference_follow_count = 0u;
   apx::TypeCode type_code;
   data_element = get_data_element();

   do
   {
      type_code = data_element->get_type_code();
      assert((type_code != apx::TypeCode::TypeRefName) && (type_code != apx::TypeCode::TypeRefId));
      if (type_code == apx::TypeCode::TypeRefPtr)
      {
         auto* data_type = data_element->get_typeref_ptr();
         if (data_type == nullptr)
         {
            retval = APX_NULL_PTR_ERROR;
         }
         else
         {
            if (parent != nullptr) *parent = data_element;
            data_element = data_type->get_const_data_element();
            if (data_element == nullptr)
            {
               retval = APX_NULL_PTR_ERROR;
            }
            reference_follow_count++;
         }
      }
   } while ((retval == APX_NO_ERROR) &&
      (type_code == apx::TypeCode::TypeRefPtr) &&
      (reference_follow_count <= MAX_TYPE_REF_FOLLOW_COUNT));
   return retval;
}

apx::error_t apx::DataType::derive_types_on_element(const std::vector<std::unique_ptr<apx::DataType>>& type_list, const std::map<std::string, apx::DataType*>& type_map)
{
   auto data_element = dsg.element.get();
   if (data_element == nullptr)
   {
      return APX_NULL_PTR_ERROR;
   }
   return data_element->derive_types_on_element(type_list, type_map);
}
