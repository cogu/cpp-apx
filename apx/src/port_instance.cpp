#include "cpp-apx/port_instance.h"
#include <iostream>

namespace apx
{
   apx::error_t PortInstance::derive_properties(std::uint32_t offset, std::uint32_t& size)
   {
      apx::error_t result = APX_NO_ERROR;
      result = process_info_from_program_header(
         (m_port_type == PortType::ProvidePort)? m_pack_program.get() : m_unpack_program.get() );
      if (result != APX_NO_ERROR)
      {
         return result;
      }
      m_data_offset = offset;
      size = m_data_size;
      return APX_NO_ERROR;
   }

   apx::error_t PortInstance::process_info_from_program_header(apx::vm::Program const* program)
   {
      if (program == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      apx::vm::ProgramHeader header;
      auto result = apx::vm::decode_program_header(*program, header);
      if (result != APX_NO_ERROR)
      {
         return result;
      }
      m_data_size = header.data_size;
      m_is_dynamic_data = header.is_dynamic_data;
      m_queue_length = header.queue_length;
      m_element_size = header.element_size;
      return APX_NO_ERROR;
   }
}
