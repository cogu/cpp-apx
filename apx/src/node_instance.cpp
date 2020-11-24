/*****************************************************************************
* \file      node_instance.cpp
* \author    Conny Gustafsson
* \date      2020-10-13
* \brief     Container for an instantiated APX node
*
* Copyright (c) 2020 Conny Gustafsson
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
******************************************************************************/

#include <cassert>
#include <cstring>
#include "cpp-apx/node_instance.h"
#include "cpp-apx/sha256.h"

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
      if (m_num_data_elements > 0u)
      {
         for (std::size_t i = 0; i < m_num_data_elements; i++)
         {
            delete m_data_elements[i];
         }
         delete[] m_data_elements;
      }
      if (m_num_computation_lists > 0u)
      {
         for (std::size_t i = 0; i < m_num_computation_lists; i++)
         {
            delete m_computation_lists[i];
         }
         delete[] m_computation_lists;
      }

      if (m_provide_port_init_data != nullptr)
      {
         delete[] m_provide_port_init_data;
      }
      if (m_require_port_init_data != nullptr)
      {
         delete[] m_require_port_init_data;
      }
      if (m_provide_port_refs != nullptr)
      {
         delete[] m_provide_port_refs;
      }
      if (m_require_port_refs != nullptr)
      {
         delete[] m_require_port_refs;
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
      if (m_num_require_ports > 0u)
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

   PortInstance* NodeInstance::get_provide_port(std::size_t port_id) const
   {
      if (port_id < m_num_provide_ports)
      {
         return m_provide_ports[port_id];
      }
      return nullptr;
   }

   PortInstance* NodeInstance::get_require_port(std::size_t port_id) const
   {
      if (port_id < m_num_require_ports)
      {
         return m_require_ports[port_id];
      }
      return nullptr;
   }

   DataElement const* NodeInstance::get_data_element(element_id_t id) const
   {
      if (static_cast<std::size_t>(id) < m_num_data_elements)
      {
         return const_cast<DataElement const*>(m_data_elements[id]);
      }
      return nullptr;
   }

   ComputationList const* NodeInstance::get_computation_list(computation_id_t id) const
   {
      if (static_cast<std::size_t>(id) < m_num_computation_lists)
      {
         return const_cast<ComputationList const*>(m_computation_lists[id]);
      }
      return nullptr;
   }

   apx::error_t NodeInstance::create_node_data(std::uint8_t const* definition_data, std::size_t definition_size)
   {
      std::unique_ptr<NodeData> node_data = std::make_unique<NodeData>();
      auto retval = node_data->create_definition_data(definition_data, definition_size);
      if ( (retval == APX_NO_ERROR) && has_provide_port_data())
      {
         retval = node_data->create_provide_port_data(m_num_provide_ports, m_provide_port_init_data, m_provide_port_init_data_size);
      }
      if ((retval == APX_NO_ERROR) && has_require_port_data())
      {
         retval = node_data->create_require_port_data(m_num_require_ports, m_require_port_init_data, m_require_port_init_data_size);
      }
      if (retval == APX_NO_ERROR)
      {
         m_node_data = std::move(node_data);
      }
      return retval;
   }

   std::size_t NodeInstance::get_definition_size() const
   {
      auto const* node_data = get_const_node_data();
      if (node_data != nullptr)
      {
         return node_data->definition_data_size();
      }
      return 0u;
   }

   std::uint8_t const* NodeInstance::get_definition_data() const
   {
      auto const* node_data = get_const_node_data();
      if (node_data != nullptr)
      {
         return node_data->get_definition_data();
      }
      return nullptr;
   }

   void NodeInstance::create_data_element_list(std::vector<std::unique_ptr<DataElement>>& data_element_list)
   {
      m_num_data_elements = data_element_list.size();
      if (m_num_data_elements > 0u)
      {
         m_data_elements = new DataElement*[m_num_data_elements];
         if (m_data_elements != nullptr)
         {
            for (std::size_t i = 0; i < m_num_data_elements; i++)
            {
               m_data_elements[i] = data_element_list[i].release();
            }
         }
      }
   }

   void NodeInstance::create_computation_lists(std::vector<std::unique_ptr<ComputationList>>& computation_lists)
   {
      m_num_computation_lists = computation_lists.size();
      if (m_num_computation_lists > 0u)
      {
         m_computation_lists = new ComputationList*[m_num_computation_lists];
         if (m_computation_lists != nullptr)
         {
            for (std::size_t i = 0; i < m_num_computation_lists; i++)
            {
               m_computation_lists[i] = computation_lists[i].release();
            }
         }
      }
   }

   void NodeInstance::create_require_port_byte_map()
   {
      m_require_port_byte_map.reset(new BytePortMap(m_require_port_init_data_size,
         const_cast<const apx::PortInstance**>(m_require_ports), m_num_require_ports));
   }

   void NodeInstance::create_port_refs()
   {
      if (m_num_provide_ports > 0)
      {
         m_provide_port_refs = new PortRef[m_num_provide_ports];
         for (std::size_t i = 0u; i < m_num_provide_ports; i++)
         {
            m_provide_port_refs[i].node_instance = this;
            m_provide_port_refs[i].port_instance = m_provide_ports[i];
         }
      }

      if (m_num_require_ports > 0)
      {
         m_require_port_refs = new PortRef[m_num_require_ports];
         for (std::size_t i = 0u; i < m_num_require_ports; i++)
         {
            m_require_port_refs[i].node_instance = this;
            m_require_port_refs[i].port_instance = m_require_ports[i];
         }
      }
   }

   error_t NodeInstance::attach_to_file_manager(FileManager* file_manager)
   {
      rmf::FileInfo file_info;
      if (has_provide_port_data())
      {
         set_provide_port_data_state(PortDataState::WaitingForFileOpenRequest);
         fill_provide_port_data_file_info(file_info);
         auto* provide_port_data_file = file_manager->create_local_file(file_info);
         if (provide_port_data_file == nullptr)
         {
            return APX_FILE_CREATE_ERROR;
         }
         provide_port_data_file->set_notification_handler(this);
      }
      if (has_require_port_data())
      {
         set_require_port_data_state(PortDataState::WaitingForFileInfo);
      }
      auto result = fill_definition_file_info(file_info);
      if (result != APX_NO_ERROR)
      {
         return result;
      }
      auto* definition_data_file = file_manager->create_local_file(file_info);
      if (definition_data_file == nullptr)
      {
         return APX_FILE_CREATE_ERROR;
      }
      definition_data_file->set_notification_handler(this);
      return APX_NO_ERROR;
   }

   error_t NodeInstance::file_open_notify(File* file)
   {

      auto* file_manager = file->get_file_manager();
      auto const file_type = file->get_apx_file_type();
      std::unique_ptr<rmf::FileInfo> file_info{file->clone_file_info()};
      assert(file_manager != nullptr);
      error_t retval = APX_NO_ERROR;
      switch (file_type)
      {
      case FileType::Definition:
         retval = send_definition_data_to_file_manager(file_manager, file_info.get());
         break;
      case FileType::ProvidePortData:
         retval = send_provide_port_data_to_file_manager(file_manager, file_info.get());
         break;
      default:
         retval = APX_UNSUPPORTED_ERROR;
      }
      return retval;
   }

   error_t NodeInstance::file_close_notify(File* file)
   {
      (void)file;
      return APX_NO_ERROR;
   }

   error_t NodeInstance::file_write_notify(File* file, std::uint32_t offset, std::uint8_t const* data, std::size_t size)
   {
      error_t retval = APX_NO_ERROR;
      switch (file->get_apx_file_type())
      {
      case FileType::RequirePortData:
         retval = process_remote_write_require_port_data(offset, data, size);
         break;
      }
      return retval;
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

   error_t NodeInstance::fill_definition_file_info(rmf::FileInfo& file_info)
   {
      file_info.size = static_cast<std::uint32_t>(get_definition_size());
      file_info.name = m_name + ".apx";
      file_info.digest_type = rmf::DigestType::SHA256;
      auto success = sha256::calc(file_info.digest_data.data(), file_info.digest_data.size(), get_definition_data(), get_definition_size());
      return success ? APX_NO_ERROR : APX_INTERNAL_ERROR;
   }

   void NodeInstance::fill_provide_port_data_file_info(rmf::FileInfo& file_info)
   {
      file_info.size = static_cast<std::uint32_t>(get_provide_port_init_data_size());
      file_info.digest_type = rmf::DigestType::None;
      std::memset(file_info.digest_data.data(), 0, file_info.digest_data.size());
      file_info.name = m_name + ".out";
   }

   error_t NodeInstance::send_definition_data_to_file_manager(FileManager* file_manager, rmf::FileInfo const* file_info)
   {
      auto const* definition_data = m_node_data->get_definition_data();
      auto const definition_size = m_node_data->definition_data_size();
      return file_manager->send_local_const_data(file_info->address_without_flags(), definition_data, definition_size);
   }

   error_t NodeInstance::send_provide_port_data_to_file_manager(FileManager* file_manager, rmf::FileInfo const* file_info)
   {
      auto* snapshot = m_node_data->take_provide_port_data_snapshot();
      auto const provide_port_data_size = m_node_data->provide_port_data_size();
      if (snapshot == nullptr)
      {
         return APX_MEM_ERROR;
      }
      return file_manager->send_local_data(file_info->address_without_flags(), snapshot, provide_port_data_size);
   }

   error_t NodeInstance::process_remote_write_require_port_data(std::uint32_t offset, std::uint8_t const* data, std::size_t size)
   {
      if (m_require_port_data_state == PortDataState::WaitingForFileData)
      {
         m_require_port_data_state = PortDataState::Synchronized;
      }
      return m_node_data->write_require_port_data(offset, data, size);
   }
}

