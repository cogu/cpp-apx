#pragma once
#include <memory>
#include <cstddef>
#include <vector>
#include <cstdint>
#include <string>
#include "cpp-apx/types.h"
#include "cpp-apx/program.h"
#include "cpp-apx/data_element.h"

namespace apx
{
   class PortInstance
   {
   public:
      PortInstance() = delete;
      PortInstance(apx::PortType type, port_id_t port_id, std::string const & name, apx::vm::Program* pack_program, apx::vm::Program* unpack_program) :
         m_port_type{ type },
         m_port_id { port_id },
         m_name { name },
         m_pack_program{ pack_program },
         m_unpack_program{ unpack_program }
      {}
      apx::error_t derive_properties(std::uint32_t offset, std::uint32_t&size);
      std::string const& get_name() { return m_name; }
      std::uint32_t get_data_offset() { return m_data_offset; }
      std::size_t get_data_size() { return static_cast<std::size_t>(m_data_size); }
      std::uint32_t get_queue_length() { return m_queue_length; }
      std::size_t get_element_size() { return static_cast<std::size_t>(m_element_size); }
      bool is_dynamic_data() { return m_is_dynamic_data; }
      apx::vm::Program const& get_pack_program() { return *m_pack_program; }
      apx::vm::Program const& get_unpack_program() { return *m_unpack_program; }
   protected:
      apx::PortType m_port_type;
      apx::port_id_t m_port_id;
      std::string m_name;
      std::uint32_t m_data_offset = 0u;
      std::uint32_t m_data_size = 0u;
      std::uint32_t m_queue_length = 0u;
      std::uint32_t m_element_size = 0u; //Only used when m_queue_length > 0
      std::unique_ptr<apx::vm::Program> m_pack_program;
      std::unique_ptr<apx::vm::Program> m_unpack_program;
      bool m_is_dynamic_data = false;

      apx::error_t process_info_from_program_header(apx::vm::Program const* program);
   };
}