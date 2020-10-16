#include <iostream>
#include <cassert>
#include "cpp-apx/port.h"

namespace apx
{

   apx::error_t Port::derive_types(const std::vector<std::unique_ptr<apx::DataType>>& type_list, const std::map<std::string, apx::DataType*>& type_map)
   {
      auto data_element = get_data_element();
      if (data_element == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      return data_element->derive_types_on_element(type_list, type_map);
   }

   apx::error_t Port::derive_proper_init_value()
   {
      dtl::DynamicValue derived_init_value;
      auto* data_element = get_data_element();
      auto result = derive_data_element(data_element);
      if (result != APX_NO_ERROR)
      {
         return result;
      }
      if (data_element == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      if (attr.get() != nullptr)
      {
         dtl::DynamicValue parsed_init_value = attr->get_shared_init_value();
         if (parsed_init_value.get() != nullptr)
         {
            result = data_element->derive_proper_init_value(parsed_init_value, derived_init_value);
            if (result != APX_NO_ERROR)
            {
               return result;
            }
         }
      }
      if (proper_init_value.get() == nullptr)
      {
         result = data_element->create_default_init_value(derived_init_value);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
      }
      if (derived_init_value.get() != nullptr)
      {
         proper_init_value.swap(derived_init_value);
      }
      return APX_NO_ERROR;
   }

   bool Port::is_queued() const
   {
      if (auto attributes = get_attributes(); attributes != nullptr)
      {
         return attributes->queue_length > 0u;
      }
      return false;
   }

   bool Port::is_parameter() const
   {
      if (auto attributes = get_attributes(); attributes != nullptr)
      {
         return attributes->is_parameter;
      }
      return false;
   }

   std::uint32_t Port::get_queue_length() const
   {
      if (auto attributes = get_attributes(); attributes != nullptr)
      {
         return attributes->queue_length;
      }
      return 0u;
   }

   apx::error_t Port::derive_data_element(apx::DataElement*& data_element) const
   {
      apx::error_t retval = APX_NO_ERROR;
      assert(data_element != nullptr);
      apx::TypeCode type_code = data_element->get_type_code();
      if (type_code == apx::TypeCode::TypeRefPtr)
      {
         auto data_type = data_element->get_typeref_ptr();
         if (data_type != nullptr)
         {
            retval = data_type->derive_data_element(data_element);
            assert(data_element != nullptr);
         }
         else
         {
            retval = APX_NULL_PTR_ERROR;
         }
      }
      return retval;
   }

   apx::error_t Port::derive_data_element(apx::DataElement const*& data_element) const
   {
      apx::error_t retval = APX_NO_ERROR;
      assert(data_element != nullptr);
      apx::TypeCode type_code = data_element->get_type_code();
      if (type_code == apx::TypeCode::TypeRefPtr)
      {
         auto data_type = data_element->get_typeref_ptr();
         if (data_type != nullptr)
         {
            retval = data_type->derive_data_element(data_element);
            assert(data_element != nullptr);
         }
         else
         {
            retval = APX_NULL_PTR_ERROR;
         }
      }
      return retval;
   }
}




