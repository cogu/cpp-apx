#include "cpp-apx\node_instance.h"

namespace apx
{
   NodeInstance::~NodeInstance()
   {
   }

   void NodeInstance::init_port_memory(std::size_t num_provide_ports, std::size_t num_require_ports)
   {
      m_num_provide_ports = num_provide_ports;
      m_num_require_ports = num_require_ports;
      if (num_provide_ports > 0u)
      {
         m_provide_ports.reset(new std::unique_ptr<PortInstance>[num_provide_ports]);
      }
      if (num_require_ports > 0u)
      {
         m_require_ports.reset(new std::unique_ptr<PortInstance>[num_require_ports]);
      }
   }
   apx::error_t NodeInstance::create_provide_port(port_id_t port_id, std::string const& name, std::unique_ptr<apx::vm::Program> pack_program, std::uint32_t data_offset, std::uint32_t& data_size)
   {
      if (port_id < m_num_provide_ports)
      {
         m_provide_ports[port_id] = std::make_unique<PortInstance>(PortType::ProvidePort, port_id, name, pack_program.release(), nullptr);
         return m_provide_ports[port_id]->derive_properties(data_offset, data_size);
      }
      return APX_INVALID_ARGUMENT_ERROR;
   }

   apx::error_t NodeInstance::create_require_port(port_id_t port_id, std::string const& name, std::unique_ptr<apx::vm::Program> pack_program, std::unique_ptr<apx::vm::Program> unpack_program, std::uint32_t data_offset, std::uint32_t& data_size)
   {
      if (port_id < m_num_require_ports)
      {
         m_require_ports[port_id] = std::make_unique<PortInstance>(PortType::RequirePort, port_id, name, pack_program.release(), unpack_program.release());
         return m_require_ports[port_id]->derive_properties(data_offset, data_size);
      }
      return APX_INVALID_ARGUMENT_ERROR;
   }


}

