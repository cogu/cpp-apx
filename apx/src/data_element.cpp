#include <cassert>
#include "cpp-apx/data_element.h"
#include "cpp-apx/data_type.h"
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

   std::pair<std::int32_t, std::int32_t>  DataElement::get_limits_i32() const
   {
      return std::make_pair(std::get<std::int32_t>(m_lower_limit.value()), std::get<std::int32_t>(m_upper_limit.value()));
   }

   std::pair<std::uint32_t, std::uint32_t>  DataElement::get_limits_u32() const
   {
      return std::make_pair(std::get<std::uint32_t>(m_lower_limit.value()), std::get<std::uint32_t>(m_upper_limit.value()));
   }

   std::pair<std::int64_t, std::int64_t>  DataElement::get_limits_i64() const
   {
      return std::make_pair(std::get<std::int64_t>(m_lower_limit.value()), std::get<std::int64_t>(m_upper_limit.value()));
   }

   std::pair<std::uint64_t, std::uint64_t>  DataElement::get_limits_u64() const
   {
      return std::make_pair(std::get<std::uint64_t>(m_lower_limit.value()), std::get<std::uint64_t>(m_upper_limit.value()));
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

   /*
   * Note: Calling this function before finalizing the node is not allowed
   */
   apx::TypeCode DataElement::resolve_type_code()
   {
      if ((m_type_code == apx::TypeCode::TypeRefId) || (m_type_code == apx::TypeCode::TypeRefName))
      {
         return apx::TypeCode::None;
      }
      else if (m_type_code == apx::TypeCode::TypeRefPtr)
      {
         apx::DataType* data_type = get_typeref_ptr();
         assert(data_type != nullptr);
         return data_type->resolve_type_code();
      }
      return m_type_code;
   }


   apx::error_t DataElement::derive_proper_init_value(dtl::DynamicValue const & parsed_init_value, dtl::DynamicValue& derived_value)
   {
      apx::TypeCode type_code = resolve_type_code();
      assert(  (type_code != apx::TypeCode::None) &&
               (type_code != apx::TypeCode::TypeRefId) &&
               (type_code != apx::TypeCode::TypeRefName) &&
               (type_code != apx::TypeCode::TypeRefPtr)
            );
      if (type_code == apx::TypeCode::Record)
      {
         if (is_array())
         {
            return APX_NOT_IMPLEMENTED_ERROR; //Array of records
         }
         if (parsed_init_value->dv_type() == dtl::ValueType::Array)
         {
            auto parsed_av = dynamic_cast<dtl::Array*>(parsed_init_value.get());
            assert(parsed_av != nullptr);
            dtl::Hash* derived_hv = nullptr;
            apx::error_t result = derive_hash_init_value(parsed_av, derived_hv);
            if (result == APX_NO_ERROR)
            {
               assert(derived_hv != nullptr);
               derived_value.reset(derived_hv);
            }
            else
            {
               assert(derived_hv == nullptr);
            }
         }
      }
      else if (is_array())
      {
         if (parsed_init_value->dv_type() == dtl::ValueType::Array)
         {
            auto parsed_av = dynamic_cast<dtl::Array*>(parsed_init_value.get());
            assert(parsed_av != nullptr);
            if (is_dynamic_array())
            {
               if (parsed_av->length() == 0u)
               {
                  derived_value = make_unique<dtl::Array>();
               }
               else
               {
                  return APX_INVALID_ATTRIBUTE_ERROR; //dynamic array only supports empty initializer list
               }
            }
            else
            {
               if (m_array_len == parsed_av->length())
               {
                  auto derived_av = new dtl::Array();
                  for (uint32_t i = 0; i < m_array_len; i++)
                  {
                     auto child_dv = parsed_av->at(i);
                     if (child_dv->dv_type() == dtl::ValueType::Scalar)
                     {
                        derived_av->push(child_dv);
                     }
                     else
                     {
                        delete derived_av;
                        return APX_VALUE_TYPE_ERROR;
                     }
                  }
                  derived_value.reset(derived_av);
               }
               else
               {
                  return APX_LENGTH_ERROR;
               }
            }
         }
         else
         {
            return APX_VALUE_TYPE_ERROR;
         }
      }
      else
      {
         if (parsed_init_value->dv_type() == dtl::ValueType::Scalar)
         {
            derived_value = parsed_init_value;
         }
         else
         {
            return APX_VALUE_TYPE_ERROR;
         }
      }
      return APX_NO_ERROR;
   }

   apx::error_t DataElement::create_default_init_value(dtl::DynamicValue& derived_value)
   {
      (void)derived_value;
      return APX_NO_ERROR;
   }

   apx::error_t DataElement::derive_hash_init_value(dtl::Array const* parsed_av, dtl::Hash*& derived_hv)
   {
      unique_ptr<dtl::Hash> hv = std::make_unique<dtl::Hash>();
      std::size_t num_children = get_num_child_elements();
      if (num_children != parsed_av->length())
      {
         return APX_LENGTH_ERROR;
      }
      for (std::size_t i = 0u; i < num_children; i++)
      {
         DataElement* child_element = get_child_at(i);
         dtl::DynamicValue const parsed_dv = parsed_av->at(i);
         dtl::DynamicValue derived_dv;
         apx::error_t result = child_element->derive_proper_init_value(parsed_dv, derived_dv);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         hv->insert(std::make_pair(child_element->get_name(), derived_dv));
      }
      derived_hv = hv.release();
      return APX_NO_ERROR;
   }


}
