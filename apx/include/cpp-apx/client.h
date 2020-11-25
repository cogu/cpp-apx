/*****************************************************************************
* \file      client.h
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
#pragma once

#include <memory>
#include <mutex>
#include "cpp-apx/socket_client_connection.h"
#include "cpp-apx/event_listener.h"
#include "cpp-apx/vm.h"
#include "dtl/dtl.hpp"
#ifdef UNIT_TEST
#include "testsocket.h"
#endif

namespace apx
{
   class Client
   {
   public:
      friend class ClientConnection;
      error_t build_node(char const* apx_text);
      error_t build_node(std::string const& apx_text);
      void register_event_listener(ClientEventListener* listener) { m_event_listener = listener; }
      void unregister_event_listener() { m_event_listener = nullptr; }

      //Port API
      error_t read_port_value(PortInstance* port_instance, dtl::ScalarValue& sv);
      error_t write_port_value(PortInstance* port_instance, dtl::ScalarValue& sv);
      PortInstance* get_port(char const* node_name, char const* port_name);
      PortInstance* get_port(std::string const& node_name, std::string const& port_name);

      //Connect API
#ifndef UNIT_TEST
      error_t connect_tcp(char const* address, std::uint16_t port);
      error_t connect_tcp(std::string const& address, std::uint16_t port);
      error_t connect_unix(char const* path);
      error_t connect_unix(std::string const& path);
#else
      error_t connect(testsocket_t* test_socket);
      void run();
      void receive_accepted_cmd();
      void receive_file_info_cmd(std::uint32_t address, char const* file_name, std::size_t file_size);
      void receive_data_messsage(std::uint32_t address, std::uint8_t const* data, std::size_t size);
#endif

   protected:
      void on_connection_connected(ClientConnection *connection);
      void on_connection_disconnected(ClientConnection* connection);
      void on_require_port_written(PortInstance* port_instance);

      NodeManager m_node_manager;
      std::unique_ptr<SocketClientConnection> m_connection{ nullptr };
      ClientEventListener* m_event_listener{ nullptr }; //TODO: Replace with list to allow parellell event listeners
      std::uint8_t* acquire_buffer(std::size_t required_size, std::uint8_t* suggested_buffer, std::size_t& buffer_size);
      std::mutex m_mutex;
      VirtualMachine m_vm;
   };
}
