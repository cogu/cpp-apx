#include <cassert>
#include "cpp-apx/data_element.h"
//DEBUG ONLY
#include <iostream>

using namespace std;

namespace apx
{

   DataElement::DataElement(apx::TypeCode type_code, uint32_t array_length) :
      m_type_code{ type_code }, m_array_len { array_length }
   {
      init_element_vector();
   }

   DataElement::DataElement(apx::TypeCode type_code, int32_t lowerLimit, int32_t upperLimit, uint32_t array_length) :
      m_type_code{ type_code }, m_array_len{ array_length }, m_lower_limit{ lowerLimit }, m_upper_limit{ upperLimit }
   {
      init_element_vector();
   }

   DataElement::DataElement(apx::TypeCode type_code, uint32_t lowerLimit, uint32_t upperLimit, uint32_t array_length) :
      m_type_code{ type_code }, m_array_len{ array_length }, m_lower_limit{ lowerLimit }, m_upper_limit{ upperLimit }
   {
      init_element_vector();
   }

   DataElement::~DataElement()
   {
      //std::cout << "Destroying" << (int) m_type_code << std::endl;
   }

   bool DataElement::get_lower_limit(int32_t& limit) const noexcept
   {
      if ( m_lower_limit.has_value() && std::holds_alternative<int32_t>(m_lower_limit.value()) )
      {
         limit = std::get<int32_t>(m_lower_limit.value());
         return true;
      }
      return false;
   }

   bool DataElement::get_lower_limit(uint32_t& limit) const noexcept
   {
      if (m_lower_limit.has_value() && std::holds_alternative<uint32_t>(m_lower_limit.value()))
      {
         limit = std::get<uint32_t>(m_lower_limit.value());
         return true;
      }
      return false;
   }

   bool DataElement::get_upper_limit(int32_t& limit) const noexcept
   {
      if (m_upper_limit.has_value() && std::holds_alternative<int32_t>(m_upper_limit.value()))
      {
         limit = std::get<int32_t>(m_upper_limit.value());
         return true;
      }
      return false;
   }

   bool DataElement::get_upper_limit(uint32_t& limit) const noexcept
   {
      if (m_upper_limit.has_value() && holds_alternative<int32_t>(m_upper_limit.value()))
      {
         limit = std::get<uint32_t>(m_upper_limit.value());
         return true;
      }
      return false;
   }

   std::pair<int32_t, int32_t>  DataElement::get_limits_signed()
   {
      return std::make_pair(std::get<int32_t>(m_lower_limit.value()), std::get<int32_t>(m_upper_limit.value()));
   }

   std::pair<uint32_t, uint32_t>  DataElement::get_limits_unsigned()
   {
      return std::make_pair(std::get<uint32_t>(m_lower_limit.value()), std::get<uint32_t>(m_upper_limit.value()));
   }

   void DataElement::append(std::unique_ptr<DataElement> child_element)
   {
      m_elements->push_back(std::forward<std::unique_ptr<DataElement>>(child_element));
   }

   void DataElement::init_element_vector()
   {
      if (m_type_code == apx::TypeCode::Record)
      {
         m_elements = std::make_unique<std::vector<std::unique_ptr<DataElement>>>();
      }
   }

   apx::error_t apx::DataElement::derive_types_on_element(const std::vector<std::unique_ptr<apx::DataType>>& type_list, const std::map<std::string, apx::DataType*>& type_map)
   {
      const auto type_code = m_type_code;
      if (type_code == apx::TypeCode::Record)
      {
         std::size_t num_elements = get_num_child_elements();
         for (std::size_t i = 0u; i < num_elements; i++)
         {
            auto child_element = get_child_at(i);
            if (child_element == nullptr)
            {
               return APX_NULL_PTR_ERROR;
            }
            auto result = child_element->derive_types_on_element(type_list, type_map);
            if (result != APX_NO_ERROR)
            {
               return result;
            }
         }
      }
      else if (type_code == apx::TypeCode::TypeRefId)
      {
         uint32_t type_index = get_typeref_id();
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
         set_typeref(data_type);
      }
      else if (type_code == apx::TypeCode::TypeRefName)
      {
         auto type_name = get_typeref_name();
         auto it = type_map.find(type_name);
         if (it != type_map.end())
         {
            set_typeref(it->second);
         }
         else
         {
            return APX_INVALID_TYPE_REF_ERROR;
         }
      }
      else
      {
         //All other types are already derived
      }
      return APX_NO_ERROR;
   }
}
