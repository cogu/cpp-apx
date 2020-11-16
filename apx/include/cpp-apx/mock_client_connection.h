/*****************************************************************************
* \file      client_connection_mock.h
* \author    Conny Gustafsson
* \date      2020-11-10
* \brief     A client connection used for unit testing
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

#include <vector>
#include "cpp-apx/client_connection.h"

namespace apx
{
   class MockClientConnection : public apx::ClientConnection
   {
   public:
      MockClientConnection()
      {
         attach_node_manager(&m_node_manager);
      }
      //Transmit API
      std::int32_t transmit_max_bytes_avaiable() const override;
      std::int32_t transmit_current_bytes_avaiable() const override;
      void transmit_begin() override;
      void transmit_end() override;
      error_t transmit_data_message(std::uint32_t write_address, bool more_bit, std::uint8_t const* msg_data, std::int32_t msg_size, std::int32_t& bytes_available) override;

      //Log API
      std::size_t log_length() { return m_transmit_log.size(); }
      apx::ByteArray const& get_log_packet(std::size_t index) { return m_transmit_log[index]; }
      void clear_log() { m_transmit_log.clear(); };

      //Mock API
      FileManager& get_file_manager(){ return m_file_manager; }
      error_t request_open_local_file(char const* file_name);
#ifdef UNIT_TEST
      void run();
#endif

   protected:
      void send_packet();
      std::vector<apx::ByteArray> m_transmit_log;
      apx::ByteArray m_transmit_buffer;
      std::size_t const m_default_buffer_size{ 1024u };
      std::size_t m_pending_bytes{ 0u };
      NodeManager m_node_manager;
   };
}