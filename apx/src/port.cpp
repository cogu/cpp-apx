#include <iostream>
#include <cassert>
#include "cpp-apx/port.h"

namespace apx
{
   apx::TypeAttributes* Port::get_referenced_type_attributes() const
   {
      auto const* data_element = get_data_element();
      assert(data_element != nullptr);
      auto const type_code = data_element->get_type_code();
      assert( (type_code != apx::TypeCode::TypeRefId) && (type_code != apx::TypeCode::TypeRefName) ); //User must have previously finalized the parse tree
      if (type_code == apx::TypeCode::TypeRefPtr)
      {
         auto const* data_type = data_element->get_typeref_ptr();
         assert(data_type != nullptr);
         if (data_type->has_attributes())
         {
            return data_type->get_attributes();
         }
      }
      return nullptr;
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
      dtl::DynamicValue derived_init_value;
      auto const* data_element = get_effective_data_element();
      if (data_element == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      if (data_element == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      apx::error_t result = APX_NO_ERROR;
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

   apx::error_t Port::derive_data_element(apx::DataElement*& data_element, apx::DataElement*& parent) const
   {
      apx::error_t retval = APX_NO_ERROR;
      assert(data_element != nullptr);
      apx::TypeCode type_code = data_element->get_type_code();
      if (type_code == apx::TypeCode::TypeRefPtr)
      {
         auto data_type = data_element->get_typeref_ptr();
         if (data_type != nullptr)
         {
            retval = data_type->derive_data_element(data_element, &parent);
            assert(data_element != nullptr);
         }
         else
         {
            retval = APX_NULL_PTR_ERROR;
         }
      }
      return retval;
   }

   apx::error_t Port::derive_data_element(apx::DataElement const*& data_element, apx::DataElement const*& parent) const
   {
      apx::error_t retval = APX_NO_ERROR;
      assert(data_element != nullptr);
      apx::TypeCode type_code = data_element->get_type_code();
      if (type_code == apx::TypeCode::TypeRefPtr)
      {
         auto data_type = data_element->get_typeref_ptr();
         if (data_type != nullptr)
         {
            retval = data_type->derive_data_element(data_element, &parent);
            assert(data_element != nullptr);
         }
         else
         {
            retval = APX_NULL_PTR_ERROR;
         }
      }
      return retval;
   }

   apx::error_t Port::flatten_data_element()
   {
      auto const* data_element = get_const_data_element();
      DataElement const* parent_element = nullptr;
      auto result = derive_data_element(data_element, parent_element);
      if (result != APX_NO_ERROR)
      {
         return result;
      }
      std::unique_ptr<DataElement> cloned_element = std::make_unique<DataElement>(*data_element);
      if (parent_element != nullptr)
      {
         if (parent_element->is_array() && cloned_element->is_array())
         {
            return APX_PARSE_ERROR; //Illegal in APX to create an array-reference to array-element.
         }
         else if (parent_element->is_array())
         {
            //Handle array-reference to data element
            cloned_element->set_array_length(parent_element->get_array_length());
            if (parent_element->is_dynamic_array())
            {
               cloned_element->set_dynamic_array();
            }
         }
      }
      dsg.effective_element = std::move(cloned_element);
      return APX_NO_ERROR;
   }
}