#pragma once
#include <mutex>
#include "cpp-apx/client_connection.h"
#include "msocket_adapter.h"
#ifdef UNIT_TEST
#include "testsocket.h"
#else
#include "msocket.h"
#endif

#ifdef _MSC_VER
# define ACQUIRES_LOCK(lock) _Acquires_lock_(lock)
# define RELEASES_LOCK(lock) _Releases_lock_(lock)
# define REQUIRES_LOCK_HELD(lock) _Requires_lock_held_(lock)
#else
# define ACQUIRES_LOCK(lock)
# define RELEASES_LOCK(lock)
# define REQUIRES_LOCK_HELD(lock)
#endif

namespace apx
{
#ifdef UNIT_TEST
   using SOCKET_TYPE = testsocket_t;
#else
   using SOCKET_TYPE = msocket_t;
#endif
   class SocketClientConnection : public msocket::Handler, public apx::ClientConnection
   {
   public:
      SocketClientConnection(SOCKET_TYPE* socket);
      SocketClientConnection(SOCKET_TYPE* socket, Client* parent_client);
      ~SocketClientConnection();
      //connection API
#ifdef UNIT_TEST
      error_t connect();
      void run() override;
      void receive_accepted_cmd();
      void receive_file_info_cmd(std::uint32_t address, char const* file_name, std::size_t file_size);
      void receive_data_messsage(std::uint32_t address, std::uint8_t const* data, std::size_t size);
#else
      error_t connect_tcp(char const* address, std::uint16_t port);
      error_t connect_tcp(std::string const& address, std::uint16_t port);
# ifndef _WIN32
      error_t connect_unix(std::string const& path);
# endif
#endif

      //msocket::Handler API
      void socket_connected(const std::string & address, std::uint16_t port) override;
      void socket_disconnected() override;
      int socket_data_received(const std::uint8_t* data, std::size_t data_size, std::size_t& parse_len) override;

      //apx::TransmitHandler API
      std::int32_t transmit_max_bytes_avaiable() const override;
      std::int32_t transmit_current_bytes_avaiable() const override;
      ACQUIRES_LOCK(m_mutex) void transmit_begin() override;
      RELEASES_LOCK(m_mutex) void transmit_end() override;
      REQUIRES_LOCK_HELD(m_mutex) error_t transmit_data_message(std::uint32_t write_address, bool more_bit, std::uint8_t const* msg_data, std::int32_t msg_size, std::int32_t& bytes_available) override;
      error_t transmit_direct_message(std::uint8_t const* data, std::int32_t size, std::int32_t& bytes_available) override;

   protected:
      void send_packet();

      SOCKET_TYPE* m_socket;
      std::mutex m_mutex;
      apx::ByteArray m_transmit_buffer;
      std::size_t const m_default_buffer_size{ 2048u };
      std::size_t m_pending_bytes{ 0u };
   };
}


