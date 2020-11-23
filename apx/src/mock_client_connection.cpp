/*****************************************************************************
* \file      client_connection_mock.cpp
* \author    Conny Gustafsson
* \date      2020-11-10
* \brief     A client connection used during unit testing
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
#include <cassert>
#include <cstring>
#include "cpp-apx/remotefile.h"
#include "cpp-apx/numheader.h"
#include "cpp-apx/mock_client_connection.h"

namespace apx
{
   std::int32_t MockClientConnection::transmit_max_bytes_avaiable() const
   {
      return static_cast<std::int32_t>(m_default_buffer_size);
   }

   std::int32_t MockClientConnection::transmit_current_bytes_avaiable() const
   {
      return static_cast<std::int32_t>(m_transmit_buffer.size());
   }

   void MockClientConnection::transmit_begin()
   {
      if (m_transmit_buffer.size() < m_default_buffer_size)
      {
         m_transmit_buffer.resize(m_default_buffer_size);
      }
      m_pending_bytes = 0u;
      assert(m_transmit_buffer.size() >= m_default_buffer_size);
   }

   void MockClientConnection::transmit_end()
   {
      if (m_pending_bytes > 0u)
      {
         send_packet();
      }
   }

   error_t MockClientConnection::transmit_data_message(std::uint32_t write_address, bool more_bit, std::uint8_t const* msg_data, std::int32_t msg_size, std::int32_t& bytes_available)
   {
      std::array<std::uint8_t, numheader::LONG32_SIZE + rmf::HIGH_ADDR_SIZE> header;
      std::size_t const address_size = rmf::needed_encoding_size(write_address);
      std::size_t const payload_size = address_size + msg_size;
      if (payload_size > transmit_max_bytes_avaiable())
      {
         return APX_MSG_TOO_LARGE_ERROR;
      }
      std::size_t const header1_size = numheader::encode32(header.data(), header.data() + header.size(), static_cast<std::uint16_t>(payload_size));
      assert(header1_size > 0);
      std::size_t const header2_size = rmf::address_encode(header.data() + header1_size, header.size(), write_address, more_bit);
      assert(header2_size == address_size);
      std::size_t const bytes_to_send = header1_size + header2_size + payload_size;
      std::size_t const buffer_available = m_transmit_buffer.size() - m_pending_bytes;
      if (bytes_to_send > buffer_available)
      {
         send_packet();
         assert(m_pending_bytes == 0u);
      }
      std::memcpy(m_transmit_buffer.data() + m_pending_bytes, header.data(), header1_size + header2_size);
      m_pending_bytes += (header1_size + header2_size);
      std::memcpy(m_transmit_buffer.data() + m_pending_bytes, msg_data, msg_size);
      m_pending_bytes += msg_size;
      bytes_available = static_cast<std::int32_t>(m_transmit_buffer.size() - m_pending_bytes);
      return APX_NO_ERROR;
   }

   error_t MockClientConnection::transmit_direct_message(std::uint8_t const* msg_data, std::int32_t msg_size, std::int32_t& bytes_available)
   {
      std::array<std::uint8_t, numheader::LONG32_SIZE> header;
      if (msg_size > transmit_max_bytes_avaiable())
      {
         return APX_MSG_TOO_LARGE_ERROR;
      }
      std::size_t const header_size = numheader::encode32(header.data(), header.data() + header.size(), static_cast<std::uint16_t>(msg_size));
      std::size_t const bytes_to_send = header_size + msg_size;
      std::size_t const buffer_available = m_transmit_buffer.size() - m_pending_bytes;
      if (bytes_to_send > buffer_available)
      {
         send_packet();
         assert(m_pending_bytes == 0u);
      }
      std::memcpy(m_transmit_buffer.data() + m_pending_bytes, header.data(), header_size);
      m_pending_bytes += header_size;
      std::memcpy(m_transmit_buffer.data() + m_pending_bytes, msg_data, msg_size);
      m_pending_bytes += msg_size;
      bytes_available = static_cast<std::int32_t>(m_transmit_buffer.size() - m_pending_bytes);
      return APX_NO_ERROR;
   }

   error_t MockClientConnection::request_open_local_file(char const* file_name)
   {
      auto* file = m_file_manager.find_local_file_by_name(file_name);
      if (file == nullptr)
      {
         return APX_FILE_NOT_FOUND_ERROR;
      }
      std::array<std::uint8_t, rmf::HIGH_ADDR_SIZE + rmf::CMD_TYPE_SIZE + rmf::FILE_OPEN_CMD_SIZE> buffer;
      if (rmf::address_encode(buffer.data(), rmf::HIGH_ADDR_SIZE, rmf::CMD_AREA_START_ADDRESS, false) != rmf::HIGH_ADDR_SIZE)
      {
         return APX_INTERNAL_ERROR;
      }
      std::size_t const cmd_size = rmf::CMD_TYPE_SIZE + rmf::FILE_OPEN_CMD_SIZE;
      auto result = rmf::encode_open_file_cmd(buffer.data() + rmf::HIGH_ADDR_SIZE, cmd_size, file->get_address_without_flags());
      if (result == 0)
      {
         return APX_INTERNAL_ERROR;
      }
      return m_file_manager.message_received(buffer.data(), buffer.size());
   }

   error_t MockClientConnection::publish_remote_file(std::uint32_t address, char const* file_name, std::size_t file_size)
   {
      rmf::FileInfo file_info{ file_name, static_cast<std::uint32_t>(file_size), address };
      std::array<std::uint8_t, rmf::HIGH_ADDR_SIZE + rmf::CMD_TYPE_SIZE + rmf::FILE_INFO_HEADER_SIZE + rmf::FILE_NAME_MAX_SIZE> buffer;
      if (rmf::address_encode(buffer.data(), rmf::HIGH_ADDR_SIZE, rmf::CMD_AREA_START_ADDRESS, false) != rmf::HIGH_ADDR_SIZE)
      {
         return APX_INTERNAL_ERROR;
      }
      std::size_t const max_cmd_size = buffer.size() - rmf::HIGH_ADDR_SIZE;
      std::size_t const cmd_size = rmf::encode_publish_file_cmd(buffer.data() + rmf::HIGH_ADDR_SIZE, max_cmd_size, &file_info);
      if (cmd_size == 0)
      {
         return APX_INTERNAL_ERROR;
      }
      return m_file_manager.message_received(buffer.data(), rmf::HIGH_ADDR_SIZE + cmd_size);
   }

   error_t MockClientConnection::write_remote_data(std::uint32_t address, std::uint8_t const* payload_data, std::size_t payload_size)
   {
      std::array<std::uint8_t, rmf::HIGH_ADDR_SIZE> header;
      auto header_size = rmf::address_encode(header.data(), header.size(), address, false);
      if (header_size == 0u)
      {
         return APX_INTERNAL_ERROR;
      }
      apx::ByteArray msg(header_size + payload_size);
      std::memcpy(msg.data(), header.data(), header_size);
      std::memcpy(msg.data() + header_size, payload_data, payload_size);
      return m_file_manager.message_received(msg.data(), msg.size());
   }


#ifdef UNIT_TEST
   void MockClientConnection::run()
   {
      m_file_manager.run();
   }
#endif

   void MockClientConnection::send_packet()
   {
      apx::ByteArray packet(m_transmit_buffer.data(), m_transmit_buffer.data() + m_pending_bytes);
      m_transmit_log.push_back(packet);
      m_transmit_buffer.clear();
      m_pending_bytes = 0u;
   }

}