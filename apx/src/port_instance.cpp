/*****************************************************************************
* \file      port_instance.cpp
* \author    Conny Gustafsson
* \date      2020-08-27
* \brief     Static information about an APX port (things that do not change during run-time)
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
#include "cpp-apx/port_instance.h"
#include <cassert>

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

    element_id_t PortInstance::data_element_id() const
    {
       if (m_effective_data_element != nullptr)
       {
          return m_effective_data_element->get_id();
       }
       return INVALID_ELEMENT_ID;
    }

    std::size_t PortInstance::get_computation_list_length() const
    {
       if (m_computation_list != nullptr)
       {
          return m_computation_list->get_computation_length();
       }
       return 0u;
    }

    Computation const* PortInstance::get_computation(std::size_t index) const
    {
       if (m_computation_list != nullptr)
       {
          return m_computation_list->get_computation(index);
       }
       return nullptr;

    }

    computation_id_t PortInstance::get_computation_list_id() const
    {
       if (m_computation_list != nullptr)
       {
          return m_computation_list->get_id();
       }
       return apx::INVALID_COMPUTATION_ID;
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
