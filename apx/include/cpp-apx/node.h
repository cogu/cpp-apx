#pragma once

#include <vector>
#include <memory>
#include <map>
#include "cpp-apx/data_type.h"
#include "cpp-apx/port.h"
#include "cpp-apx/error.h"

namespace apx
{
   class Node
   {
   public:
      Node() {}
      Node(char const* name) : m_name{ name } {}
      Node(std::string const& name) : m_name{ name } {}
      apx::error_t append(DataType* data_type);
      apx::error_t append(Port* port);
      std::string const& get_name() const { return m_name; }
      std::size_t get_num_data_types() const { return m_data_types.size(); }
      std::size_t get_num_require_ports() const { return m_require_ports.size(); }
      std::size_t get_num_provide_ports() const { return m_provide_ports.size(); }
      apx::DataType* get_data_type(type_id_t id) const;
      apx::Port* get_require_port(port_id_t id) const;
      apx::Port* get_provide_port(port_id_t id) const;
      apx::DataType* get_last_data_type() const;
      apx::Port* get_last_require_port() const;
      apx::Port* get_last_provide_port() const;
      apx::error_t finalize();
      int get_last_error_line() const { return m_last_error_line; }
   protected:
      apx::error_t derive_types_on_ports(std::vector<std::unique_ptr<apx::Port>>& ports);
      apx::error_t derive_proper_init_values_on_ports(std::vector<std::unique_ptr<apx::Port>>& ports);
      apx::error_t expand_data_elements_on_ports(std::vector<std::unique_ptr<apx::Port>>& ports);

      bool m_is_finalized{ false };
      std::string m_name;
      std::vector<std::unique_ptr<apx::DataType>> m_data_types;
      std::vector<std::unique_ptr<apx::Port>> m_require_ports;
      std::vector<std::unique_ptr<apx::Port>> m_provide_ports;
      std::map<std::string, apx::DataType*> m_type_map;
      std::map<std::string, apx::Port*> m_port_map;
      int m_last_error_line{ -1 };
   };
}