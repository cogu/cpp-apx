#include "cpp-apx/node.h"

apx::error_t apx::Node::append(apx::DataType* data_type)
{
   if (data_type == nullptr)
   {
      return APX_NULL_PTR_ERROR;
   }
   auto it = m_type_map.find(data_type->name);
   if (it != m_type_map.end())
   {
      return APX_TYPE_ALREADY_EXIST_ERROR;
   }
   else
   {
      data_type->type_id = static_cast<apx::type_id_t>(m_data_types.size());
      m_data_types.push_back(std::unique_ptr<apx::DataType>(data_type));
      m_type_map.insert(std::make_pair(data_type->name, data_type));
   }
   return APX_NO_ERROR;
}

apx::error_t apx::Node::append(apx::Port* port)
{
   if (port == nullptr)
   {
      return APX_NULL_PTR_ERROR;
   }
   auto it = m_port_map.find(port->name);
   if (it != m_port_map.end())
   {
      return APX_PORT_ALREADY_EXIST_ERROR;
   }
   else
   {
      if (port->port_type == apx::PortType::RequirePort)
      {
         port->port_id = static_cast<apx::port_id_t>(m_require_ports.size());
         m_require_ports.push_back(std::unique_ptr<apx::Port>(port));
      }
      else
      {
         port->port_id = static_cast<apx::port_id_t>(m_provide_ports.size());
         m_provide_ports.push_back(std::unique_ptr<apx::Port>(port));
      }
      m_port_map.insert(std::make_pair(port->name, port));
   }
   return APX_NO_ERROR;
}

apx::DataType* apx::Node::get_data_type(apx::type_id_t id)
{
   std::size_t i = static_cast<std::size_t>(id);
   if (m_data_types.size() > i)
   {
      return m_data_types[i].get();
   }
   return nullptr;
}

apx::Port* apx::Node::get_require_port(apx::port_id_t id)
{
   std::size_t i = static_cast<std::size_t>(id);
   if (m_require_ports.size() > i)
   {
      return m_require_ports[i].get();
   }
   return nullptr;
}

apx::Port* apx::Node::get_provide_port(apx::port_id_t id)
{
   std::size_t i = static_cast<std::size_t>(id);
   if (m_provide_ports.size() > i)
   {
      return m_provide_ports[i].get();
   }
   return nullptr;
}

apx::DataType* apx::Node::get_last_data_type()
{   
   if (m_data_types.size() > 0)
   {
      return m_data_types[m_data_types.size()-1].get();
   }
   return nullptr;
}

apx::Port* apx::Node::get_last_require_port()
{
   if (m_require_ports.size() > 0)
   {
      return m_require_ports[m_require_ports.size() - 1].get();
   }
   return nullptr;
}

apx::Port* apx::Node::get_last_provide_port()
{
   if (m_provide_ports.size() > 0)
   {
      return m_provide_ports[m_provide_ports.size() - 1].get();
   }
   return nullptr;
}

