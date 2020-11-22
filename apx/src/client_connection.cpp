#include <cassert>
#include <sstream>
#include <iostream>
#include "cpp-apx/client_connection.h"
#include "cpp-apx/numheader.h"

namespace apx
{
   void ClientConnection::greeting_header_accepted()
   {
      m_is_greeting_accepted = true;
      m_file_manager.connected();
   }

   void ClientConnection::connected()
   {
      m_is_greeting_accepted = false;
      send_greeting_header();
   }

   void ClientConnection::disconnected()
   {

   }

   void ClientConnection::attach_node_manager(NodeManager* node_manager)
   {
      if (node_manager != nullptr)
      {
         m_node_manager = node_manager;
         for (auto& node_instance : node_manager->get_nodes())
         {
            attach_node_instance(node_instance);
         }
      }
   }

   error_t ClientConnection::build_node(char const* definition_text)
   {
      if (definition_text == nullptr)
      {
         return APX_INVALID_ARGUMENT_ERROR;
      }
      if (m_node_manager == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      auto retval = m_node_manager->build_node(definition_text);
      if (retval == APX_NO_ERROR)
      {
         auto* node_instance = m_node_manager->get_last_attached();
         assert(node_instance != nullptr);
         attach_node_instance(node_instance);
      }
      return retval;
   }

   error_t ClientConnection::remote_file_published_notification(File* file)
   {
      if (file->get_apx_file_type() == FileType::RequirePortData)
      {
         return process_new_require_port_data_file(file);
      }
      return APX_NO_ERROR;
   }

   error_t ClientConnection::attach_node_instance(NodeInstance* node_instance)
   {
      return node_instance->attach_to_file_manager(&m_file_manager);
   }

   void ClientConnection::send_greeting_header()
   {
      std::stringstream ss;
      int num_header_format = 32;
      ss << "RMFP/1.0\n";
      ss << "NumHeader-Format:" << std::to_string(num_header_format) << "\n";
      ss << "\n";
      std::string const& str{ ss.str() };
      std::int32_t bytes_available;
      transmit_begin();
      transmit_direct_message(reinterpret_cast<const std::uint8_t*>(str.data()), static_cast<std::int32_t>(str.size()), bytes_available);
      transmit_end();
   }

   int ClientConnection::on_data_received(std::uint8_t const* data, std::size_t data_size, std::size_t& parse_len)
   {
      if (data == nullptr)
      {
         set_data_reception_error(APX_NULL_PTR_ERROR);
         return -1;
      }
      std::size_t total_parse_len = 0u;
      std::uint8_t const* next = data;
      std::uint8_t const* end = data + data_size;
      while (next < end)
      {
         std::uint8_t const* result;
         apx::error_t error_code{ APX_NO_ERROR };
         result = parse_message(next, end, error_code);
         if ( error_code == APX_NO_ERROR )
         {
            assert((result >= next) && (result <= end));
            if (result == next)
            {
               // No more complete messages can be parsed. There may be a partial
               // message left in buffer, but we leave it in the buffer until
               // more data has arrived.
               break;
            }
            next = result;
            total_parse_len = next - data;
            assert(total_parse_len <= data_size);
         }
         else
         {
            set_data_reception_error(error_code);
            return -1;
         }
      }
      parse_len = total_parse_len;
      return 0;
   }

   std::uint8_t const* ClientConnection::parse_message(std::uint8_t const* begin, std::uint8_t const* end, apx::error_t& error_code)
   {
      std::uint8_t const* msg_end = nullptr;
      error_code = APX_NO_ERROR;
      if (begin < end)
      {
         std::uint32_t msg_size{ 0u };
         auto header_size = numheader::decode32(begin, end, msg_size);
         if (header_size == 0)
         {
            error_code = APX_PARSE_ERROR;
            return nullptr;
         }
         std::uint8_t const* msg_data = begin + header_size;
         msg_end = msg_data + msg_size;
         if (m_is_greeting_accepted)
         {
            error_code = m_file_manager.message_received(msg_data, msg_size);
         }
         else
         {
            if (is_greeting_accepted(msg_data, msg_size, error_code))
            {
               greeting_header_accepted();
            }
            else
            {
               return nullptr;
            }
         }
      }
      else
      {
         error_code = APX_PARSE_ERROR;
         return nullptr;
      }
      return msg_end;
   }

   void ClientConnection::set_data_reception_error(apx::error_t error_code)
   {
      std::cout << "Data reception error: " << static_cast<int>(error_code) << std::endl;
   }

   bool ClientConnection::is_greeting_accepted(std::uint8_t const* msg_data, std::size_t msg_size, apx::error_t& error_code)
   {
      std::uint32_t address;
      bool more_bit{ false };
      std::uint8_t const* const msg_end = msg_data + msg_size;
      std::size_t header_size = rmf::address_decode(msg_data, msg_data + msg_size, address, more_bit);
      if (address == rmf::CMD_AREA_START_ADDRESS)
      {
         std::uint32_t cmd_type{ 0u };
         std::size_t decode_size = rmf::decode_cmd_type(msg_data + header_size, msg_end, cmd_type);
         if ((decode_size == rmf::CMD_TYPE_SIZE) && (cmd_type == rmf::CMD_ACK_MSG))
         {
            return true;
         }
      }
      else
      {
         error_code = APX_INVALID_MSG_ERROR;
      }
      return false;
   }

   error_t  ClientConnection::process_new_require_port_data_file(File* file)
   {
      auto base_name = file->get_file_info().base_name();
      auto* node_instance = m_node_manager->find(base_name);
      if (node_instance != nullptr)
      {
         assert(node_instance->get_require_port_data_state() == PortDataState::WaitingForFileInfo);
         file->set_notification_handler(node_instance);
         node_instance->Set_provide_port_data_state(PortDataState::WaitingForFileData);
         return m_file_manager.send_open_file_request(file->get_address_without_flags());
      }
      return APX_NO_ERROR; //Ignore this file since it doesn't match anything in our node manager
   }

#ifdef UNIT_TEST
   void ClientConnection::run()
   {
      m_file_manager.run();
   }
#else
   void ClientConnection::start()
   {
      m_file_manager.start();
   }
   void ClientConnection::stop()
   {
      m_file_manager.stop();
   }
#endif

}