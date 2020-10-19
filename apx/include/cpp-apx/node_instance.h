#pragma once

#include <vector>
#include <string>
#include "cpp-apx/types.h"
#include "cpp-apx/port_instance.h"
#include "cpp-apx/error.h"
#include "cpp-apx/vm.h"

namespace apx
{
   class NodeInstance
   {
   public:
      NodeInstance(){}
      ~NodeInstance();
      NodeInstance(std::string const& name) : m_name{ name } {}
      void set_name(std::string const& name) { m_name = name; }
      std::string const& get_name() { return m_name; }
      void alloc_port_instance_memory(std::size_t num_provide_ports, std::size_t num_require_ports);
      apx::error_t create_provide_port(port_id_t port_id, std::string const& name, std::unique_ptr<apx::vm::Program> pack_program, std::uint32_t data_offset, std::uint32_t& data_size);
      apx::error_t create_require_port(port_id_t port_id, std::string const& name, std::unique_ptr<apx::vm::Program> pack_program, std::unique_ptr<apx::vm::Program> unpack_program, std::uint32_t data_offset, std::uint32_t& data_size);
      std::size_t get_num_provide_ports(){ return m_num_provide_ports; }
      std::size_t get_num_require_ports(){ return m_num_require_ports; }
      std::size_t get_provide_port_init_data_size() { return m_provide_port_init_data_size; }
      std::size_t get_require_port_init_data_size() { return m_require_port_init_data_size; }
      std::uint8_t const* get_provide_port_init_data() { return m_provide_port_init_data; }
      std::uint8_t const* get_require_port_init_data() { return m_require_port_init_data; }
      apx::error_t create_port_init_data_memory(std::uint8_t*& provide_port_data, std::size_t& provide_port_data_size,
         std::uint8_t*& require_port_data, std::size_t& require_port_data_size);
      PortInstance* get_provide_port(std::size_t port_id);
      PortInstance* get_require_port(std::size_t port_id);
   protected:
      std::string m_name;
      std::size_t m_num_provide_ports{ 0u };
      std::size_t m_num_require_ports{ 0u };
      PortInstance** m_provide_ports{ nullptr };
      PortInstance** m_require_ports{ nullptr };
      std::uint8_t* m_provide_port_init_data{ nullptr };
      std::uint8_t* m_require_port_init_data{ nullptr };
      std::size_t m_provide_port_init_data_size{ 0u };
      std::size_t m_require_port_init_data_size{ 0u };

      apx::error_t calc_init_data_size(PortInstance **port_list, std::size_t num_ports, std::size_t & total_size);
   };
}
