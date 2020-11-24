/*****************************************************************************
* \file      byte_port_map.h
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
#pragma once

#include "cpp-apx/types.h"
#include "cpp-apx/port_instance.h"
#include <memory>

namespace apx
{
   class BytePortMap
   {
   public:
      BytePortMap() = delete;
      BytePortMap(std::size_t total_size, PortInstance const** port_instance_list, std::size_t num_ports);
      apx::port_id_t lookup (std::size_t offset) const;
   protected:
      size_t m_map_len;
      std::unique_ptr<apx::port_id_t[]> m_map_data;
   };
}
