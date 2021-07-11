#include <cassert>
#include "cpp-apx/data_element.h"
#include "cpp-apx/data_type.h"
//DEBUG ONLY
#include <iostream>

using namespace std;

namespace apx
{
   constexpr std::size_t INT32_INDEX  = 0u;
   constexpr std::size_t UINT32_INDEX = 1u;
   constexpr std::size_t INT64_INDEX  = 2u;
   constexpr std::size_t UINT64_INDEX = 3u;

   DataElement::DataElement(DataElement const& other) :
      m_name{other.m_name},
      m_type_code{other.m_type_code},
      m_array_len{other.m_array_len},
      m_dynamic_array{other.m_dynamic_array}
   {
      m_type_ref = other.m_type_ref;
      if (other.has_limits())
      {
         m_lower_limit = other.m_lower_limit;
         m_upper_limit = other.m_upper_limit;
      }
      if ( (other.m_elements.get() != nullptr) && (other.m_elements->size() > 0) )
      {
         m_elements = std::make_unique<std::vector<std::unique_ptr<DataElement>>>();
         for (auto& it : *other.m_elements)
         {
            assert(it.get() != nullptr);
            m_elements->push_back(std::make_unique<DataElement>(*it));
         }
      }
   }

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
         auto result = data_type->derive_types_on_element(type_list, type_map);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         set_typeref(data_type);
      }
      else if (type_code == apx::TypeCode::TypeRefName)
      {
         auto type_name = get_typeref_name();
         auto it = type_map.find(type_name);
         if (it != type_map.end())
         {
            apx::DataType* data_type = it->second;
            if (data_type == nullptr)
            {
               return APX_NULL_PTR_ERROR;
            }
            auto result = data_type->derive_types_on_element(type_list, type_map);
            if (result != APX_NO_ERROR)
            {
               return result;
            }
            set_typeref(data_type);
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

   apx::error_t DataElement::derive_proper_init_value(dtl::DynamicValue const & parsed_init_value, dtl::DynamicValue& derived_value) const
   {
      apx::TypeCode type_code = get_type_code();
      assert(  (type_code != apx::TypeCode::None) &&
               (type_code != apx::TypeCode::TypeRefId) &&
               (type_code != apx::TypeCode::TypeRefName) &&
               (type_code != apx::TypeCode::TypeRefPtr)
            );
      if (type_code == apx::TypeCode::Record)
      {
         if (is_array())
         {
            if (parsed_init_value->dv_type() == dtl::ValueType::Array)
            {
               auto parsed_av = dynamic_cast<dtl::Array*>(parsed_init_value.get());
               if (is_dynamic_array())
               {
                  if (parsed_av->length() != 0)
                  {
                     return APX_NOT_IMPLEMENTED_ERROR;
                  }
                  else
                  {
                     derived_value = dtl::make_av();
                  }
               }
               else
               {
                  auto derived_av = dtl::make_av();
                  if (parsed_av->length() != m_array_len)
                  {
                     return APX_VALUE_LENGTH_ERROR;
                  }
                  for (std::uint32_t i = 0u; i < m_array_len; i++)
                  {
                     auto const parsed_child_dv = parsed_av->at(i);
                     if (parsed_child_dv->dv_type() == dtl::ValueType::Array)
                     {
                        auto parsed_child_av = dynamic_cast<dtl::Array*>(parsed_child_dv.get());
                        assert(parsed_child_av != nullptr);
                        dtl::Hash* derived_hv = nullptr;
                        apx::error_t result = derive_hash_init_value(parsed_child_av, derived_hv);
                        if (result == APX_NO_ERROR)
                        {
                           assert(derived_hv != nullptr);
                           derived_av->push(std::shared_ptr<dtl::Value>(derived_hv));
                        }
                        else
                        {
                           return result;
                        }
                     }
                  }
                  derived_value = std::dynamic_pointer_cast<dtl::Value>(derived_av);
                  derived_av.reset();
               }
            }
            else
            {
               return APX_VALUE_TYPE_ERROR;
            }
         }
         else if (parsed_init_value->dv_type() == dtl::ValueType::Array)
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
         else
         {
            return APX_UNSUPPORTED_ERROR;
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
                  return APX_VALUE_LENGTH_ERROR;
               }
            }
         }
         else if ( (parsed_init_value->dv_type() == dtl::ValueType::Scalar) &&
            ((type_code == apx::TypeCode::Char) || (type_code == apx::TypeCode::Char8)) )
         {
            auto parsed_sv = dynamic_cast<dtl::Scalar*>(parsed_init_value.get());
            assert(parsed_sv != nullptr);
            if (parsed_sv->sv_type() == dtl::ScalarType::String)
            {
               derived_value = parsed_init_value;
            }
            else
            {
               return APX_VALUE_TYPE_ERROR;
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

   apx::error_t DataElement::create_default_init_value(dtl::DynamicValue& derived_value) const
   {
      (void)derived_value;
      return APX_NO_ERROR;
   }

   apx::error_t DataElement::derive_hash_init_value(dtl::Array const* parsed_av, dtl::Hash*& derived_hv) const
   {
      unique_ptr<dtl::Hash> hv = std::make_unique<dtl::Hash>();
      std::size_t num_children = get_num_child_elements();
      if (num_children != parsed_av->length())
      {
         return APX_VALUE_LENGTH_ERROR;
      }
      for (std::size_t i = 0u; i < num_children; i++)
      {
         DataElement* child_element;
         DataElement* derived_element;
         child_element = derived_element = get_child_at(i);
         assert(child_element != nullptr);
         apx::error_t result = derive_data_element(derived_element, nullptr);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         assert(derived_element != nullptr);
         dtl::DynamicValue const parsed_dv = parsed_av->at(i);
         dtl::DynamicValue derived_dv;
         result = derived_element->derive_proper_init_value(parsed_dv, derived_dv);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         hv->insert(std::make_pair(child_element->get_name(), derived_dv));
      }
      derived_hv = hv.release();
      return APX_NO_ERROR;
   }

   apx::error_t DataElement::derive_data_element(apx::DataElement*& data_element, apx::DataElement** parent) const
   {
      apx::error_t retval = APX_NO_ERROR;
      assert(data_element != nullptr);
      apx::TypeCode type_code = data_element->get_type_code();
      assert((type_code != apx::TypeCode::TypeRefId) && (type_code != apx::TypeCode::TypeRefName));
      if (type_code == apx::TypeCode::TypeRefPtr)
      {
         auto data_type = data_element->get_typeref_ptr();
         if (data_type != nullptr)
         {
            retval = data_type->derive_data_element(data_element, parent);
            assert(data_element != nullptr);
         }
         else
         {
            retval = APX_NULL_PTR_ERROR;
         }
      }
      return retval;
   }

   apx::error_t DataElement::derive_data_element(apx::DataElement const*& data_element, apx::DataElement const** parent) const
   {
      apx::error_t retval = APX_NO_ERROR;
      assert(data_element != nullptr);
      apx::TypeCode type_code = data_element->get_type_code();
      assert((type_code != apx::TypeCode::TypeRefId) && (type_code != apx::TypeCode::TypeRefName));
      if (type_code == apx::TypeCode::TypeRefPtr)
      {
         auto data_type = data_element->get_typeref_ptr();
         if (data_type != nullptr)
         {
            retval = data_type->derive_data_element(data_element, parent);
            assert(data_element != nullptr);
         }
         else
         {
            retval = APX_NULL_PTR_ERROR;
         }
      }
      return retval;
   }

   std::string DataElement::to_string() const
   {
      std::string retval;
      if (m_type_code == TypeCode::Record)
      {
         retval.push_back('{');
         for (auto const& it : *m_elements)
         {
            retval.push_back('"');
            retval.append(it->get_name());
            retval.push_back('"');
            retval.append(it->to_string());
         }
         retval.push_back('}');
      }
      else
      {
         char type_code = '\0';
         switch (m_type_code)
         {
         case TypeCode::UInt8:
            type_code = 'C';
            break;
         case TypeCode::UInt16:
            type_code = 'S';
            break;
         case TypeCode::UInt32:
            type_code = 'L';
            break;
         case TypeCode::UInt64:
            type_code = 'Q';
            break;
         case TypeCode::Int8:
            type_code = 'c';
            break;
         case TypeCode::Int16:
            type_code = 's';
            break;
         case TypeCode::Int32:
            type_code = 'l';
            break;
         case TypeCode::Int64:
            type_code = 'q';
            break;
         case TypeCode::Char:
            type_code = 'a';
            break;
         case TypeCode::Char8:
            type_code = 'A';
            break;
         case TypeCode::Char16:
            type_code = 'u';
            break;
         case TypeCode::Char32:
            type_code = 'U';
            break;
         case TypeCode::Bool:
            type_code = 'b';
            break;
         case TypeCode::Byte:
            type_code = 'B';
            break;
         default:
            throw std::bad_typeid();
         }
         retval.push_back(type_code);
         if (has_limits())
         {
            retval.append(limit_to_string());
         }
      }
      if (is_array())
      {
         retval.append(array_to_string());
      }
      return retval;
   }

   bool DataElement::operator==(DataElement const& other) const
   {
      if (m_type_code != other.m_type_code)
      {
         return false;
      }
      if (has_limits())
      {
         if (other.has_limits())
         {
            is_limits_equal(other);
         }
         else
         {
            return false;
         }
      }
      else if (other.has_limits())
      {
         return false;
      }
      if (is_array())
      {
         if (other.is_array())
         {
            is_array_equal(other);
         }
         else
         {
            return false;
         }
      }
      else if (other.is_array())
      {
         return false;
      }
      switch (m_type_code)
      {
      case TypeCode::Record:
         if (!is_elements_equal(other))
         {
            return false;
         }
         break;
      case TypeCode::TypeRefId:
         if (!is_typeref_id_equal(other))
         {
            return false;
         }
         break;
      case TypeCode::TypeRefName:
         if (!is_typeref_name_equal(other))
         {
            return false;
         }
         break;
      case TypeCode::TypeRefPtr:
         if (!is_typeref_ptr_equal(other))
         {
            return false;
         }
         break;
      default:
         break;
      }
      return true;
   }

   void DataElement::init_element_vector()
   {
      if (m_type_code == apx::TypeCode::Record)
      {
         m_elements = std::make_unique<std::vector<std::unique_ptr<DataElement>>>();
      }
   }

   std::string DataElement::limit_to_string() const
   {
      std::string retval{ '(' };
      switch (m_lower_limit->index())
      {
      case INT32_INDEX:
         retval.append(std::to_string(std::get<std::int32_t>(m_lower_limit.value())));
         retval.push_back(',');
         retval.append(std::to_string(std::get<std::int32_t>(m_upper_limit.value())));
         break;
      case UINT32_INDEX:
         retval.append(std::to_string(std::get<std::uint32_t>(m_lower_limit.value())));
         retval.push_back(',');
         retval.append(std::to_string(std::get<std::uint32_t>(m_upper_limit.value())));
         break;
      case INT64_INDEX:
         retval.append(std::to_string(std::get<std::int64_t>(m_lower_limit.value())));
         retval.push_back(',');
         retval.append(std::to_string(std::get<std::int64_t>(m_upper_limit.value())));
         break;
      case UINT64_INDEX:
         retval.append(std::to_string(std::get<std::uint64_t>(m_lower_limit.value())));
         retval.push_back(',');
         retval.append(std::to_string(std::get<std::uint64_t>(m_upper_limit.value())));
         break;
      default:
         throw std::bad_typeid();
      }
      retval.push_back(')');
      return retval;
   }

   std::string DataElement::array_to_string() const
   {
      std::string retval{ '[' };
      retval.append(std::to_string(m_array_len));
      if (m_dynamic_array)
      {
         retval.push_back('*');
      }
      retval.push_back(']');
      return retval;
   }

   bool DataElement::is_limits_equal(DataElement const& other) const
   {
      if (m_lower_limit->index() != other.m_lower_limit->index())
      {
         return false;
      }
      switch (m_lower_limit->index())
      {
      case INT32_INDEX:
         if ((std::get<std::int32_t>(m_lower_limit.value()) != std::get<std::int32_t>(other.m_lower_limit.value())) ||
            (std::get<std::int32_t>(m_upper_limit.value()) != std::get<std::int32_t>(other.m_upper_limit.value())))
            return false;
         break;
      case UINT32_INDEX:
         if ((std::get<std::uint32_t>(m_lower_limit.value()) != std::get<std::uint32_t>(other.m_lower_limit.value())) ||
            (std::get<std::uint32_t>(m_upper_limit.value()) != std::get<std::uint32_t>(other.m_upper_limit.value())))
            return false;
         break;
      case INT64_INDEX:
         if ((std::get<std::int64_t>(m_lower_limit.value()) != std::get<std::int64_t>(other.m_lower_limit.value())) ||
            (std::get<std::int64_t>(m_upper_limit.value()) != std::get<std::int64_t>(other.m_upper_limit.value())))
            return false;
         break;
      case UINT64_INDEX:
         if ((std::get<std::uint64_t>(m_lower_limit.value()) != std::get<std::uint64_t>(other.m_lower_limit.value())) ||
            (std::get<std::uint64_t>(m_upper_limit.value()) != std::get<std::uint64_t>(other.m_upper_limit.value())))
            return false;
         break;
      default:
         throw std::bad_typeid();
      }
      return true;
   }

   bool DataElement::is_array_equal(DataElement const& other) const
   {
      if ((m_array_len != other.m_array_len) || m_dynamic_array != other.m_dynamic_array)
      {
         return false;
      }
      return true;
   }

   bool DataElement::is_elements_equal(DataElement const& other) const
   {
      if ( (m_elements.get() == nullptr) || (other.m_elements.get() == nullptr) )
      {
         return false;
      }

      if (m_elements->size() != other.m_elements->size())
      {
         return false;
      }
      std::size_t const num_elements = m_elements->size();
      for (std::size_t i=0; i < num_elements; i++)
      {
         auto const &my_child = m_elements->at(i);
         auto const &other_child = other.m_elements->at(i);
         if ((my_child.get() == nullptr) || (other_child.get() == nullptr))
         {
            return false;
         }
         if ( !(*my_child == *other_child) )
         {
            return false;
         }
      }
      return true;
   }
}
