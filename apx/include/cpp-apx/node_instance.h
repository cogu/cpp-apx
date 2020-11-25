/*****************************************************************************
* \file      node_instance.h
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
#pragma once

#include <vector>
#include <string>
#include <memory>
#include "cpp-apx/types.h"
#include "cpp-apx/port_instance.h"
#include "cpp-apx/error.h"
#include "cpp-apx/vm.h"
#include "cpp-apx/node_data.h"
#include "cpp-apx/data_element.h"
#include "cpp-apx/computation.h"
#include "cpp-apx/file_manager.h"
#include "cpp-apx/byte_port_map.h"

namespace apx
{
   class NodeManager;

   class NodeInstance : public FileNotificationHandler
   {
   public:
      NodeInstance(){}
      ~NodeInstance();
      NodeInstance(std::string const& name) : m_name{ name } {}
      void set_name(std::string const& name) { m_name = name; }
      std::string const& get_name() const { return m_name; }
      void alloc_port_instance_memory(std::size_t num_provide_ports, std::size_t num_require_ports);
      apx::error_t create_provide_port(port_id_t port_id, std::string const& name, std::unique_ptr<apx::vm::Program> pack_program,
         std::uint32_t data_offset, std::uint32_t& data_size);
      apx::error_t create_require_port(port_id_t port_id, std::string const& name, std::unique_ptr<apx::vm::Program> pack_program,
         std::unique_ptr<apx::vm::Program> unpack_program, std::uint32_t data_offset, std::uint32_t& data_size);
      std::size_t get_num_data_elements() const { return m_num_data_elements; }
      std::size_t get_num_computation_lists() const { return m_num_computation_lists; }
      std::size_t get_num_provide_ports() const { return m_num_provide_ports; }
      std::size_t get_num_require_ports() const { return m_num_require_ports; }
      std::size_t get_provide_port_init_data_size() const { return m_provide_port_init_data_size; }
      std::size_t get_require_port_init_data_size() const { return m_require_port_init_data_size; }
      std::uint8_t const* get_provide_port_init_data() const { return m_provide_port_init_data; }
      std::uint8_t const* get_require_port_init_data() const { return m_require_port_init_data; }
      apx::error_t create_port_init_data_memory(std::uint8_t*& provide_port_data, std::size_t& provide_port_data_size,
         std::uint8_t*& require_port_data, std::size_t& require_port_data_size);
      PortInstance* get_provide_port(port_id_t port_id) const;
      PortInstance* get_require_port(port_id_t port_id) const;
      DataElement const* get_data_element(element_id_t id) const;
      ComputationList const* get_computation_list(computation_id_t id) const;
      apx::error_t create_node_data(std::uint8_t const* definition_data, std::size_t definition_size);
      bool has_provide_port_data() const { return m_provide_port_init_data != nullptr; }
      bool has_require_port_data() const { return m_require_port_init_data != nullptr; }
      NodeData const* get_const_node_data() const { return m_node_data.get(); }
      NodeData* get_node_data() const { return m_node_data.get(); }
      bool has_node_data() const { return m_node_data.get() != nullptr; }
      std::size_t get_definition_size() const;
      std::uint8_t const* get_definition_data() const;
      void create_data_element_list(std::vector<std::unique_ptr<DataElement>>& data_element_list);
      void create_computation_lists(std::vector<std::unique_ptr<ComputationList>>& computation_lists);
      void create_require_port_byte_map();
      error_t attach_to_file_manager(FileManager* file_manager);
      error_t file_open_notify(File* file) override;
      error_t file_close_notify(File* file) override;
      error_t file_write_notify(File* file, std::uint32_t offset, std::uint8_t const* data, std::size_t size) override;
      PortDataState get_require_port_data_state() const { return m_require_port_data_state; }
      PortDataState get_provide_port_data_state() const{ return m_provide_port_data_state; }
      void set_require_port_data_state(PortDataState state) { m_require_port_data_state = state; }
      void set_provide_port_data_state(PortDataState state) { m_provide_port_data_state = state; }
      BytePortMap const* get_require_port_map() { return const_cast<const BytePortMap*>(m_require_port_byte_map.get()); }
      void set_node_manager(NodeManager* node_manager) { m_node_manager = node_manager; }
      NodeManager* get_node_manager() const { return m_node_manager; }
      port_id_t lookup_require_port_id(std::size_t byte_offset);
      PortInstance* find(char const* name);
      PortInstance* find(std::string const& name);


   protected:
      std::string m_name;
      std::size_t m_num_provide_ports{ 0u };
      std::size_t m_num_require_ports{ 0u };
      std::size_t m_num_data_elements{ 0u };
      std::size_t m_num_computation_lists{ 0u };
      PortInstance** m_provide_ports{ nullptr };
      PortInstance** m_require_ports{ nullptr };
      DataElement** m_data_elements{ nullptr };
      ComputationList** m_computation_lists{ nullptr };
      std::uint8_t* m_provide_port_init_data{ nullptr };
      std::uint8_t* m_require_port_init_data{ nullptr };
      std::size_t m_provide_port_init_data_size{ 0u };
      std::size_t m_require_port_init_data_size{ 0u };
      std::unique_ptr<NodeData> m_node_data{ nullptr };
      std::unique_ptr<BytePortMap> m_require_port_byte_map{ nullptr }; //Used by APX clients
      PortDataState m_require_port_data_state{ PortDataState::Init };
      PortDataState m_provide_port_data_state{ PortDataState::Init };
      NodeManager* m_node_manager{ nullptr };

      apx::error_t calc_init_data_size(PortInstance **port_list, std::size_t num_ports, std::size_t & total_size);
      error_t fill_definition_file_info(rmf::FileInfo& file_info);
      void fill_provide_port_data_file_info(rmf::FileInfo& file_info);
      error_t send_definition_data_to_file_manager(FileManager* file_manager, rmf::FileInfo const* file_info);
      error_t send_provide_port_data_to_file_manager(FileManager* file_manager, rmf::FileInfo const* file_info);
      error_t process_remote_write_require_port_data(std::uint32_t offset, std::uint8_t const* data, std::size_t size);
   };
}
