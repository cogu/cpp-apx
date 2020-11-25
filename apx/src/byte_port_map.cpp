/*****************************************************************************
* \file      byte_port_map.cpp
* \author    Conny Gustafsson
* \date      2020-11-24
* \brief     Byte offset to port ID lookup table
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

#include <stdexcept>
#include <cassert>
#include "cpp-apx/byte_port_map.h"

namespace apx
{
   BytePortMap::BytePortMap(std::size_t total_size, PortInstance const** port_instance_list, std::size_t num_ports):
      m_map_len{ total_size }, m_map_data{nullptr}
   {
      if (total_size > 0u)
      {
         std::size_t offset = 0u;
         m_map_data.reset(new port_id_t[total_size]);
         for (port_id_t port_id = 0u; port_id < static_cast<port_id_t>(num_ports); port_id++)
         {
            PortInstance const* port_instance = port_instance_list[port_id];
            auto data_size = port_instance->data_size();
            for (std::size_t i = 0; i < data_size; i++)
            {
               if (offset >= m_map_len)
               {
                  throw std::length_error{ "Inconsistent arguments given to BytePortMap constructor" };
               }
               m_map_data[offset++] = port_id;
            }
         }
         assert(offset == m_map_len); //Is entire map filled in?
      }
   }

   apx::port_id_t BytePortMap::lookup(std::size_t offset) const
   {
      if (offset < m_map_len)
      {
         return m_map_data[offset];
      }
      return apx::INVALID_PORT_ID;
   }
}