/*****************************************************************************
* \file      client.cpp
* \author    Conny Gustafsson
* \date      2020-11-22
* \brief     APX Client
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
#include <array>
#include "cpp-apx/client.h"

namespace apx
{
   error_t Client::build_node(char const* apx_text)
   {
      return m_node_manager.build_node(apx_text);
   }

   error_t Client::build_node(std::string const& apx_text)
   {
      return m_node_manager.build_node(apx_text);
   }
   //Port API
   error_t Client::read_port_value(PortInstance* port_instance, dtl::ScalarValue& sv)
   {
      if ( (port_instance == nullptr) || (port_instance->port_type() != PortType::RequirePort) )
      {
         return APX_INVALID_ARGUMENT_ERROR;
      }
      std::array<std::uint8_t, apx::MAX_STACK_BUFFER_SIZE> stack_buffer;
      std::size_t const data_size = port_instance->data_size();
      std::size_t buffer_size = stack_buffer.size();
      std::unique_ptr<std::uint8_t[]> ptr;
      auto* read_buffer = acquire_buffer(data_size, stack_buffer.data(), buffer_size);
      if (read_buffer == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      if (read_buffer != stack_buffer.data())
      {
         ptr.reset(read_buffer); //automaticaly delete later
      }
      auto* node_instance = port_instance->node_instance();
      if (node_instance == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      auto* node_data = node_instance->get_node_data();
      if (node_data == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      error_t retval = node_data->read_require_port_data(port_instance->data_offset(), read_buffer, data_size);
      if (retval == APX_NO_ERROR)
      {
         std::scoped_lock lock(m_mutex);
         retval = m_vm.set_read_buffer(read_buffer, data_size);
         if (retval == APX_NO_ERROR)
         {
            retval = m_vm.select_program(port_instance->unpack_program());
         }
         if (retval == APX_NO_ERROR)
         {
            retval = m_vm.unpack_value(sv);
         }
      }
      return retval;
   }

   error_t Client::write_port_value(PortInstance* port_instance, dtl::ScalarValue& sv)
   {
      if ((port_instance == nullptr) || (port_instance->port_type() != PortType::ProvidePort))
      {
         return APX_INVALID_ARGUMENT_ERROR;
      }
      std::array<std::uint8_t, apx::MAX_STACK_BUFFER_SIZE> stack_buffer;
      std::size_t const data_size = port_instance->data_size();
      std::size_t buffer_size = stack_buffer.size();
      std::unique_ptr<std::uint8_t[]> ptr;
      auto* write_buffer = acquire_buffer(data_size, stack_buffer.data(), buffer_size);
      if (write_buffer == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      if (write_buffer != stack_buffer.data())
      {
         ptr.reset(write_buffer); //automaticaly delete later
      }
      auto* node_instance = port_instance->node_instance();
      if (node_instance == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      auto* node_data = node_instance->get_node_data();
      error_t retval = APX_NO_ERROR;
      if (node_data == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      else
      {
         std::scoped_lock lock(m_mutex);
         retval = m_vm.set_write_buffer(write_buffer, data_size);
         if (retval == APX_NO_ERROR)
         {
            retval = m_vm.select_program(port_instance->pack_program());
         }
         if (retval == APX_NO_ERROR)
         {
            retval = m_vm.pack_value(sv);
         }
         if (retval == APX_NO_ERROR)
         {
            retval = node_data->write_provide_port_data(port_instance->data_offset(), write_buffer, data_size);
         }
      }
      return retval;
   }

   PortInstance* Client::get_port(char const* node_name, char const* port_name)
   {
      auto* node_instance = m_node_manager.find(node_name);
      if (node_instance != nullptr)
      {
         return node_instance->find(port_name);
      }
      return nullptr;
   }

   PortInstance* Client::get_port(std::string const& node_name, std::string const& port_name)
   {
      auto* node_instance = m_node_manager.find(node_name);
      if (node_instance != nullptr)
      {
         return node_instance->find(port_name);
      }
      return nullptr;
   }

   //Connect API
#ifndef UNIT_TEST
   error_t Client::connect_tcp(char const* address, std::uint16_t port)
   {
      auto* msocket = msocket_new(AF_INET);
      if (msocket == nullptr)
      {
         return APX_MEM_ERROR;
      }
      m_connection = std::make_unique<SocketClientConnection>(msocket, this);
      m_connection->start();
      m_connection->attach_node_manager(&m_node_manager);
      return m_connection->connect_tcp(address, port);
   }

   error_t Client::connect_tcp(std::string const& address, std::uint16_t port)
   {
      return connect_tcp(address.data(), port);
   }

   error_t Client::connect_unix(char const* path)
   {
      (void)path;
      return APX_UNSUPPORTED_ERROR;
   }

   error_t Client::connect_unix(std::string const& path)
   {
      (void)path;
      return APX_UNSUPPORTED_ERROR;
   }
#else
   error_t Client::connect(testsocket_t* test_socket)
   {
      m_connection = std::make_unique<SocketClientConnection>(test_socket, this);
      m_connection->attach_node_manager(&m_node_manager);
      return m_connection->connect();
   }
   void Client::run()
   {
      if (m_connection != nullptr)
      {
         m_connection->run();
      }
   }
   void Client::receive_accepted_cmd()
   {
      if (m_connection != 0)
      {
         m_connection->receive_accepted_cmd();
      }
   }
   void Client::receive_file_info_cmd(std::uint32_t address, char const* file_name, std::size_t file_size)
   {
      if (m_connection != 0)
      {
         m_connection->receive_file_info_cmd(address, file_name, file_size);
      }
   }
   void Client::receive_data_messsage(std::uint32_t address, std::uint8_t const* data, std::size_t size)
   {
      if (m_connection != 0)
      {
         m_connection->receive_data_messsage(address, data, size);
      }
   }
#endif

   void Client::on_connection_connected(ClientConnection* connection)
   {
      if (m_event_listener != nullptr)
      {
         m_event_listener->connected1(connection);
      }
   }

   void Client::on_connection_disconnected(ClientConnection* connection)
   {
      if (m_event_listener != nullptr)
      {
         m_event_listener->disconnected1(connection);
      }
   }
   void Client::on_require_port_written(PortInstance* port_instance)
   {
      if (m_event_listener != nullptr)
      {
         m_event_listener->require_port_written1(port_instance);
      }
   }
   std::uint8_t* Client::acquire_buffer(std::size_t required_size, std::uint8_t* suggested_buffer, std::size_t& buffer_size)
   {
      if (required_size > buffer_size)
      {
         buffer_size = required_size;
         return new std::uint8_t[required_size];
      }
      return suggested_buffer;
   }
}