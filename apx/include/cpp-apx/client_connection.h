/*****************************************************************************
* \file      client_connection.h
* \author    Conny Gustafsson
* \date      2020-11-10
* \brief     Client connection (abstract) base class
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
#include "cpp-apx/file_manager.h"
#include "cpp-apx/transmit_handler.h"
#include "cpp-apx/node_manager.h"

namespace apx
{
   class ClientConnection : public TransmitHandler
   {
   public:
      ClientConnection() :m_file_manager{ this }, m_node_manager{ nullptr }{}
      virtual ~ClientConnection() {}
      void greeting_header_accepted();
      void connected();
      void disconnected();
      void attach_node_manager(NodeManager* node_manager);
      NodeManager* get_node_manager() const { return m_node_manager; }
      error_t build_node(char const* definition_text);
#ifdef UNIT_TEST
      virtual void run();
#endif
   protected:
      error_t attach_node_instance(NodeInstance* node_instance);
      void send_greeting_header();
      int on_data_received(std::uint8_t const* data, std::size_t data_size, std::size_t& parse_len);
      std::uint8_t const* parse_message(std::uint8_t const* begin, std::uint8_t const* end, apx::error_t& error_code);
      void set_data_reception_error(apx::error_t error_code);
      bool is_greeting_accepted(std::uint8_t const* msg_data, std::size_t msg_size, apx::error_t& error_code);
      bool m_is_greeting_accepted{ false };
      FileManager m_file_manager;
      NodeManager* m_node_manager;

   };
}