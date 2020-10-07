#pragma once

#include <cstdint>
#include <vector>
#include <variant>
#include <optional>
#include <memory>
#include <map>
#include <utility>
#include <string>
#include "cpp-apx/types.h"
#include "cpp-apx/error.h"
#include "dtl/dtl.hpp"

namespace apx
{
   struct DataType;
   class DataElement
   {
   public:
      DataElement() = default;
      DataElement(apx::TypeCode type_code, std::uint32_t array_length = 0u);
      DataElement(apx::TypeCode type_code, std::int32_t lowerLimit, std::int32_t upperLimit, std::uint32_t array_length = 0u);
      DataElement(apx::TypeCode type_code, std::uint32_t lowerLimit, std::uint32_t upperLimit, std::uint32_t array_length = 0u);
      ~DataElement();
      apx::TypeCode get_type_code() const { return m_type_code; }
      std::uint32_t get_array_length() const { return m_array_len; }
      void set_array_length(std::uint32_t array_len) { m_array_len = array_len; }
      bool is_array() const { return m_array_len > 0u; }
      bool has_limits() const { return m_lower_limit.has_value();  }
      bool get_lower_limit(std::int32_t& limit) const noexcept;
      bool get_lower_limit(std::uint32_t& limit) const noexcept;
      bool get_upper_limit(std::int32_t& limit) const noexcept;
      bool get_upper_limit(std::uint32_t& limit) const noexcept;
      void set_limits(std::int32_t lower, std::int32_t upper) { m_lower_limit = lower; m_upper_limit = upper;  }
      void set_limits(std::uint32_t lower, std::uint32_t upper) { m_lower_limit = lower; m_upper_limit = upper; }
      std::pair<std::int32_t, std::int32_t> get_limits_signed() const;
      std::pair<std::uint32_t, std::uint32_t> get_limits_unsigned() const;
      void append(std::unique_ptr<DataElement> child_element);
      std::size_t get_num_child_elements() const { return m_elements->size(); }
      DataElement* get_child_at(std::size_t i) const { return m_elements->at(i).get(); }
      void set_typeref(std::uint32_t type_id) { m_type_ref = type_id; m_type_code = apx::TypeCode::TypeRefId; }
      void set_typeref(char const* name) { m_type_ref = name; m_type_code = apx::TypeCode::TypeRefName; }
      void set_typeref(DataType* element) { m_type_ref = element; m_type_code = apx::TypeCode::TypeRefPtr; }
      std::size_t get_typeref_index() const { return m_type_ref.index(); }
      std::uint32_t get_typeref_id() const { return std::get<std::uint32_t>(m_type_ref); };
      std::string const& get_typeref_name() const { return std::get<std::string>(m_type_ref); };
      DataType* get_typeref_ptr() const { return std::get<DataType*>(m_type_ref); };
      void set_name(std::string const&& name) { m_name = name; }
      std::string const& get_name() const { return m_name; }
      void set_dynamic_array() { m_dynamic_array = true; }
      bool is_dynamic_array() const { return m_dynamic_array; }
      apx::error_t derive_types_on_element(const std::vector<std::unique_ptr<apx::DataType>>& type_list, const std::map<std::string, apx::DataType*>& type_map);
      apx::TypeCode resolve_type_code();
      apx::error_t derive_proper_init_value(dtl::DynamicValue const &parsed_init_value, dtl::DynamicValue& derived_value);
      apx::error_t create_default_init_value(dtl::DynamicValue& derived_value);
      apx::error_t derive_hash_init_value(dtl::Array const* parsed_av, dtl::Hash*& derived_hv);


   protected:
      void init_element_vector();
      std::string m_name;
      apx::TypeCode m_type_code = apx::TypeCode::None;
      std::uint32_t m_array_len = 0u;
      bool m_dynamic_array = false;
      std::variant<std::uint32_t, std::string, DataType*> m_type_ref;    // uint32_t when m_type_code equals TypeRefId,
                                                                    // String when m_type_code equals TypeRefName,
                                                                    // Pointer when m_type_code equals TypeRefPtr
      std::unique_ptr<std::vector<std::unique_ptr<DataElement>>> m_elements = nullptr;
      std::optional<std::variant<std::int32_t, std::uint32_t>> m_lower_limit = {};
      std::optional <std::variant<std::int32_t, std::uint32_t>> m_upper_limit = {};
   };
}