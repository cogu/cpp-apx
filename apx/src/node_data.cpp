/*****************************************************************************
* \file      node_data.cpp
* \author    Conny Gustafsson
* \date      2020-10-20
* \brief     Container for the dynamic data of an APX ndoe
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

#include <cstring>
#include "cpp-apx/node_data.h"

namespace apx
{

   apx::error_t NodeData::create_definition_data(std::uint8_t const* init_data, std::size_t data_size)
   {
      if ( (init_data != nullptr) && (init_data > 0u) ) //init_data is not optional
      {
         m_definition_data.reset(new std::uint8_t[data_size]);
         m_definition_data_size = data_size;
         std::memcpy(m_definition_data.get(), init_data, data_size);
         return APX_NO_ERROR;
      }
      return APX_INVALID_ARGUMENT_ERROR;
   }

   apx::error_t NodeData::create_provide_port_data(std::size_t num_ports, std::uint8_t const* init_data, std::size_t data_size)
   {
      if ( data_size > 0u ) //init_data is optional
      {
         m_provide_port_data.reset(new std::uint8_t[data_size]);
         m_provide_port_data_size = data_size;
         m_num_provide_ports = num_ports;
         if (init_data != nullptr)
         {
            std::memcpy(m_provide_port_data.get(), init_data, data_size);
         }
         else
         {
            std::memset(m_provide_port_data.get(), 0, data_size);
         }
         return APX_NO_ERROR;
      }
      return APX_INVALID_ARGUMENT_ERROR;
   }

   apx::error_t NodeData::create_require_port_data(std::size_t num_ports, std::uint8_t const* init_data, std::size_t data_size)
   {
      if (data_size > 0u)  //init_data is optional
      {
         m_require_port_data.reset(new std::uint8_t[data_size]);
         m_require_port_data_size = data_size;
         m_num_require_ports = num_ports;
         if (init_data != nullptr)
         {
            std::memcpy(m_require_port_data.get(), init_data, data_size);
         }
         else
         {
            std::memset(m_require_port_data.get(), 0, data_size);
         }
         return APX_NO_ERROR;
      }
      return APX_INVALID_ARGUMENT_ERROR;
   }

   apx::error_t NodeData::write_provide_port_data(std::size_t offset, std::uint8_t const* src, std::size_t size)
   {
      if ((offset + size) > m_provide_port_data_size)
      {
         return APX_INVALID_ARGUMENT_ERROR;
      }
      std::memcpy(m_provide_port_data.get() + offset, src, size);
      return APX_NO_ERROR;
   }

   apx::error_t NodeData::read_provide_port_data(std::size_t offset, std::uint8_t* dest, std::size_t size) const
   {
      if ((offset + size) > m_provide_port_data_size)
      {
         return APX_INVALID_ARGUMENT_ERROR;
      }
      std::memcpy(dest, m_provide_port_data.get() + offset, size);
      return APX_NO_ERROR;
   }

   apx::error_t NodeData::write_require_port_data(std::size_t offset, std::uint8_t const* src, std::size_t size)
   {
      if ((offset + size) > m_require_port_data_size)
      {
         return APX_INVALID_ARGUMENT_ERROR;
      }
      std::memcpy(m_require_port_data.get() + offset, src, size);
      return APX_NO_ERROR;
   }

   apx::error_t NodeData::read_require_port_data(std::size_t offset, std::uint8_t* dest, std::size_t size) const
   {
      if ((offset + size) > m_require_port_data_size)
      {
         return APX_INVALID_ARGUMENT_ERROR;
      }
      std::memcpy(dest, m_require_port_data.get() + offset, size);
      return APX_NO_ERROR;
   }


}