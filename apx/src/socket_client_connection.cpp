#include <cassert>
#include <cstring>
#if APX_DEBUG_ENABLE
#include <iostream>
#endif
#include "cpp-apx/remotefile.h"
#include "cpp-apx/numheader.h"
#include "cpp-apx/socket_client_connection.h"
#include "cpp-apx/client.h"

#ifdef UNIT_TEST
#include <array>
#define SOCKET_DELETE testsocket_delete
#define SOCKET_SET_HANDLER(x, y) msocket::set_client_handler(x,y)
#define SOCKET_START_IO(x)
#define SOCKET_SEND testsocket_clientSend
#else
#define SOCKET_DELETE msocket_delete
#define SOCKET_SET_HANDLER(x, y) msocket::set_handler(x,y)
#define SOCKET_START_IO(x) msocket_start_io(x)
#define SOCKET_SEND msocket_send
#endif

namespace apx
{
   SocketClientConnection::SocketClientConnection(SOCKET_TYPE* socket):
      ClientConnection { nullptr }, m_socket(socket)
   {
      if (m_socket != nullptr)
      {
         SOCKET_SET_HANDLER(m_socket, this);
      }
   }

   SocketClientConnection::SocketClientConnection(SOCKET_TYPE* socket, Client* parent_client):
      ClientConnection{ parent_client }, m_socket(socket)
   {
      if (m_socket != nullptr)
      {
         SOCKET_SET_HANDLER(m_socket, this);
      }
   }
   SocketClientConnection::~SocketClientConnection()
   {
      if (m_socket != nullptr)
      {
         SOCKET_DELETE(m_socket);
      }
   }
#ifdef UNIT_TEST
   error_t SocketClientConnection::connect()
   {
      testsocket_onConnect(m_socket);
      return APX_NO_ERROR;
   }

   void SocketClientConnection::run()
   {
      testsocket_run(m_socket);
      for (int i = 0; i < 10; i++)
      {
         ClientConnection::run();
         testsocket_run(m_socket);
      }
   }
   void SocketClientConnection::receive_accepted_cmd()
   {
      std::array<std::uint8_t, numheader::SHORT_SIZE + rmf::HIGH_ADDR_SIZE + rmf::CMD_TYPE_SIZE> msg;
      msg[0] = static_cast<std::uint8_t>(msg.size()) - 1u;
      rmf::address_encode(msg.data() + 1, msg.size(), rmf::CMD_AREA_START_ADDRESS, false);
      rmf::encode_acknowledge_cmd(msg.data() + 1 + rmf::HIGH_ADDR_SIZE, rmf::CMD_TYPE_SIZE);
      testsocket_serverSend(m_socket, msg.data(), static_cast<std::uint32_t>(msg.size()));
   }

   void SocketClientConnection::receive_file_info_cmd(std::uint32_t address, char const* file_name, std::size_t file_size)
   {
      rmf::FileInfo file_info{ file_name, static_cast<std::uint32_t>(file_size), address };
      std::array<std::uint8_t, numheader::SHORT_SIZE + rmf::HIGH_ADDR_SIZE + rmf::CMD_TYPE_SIZE + rmf::FILE_INFO_HEADER_SIZE + rmf::FILE_NAME_MAX_SIZE> msg;
      if (rmf::address_encode(msg.data() + 1, rmf::HIGH_ADDR_SIZE, rmf::CMD_AREA_START_ADDRESS, false) != rmf::HIGH_ADDR_SIZE)
      {
         return;
      }
      std::size_t const max_cmd_size = msg.size() - (1+rmf::HIGH_ADDR_SIZE);
      std::size_t const cmd_size = rmf::encode_publish_file_cmd(msg.data() + 1 + rmf::HIGH_ADDR_SIZE, max_cmd_size, &file_info);
      if (cmd_size == 0)
      {
         return;
      }
      msg[0] = rmf::HIGH_ADDR_SIZE + static_cast<std::uint8_t>(cmd_size);
      testsocket_serverSend(m_socket, msg.data(), 1 + rmf::HIGH_ADDR_SIZE + static_cast<std::uint32_t>(cmd_size));
   }

   void SocketClientConnection::receive_data_messsage(std::uint32_t address, std::uint8_t const* data, std::size_t data_size)
   {
      std::size_t const needed_address_size{ rmf::needed_encoding_size(address) };
      std::size_t const needed_numheader_size = needed_address_size + data_size > numheader::SHORT_MAX ? numheader::LONG32_SIZE : numheader::SHORT_SIZE;
      apx::ByteArray msg(needed_numheader_size + needed_address_size + data_size);
      assert(msg.size() == needed_numheader_size + needed_address_size + data_size);
      numheader::encode32(msg.data(), msg.data() + needed_numheader_size, static_cast<std::uint32_t>(needed_address_size + data_size));
      std::size_t header_size = rmf::address_encode(msg.data() + needed_numheader_size, needed_address_size, address, false);
      assert(header_size == needed_address_size);
      std::memcpy(msg.data() + needed_numheader_size + needed_address_size, data, data_size);
      testsocket_serverSend(m_socket, msg.data(), static_cast<std::uint32_t>(msg.size()));
   }
#else
   error_t SocketClientConnection::connect_tcp(char const* address, std::uint16_t port)
   {
      auto result = msocket_connect(m_socket, address, port);
      if (result < 0)
      {
         return APX_CONNECTION_ERROR;
      }
      return APX_NO_ERROR;
   }

   error_t SocketClientConnection::connect_tcp(std::string const& address, std::uint16_t port)
   {
      return connect_tcp(address.data(), port);
   }
# ifndef _WIN32
   error_t SocketClientConnection::connect_unix(std::string const& path)
   {
      return APX_NO_ERROR;
   }
# endif
#endif

   void SocketClientConnection::socket_connected(const std::string& address, std::uint16_t port)
   {
      (void)address;
      (void)port;
      connected();
   }

   void SocketClientConnection::socket_disconnected()
   {
      disconnected();
   }

   int SocketClientConnection::socket_data_received(const std::uint8_t* data, std::size_t data_size, std::size_t& parse_len)
   {
#if APX_DEBUG_ENABLE
      std::cout << "Received " << std::to_string(data_size) << " bytes" << std::endl;
#endif
      return on_data_received(data, data_size, parse_len);
   }

   std::int32_t SocketClientConnection::transmit_max_bytes_avaiable() const
   {
      return static_cast<std::int32_t>(m_default_buffer_size);
   }

   std::int32_t SocketClientConnection::transmit_current_bytes_avaiable() const
   {
      return static_cast<std::int32_t>(m_transmit_buffer.size());
   }

   ACQUIRES_LOCK(m_mutex)
   void SocketClientConnection::transmit_begin()
   {
      m_mutex.lock();
      if (m_transmit_buffer.size() < m_default_buffer_size)
      {
         m_transmit_buffer.resize(m_default_buffer_size);
      }
      m_pending_bytes = 0u;
      assert(m_transmit_buffer.size() >= m_default_buffer_size);
   }

   RELEASES_LOCK(m_mutex)
   void SocketClientConnection::transmit_end()
   {
      if (m_pending_bytes > 0u)
      {
         send_packet();
      }
      m_mutex.unlock();
   }

   REQUIRES_LOCK_HELD(m_mutex)
   error_t SocketClientConnection::transmit_data_message(std::uint32_t write_address, bool more_bit, std::uint8_t const* msg_data, std::int32_t msg_size, std::int32_t& bytes_available)
   {
      std::array<std::uint8_t, numheader::LONG16_SIZE + rmf::HIGH_ADDR_SIZE> header;
      std::size_t const address_size = rmf::needed_encoding_size(write_address);
      std::size_t const payload_size = address_size + msg_size;
      if (payload_size > transmit_max_bytes_avaiable())
      {
         return APX_MSG_TOO_LARGE_ERROR;
      }
      std::size_t const header1_size = numheader::encode16(header.data(), header.data() + header.size(), static_cast<std::uint16_t>(payload_size));
      assert(header1_size > 0);
      std::size_t const header2_size = rmf::address_encode(header.data() + header1_size, header.size(), write_address, more_bit);
      assert(header2_size == address_size);
      std::size_t const bytes_to_send = header1_size + payload_size;
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

   error_t SocketClientConnection::transmit_direct_message(std::uint8_t const* msg_data, std::int32_t msg_size, std::int32_t& bytes_available)
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

   void SocketClientConnection::send_packet()
   {
      if (m_socket != nullptr)
      {
#ifdef APX_DEBUG_ENABLE
         std::cout << "Sending " << m_pending_bytes << " bytes" << std::endl;
#endif
         SOCKET_SEND(m_socket, m_transmit_buffer.data(), static_cast<std::uint32_t>(m_pending_bytes));
      }
   }
}