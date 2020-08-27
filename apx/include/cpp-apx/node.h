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
      Node(const char* name) : m_name{ name } {}
      Node(const std::string& name) : m_name{ name } {}
      apx::error_t append(apx::DataType* data_type);
      apx::error_t append(apx::Port* port);
      std::string& get_name() { return m_name; }
      std::size_t get_num_data_types() { return m_data_types.size(); }
      std::size_t get_num_require_ports() { return m_require_ports.size(); }
      std::size_t get_num_provide_ports() { return m_provide_ports.size(); }      
      apx::DataType* get_data_type(apx::type_id_t id);
      apx::Port* get_require_port(apx::port_id_t id);
      apx::Port* get_provide_port(apx::port_id_t id);
      apx::DataType* get_last_data_type();      
      apx::Port* get_last_require_port();
      apx::Port* get_last_provide_port();
   protected:
      std::string m_name;
      std::vector<std::unique_ptr<apx::DataType>> m_data_types;
      std::vector<std::unique_ptr<apx::Port>> m_require_ports;
      std::vector<std::unique_ptr<apx::Port>> m_provide_ports;
      std::map<std::string, apx::DataType*> m_type_map;
      std::map<std::string, apx::Port*> m_port_map;
   };
}