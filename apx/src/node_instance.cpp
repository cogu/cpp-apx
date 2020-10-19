#include <cassert>
#include "cpp-apx\node_instance.h"

namespace apx
{
   NodeInstance::~NodeInstance()
   {
      if (m_num_provide_ports > 0u)
      {
         for (std::size_t i = 0; i < m_num_provide_ports; i++)
         {
            delete m_provide_ports[i];
         }
         delete[] m_provide_ports;
      }
      if (m_num_require_ports > 0u)
      {
         for (std::size_t i = 0; i < m_num_require_ports; i++)
         {
            delete m_require_ports[i];
         }
         delete[] m_require_ports;
      }
      if (m_provide_port_init_data != nullptr)
      {
         delete[] m_provide_port_init_data;
      }
      if (m_require_port_init_data != nullptr)
      {
         delete[] m_require_port_init_data;
      }
   }

   void NodeInstance::alloc_port_instance_memory(std::size_t num_provide_ports, std::size_t num_require_ports)
   {
      m_num_provide_ports = num_provide_ports;
      m_num_require_ports = num_require_ports;
      if (num_provide_ports > 0u)
      {
         m_provide_ports = new PortInstance*[num_provide_ports];
      }
      if (num_require_ports > 0u)
      {
         m_require_ports = new PortInstance*[num_require_ports];
      }
   }
   apx::error_t NodeInstance::create_provide_port(port_id_t port_id, std::string const& name, std::unique_ptr<apx::vm::Program> pack_program, std::uint32_t data_offset, std::uint32_t& data_size)
   {
      if (port_id < m_num_provide_ports)
      {
         m_provide_ports[port_id] = new PortInstance(PortType::ProvidePort, port_id, name, pack_program.release(), nullptr);
         return m_provide_ports[port_id]->derive_properties(data_offset, data_size);
      }
      return APX_INVALID_ARGUMENT_ERROR;
   }

   apx::error_t NodeInstance::create_require_port(port_id_t port_id, std::string const& name, std::unique_ptr<apx::vm::Program> pack_program, std::unique_ptr<apx::vm::Program> unpack_program, std::uint32_t data_offset, std::uint32_t& data_size)
   {
      if (port_id < m_num_require_ports)
      {
         m_require_ports[port_id] = new PortInstance(PortType::RequirePort, port_id, name, pack_program.release(), unpack_program.release());
         return m_require_ports[port_id]->derive_properties(data_offset, data_size);
      }
      return APX_INVALID_ARGUMENT_ERROR;
   }

   apx::error_t NodeInstance::create_port_init_data_memory(std::uint8_t *&provide_port_data, std::size_t &provide_port_data_size,
      std::uint8_t*& require_port_data, std::size_t &require_port_data_size)
   {
      apx::error_t result = APX_NO_ERROR;
      m_provide_port_init_data_size = 0u;
      m_require_port_init_data_size = 0u;

      if (m_num_provide_ports > 0u)
      {

         result = calc_init_data_size(m_provide_ports, m_num_provide_ports, m_provide_port_init_data_size);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         m_provide_port_init_data = new std::uint8_t[m_provide_port_init_data_size];
         std::memset(m_provide_port_init_data, 0u, m_provide_port_init_data_size);
         provide_port_data = m_provide_port_init_data;
         provide_port_data_size = m_provide_port_init_data_size;
      }
      if (m_num_provide_ports > 0u)
      {
         result = calc_init_data_size(m_require_ports, m_num_require_ports, m_require_port_init_data_size);
         if (result != APX_NO_ERROR)
         {
            return result;
         }
         m_require_port_init_data = new std::uint8_t[m_require_port_init_data_size];
         std::memset(m_require_port_init_data, 0u, m_require_port_init_data_size);
         require_port_data = m_require_port_init_data;
         require_port_data_size = m_require_port_init_data_size;
      }
      return APX_NO_ERROR;
   }

   PortInstance* NodeInstance::get_provide_port(std::size_t port_id)
   {
      if (port_id < m_num_provide_ports)
      {
         return m_provide_ports[port_id];
      }
      return nullptr;
   }

   PortInstance* NodeInstance::get_require_port(std::size_t port_id)
   {
      if (port_id < m_num_require_ports)
      {
         return m_require_ports[port_id];
      }
      return nullptr;
   }

   apx::error_t NodeInstance::calc_init_data_size(PortInstance** port_list, std::size_t num_ports, std::size_t& total_size)
   {
      if ( (port_list == nullptr) || (num_ports == 0) )
      {
         return APX_INVALID_ARGUMENT_ERROR;
      }

      for (std::size_t port_id = 0u; port_id < num_ports; port_id++)
      {
         auto port_instance = port_list[port_id];
         assert(port_instance != nullptr);
         auto data_size = port_instance->get_data_size();
         assert(data_size > 0u);
         total_size += data_size;
      }
      return APX_NO_ERROR;
   }
}

