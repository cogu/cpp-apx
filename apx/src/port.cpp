#include <iostream>
#include <cassert>
#include "cpp-apx/port.h"

namespace apx
{

   DataElement const* apx::Port::get_derived_data_element() const
   {
      return get_const_data_element();
   }

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
      auto data_element = get_data_element();
      dtl::DynamicValue derived_init_value;
      if (data_element == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      if (attr.get() != nullptr)
      {
         dtl::DynamicValue parsed_init_value = attr->get_shared_init_value();
         if (parsed_init_value.get() != nullptr)
         {
            assert(parsed_init_value.use_count() == 2);
            apx::error_t result = data_element->derive_proper_init_value(parsed_init_value, derived_init_value);
            if (result != APX_NO_ERROR)
            {
               return result;
            }
         }
      }
      if (proper_init_value.get() == nullptr)
      {
         apx::error_t result = data_element->create_default_init_value(derived_init_value);
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
}




