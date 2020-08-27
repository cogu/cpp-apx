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
}
