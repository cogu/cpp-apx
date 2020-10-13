#pragma once

#include <memory>
#include <vector>
#include <string>
#include "cpp-apx/types.h"
#include "cpp-apx/port_instance.h"
#include "cpp-apx/parser.h"
#include "cpp-apx/compiler.h"
#include "cpp-apx/error.h"

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
      void init_port_memory(std::size_t num_provide_ports, std::size_t num_require_ports);
      apx::error_t create_provide_port(port_id_t port_id, std::string const& name, std::unique_ptr<apx::vm::Program> pack_program, std::uint32_t data_offset, std::uint32_t& data_size);
      apx::error_t create_require_port(port_id_t port_id, std::string const& name, std::unique_ptr<apx::vm::Program> pack_program, std::unique_ptr<apx::vm::Program> unpack_program, std::uint32_t data_offset, std::uint32_t& data_size);
   protected:
      std::string m_name;
      std::size_t m_num_provide_ports{ 0u };
      std::size_t m_num_require_ports{ 0u };
      std::unique_ptr<std::unique_ptr<PortInstance>[]> m_provide_ports;
      std::unique_ptr<std::unique_ptr<PortInstance>[]> m_require_ports;
   };
}
