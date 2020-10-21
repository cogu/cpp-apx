/*****************************************************************************
* \file      node_data.h
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
#pragma once

#include <memory>
#include "cpp-apx/types.h"
#include "cpp-apx/error.h"

namespace apx
{
   class NodeData
   {
   public:
      NodeData() {};
      std::size_t definition_data_size() const { return m_definition_data_size; }
      std::size_t provide_port_data_size() const { return m_provide_port_data_size; }
      std::size_t require_port_data_size() const { return m_require_port_data_size; }
      std::size_t num_provide_ports() const { return m_num_provide_ports; }
      std::size_t num_require_ports() const { return m_num_require_ports; }
      apx::error_t create_definition_data(std::uint8_t const* init_data, std::size_t data_size);
      apx::error_t create_provide_port_data(std::size_t num_ports, std::uint8_t const* init_data, std::size_t data_size);
      apx::error_t create_require_port_data(std::size_t num_ports, std::uint8_t const* init_data, std::size_t data_size);
      apx::error_t write_provide_port_data(std::size_t offset, std::uint8_t const* src, std::size_t size);
      apx::error_t read_provide_port_data(std::size_t offset, std::uint8_t* dest, std::size_t size) const;
      apx::error_t write_require_port_data(std::size_t offset, std::uint8_t const* src, std::size_t size);
      apx::error_t read_require_port_data(std::size_t offset, std::uint8_t* dest, std::size_t size) const;
#ifdef UNIT_TEST
      std::uint8_t const* get_provide_port_data() const { return m_provide_port_data.get(); }
      std::uint8_t const* get_require_port_data() const { return m_require_port_data.get(); }
#endif
   protected:
      std::unique_ptr<std::uint8_t[]> m_definition_data{ nullptr };
      std::unique_ptr<std::uint8_t[]> m_provide_port_data{ nullptr };
      std::unique_ptr<std::uint8_t[]> m_require_port_data{ nullptr };
      std::size_t m_definition_data_size{ 0u };
      std::size_t m_provide_port_data_size{ 0u };
      std::size_t m_require_port_data_size{ 0u };
      std::size_t m_num_provide_ports{ 0u };
      std::size_t m_num_require_ports{ 0u };

   };
}

