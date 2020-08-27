#pragma once

#include <cstdint>
#include <vector>
#include <variant>
#include <optional>
#include <memory>
#include <utility>
#include <string>
#include "cpp-apx/types.h"

namespace apx
{
   class DataElement
   {
   public:
      DataElement() = default;
      DataElement(apx::TypeCode type_code, uint32_t array_length = 0u);
      DataElement(apx::TypeCode type_code, int32_t lowerLimit, int32_t upperLimit, uint32_t array_length = 0u);
      DataElement(apx::TypeCode type_code, uint32_t lowerLimit, uint32_t upperLimit, uint32_t array_length = 0u);
      ~DataElement();
      apx::TypeCode get_type_code() const { return m_type_code; }
      uint32_t get_array_length() const { return m_array_len; }
      void set_array_length(uint32_t array_len) { m_array_len = array_len; }
      bool is_array() const { return m_array_len > 0u; }
      bool has_limits() const { return m_lower_limit.has_value();  }
      bool get_lower_limit(int32_t& limit) const noexcept;
      bool get_lower_limit(uint32_t& limit) const noexcept;
      bool get_upper_limit(int32_t& limit) const noexcept;
      bool get_upper_limit(uint32_t& limit) const noexcept;
      void set_limits(int32_t lower, int32_t upper) { m_lower_limit = lower; m_upper_limit = upper;  }
      void set_limits(uint32_t lower, uint32_t upper) { m_lower_limit = lower; m_upper_limit = upper; }
      std::pair<int32_t, int32_t> get_limits_signed();
      std::pair<uint32_t, uint32_t> get_limits_unsigned();
      void append(std::unique_ptr<DataElement> child_element);
      std::size_t get_num_child_elements() { return m_elements->size(); }
      DataElement* get_child_at(unsigned int i) const { return m_elements->at(i).get(); }
      void set_typeref(uint32_t type_id) { m_type_ref = type_id; }
      void set_typeref(char const* name) { m_type_ref = name; }
      void set_typeref(DataElement* element) { m_type_ref = element; }
      std::size_t get_typeref_index() const { return m_type_ref.index(); }
      uint32_t get_typeref_by_id() const { return std::get<uint32_t>(m_type_ref); };
      std::string const& get_typeref_by_name() const { return std::get<std::string>(m_type_ref); };
      DataElement* get_typeref_by_ptr() const { return std::get<DataElement*>(m_type_ref); };
      void set_name(std::string const&& name) { m_name = name; }
      std::string const& get_name() const { return m_name; }
      void set_dynamic_array() { m_dynamic_array = true; }
      bool is_dynamic_array() const { return m_dynamic_array; }

   protected:
      void init_element_vector();
      std::string m_name;
      apx::TypeCode m_type_code = apx::TypeCode::None;
      uint32_t m_array_len = 0u;
      bool m_dynamic_array = false;
      std::variant<uint32_t, std::string, DataElement*> m_type_ref; // uint32_t when m_type_code equals TypeRefId,
                                                                    // String when m_type_code equals TypeRefName,
                                                                    // Pointer when m_type_code equals TypeRefPtr
      std::unique_ptr<std::vector<std::unique_ptr<DataElement>>> m_elements = nullptr;
      std::optional<std::variant<int32_t, uint32_t>> m_lower_limit = {};
      std::optional <std::variant<int32_t, uint32_t>> m_upper_limit = {};
   };
}