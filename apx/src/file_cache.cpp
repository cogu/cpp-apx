/*****************************************************************************
* \file      file_cache.cpp
* \author    Conny Gustafsson
* \date      2020-11-01
* \brief     Manages serialization/deserialization of node instances to/from filesystem
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
#include <string>
#include <fstream>
#include <array>
#include <charconv>
#include <cassert>
#include "cpp-apx/file_cache.h"
#include "cpp-apx/vmdefs.h"
#include "cpp-apx/sha256.h"
#include "cpp-apx/numheader.h"

namespace fs = std::filesystem;

using namespace std::string_literals;

namespace apx
{
   FileCache::FileCache()
   {
      m_directory = fs::current_path() / ".apx"s;
   }

   void FileCache::set_directory(std::filesystem::path const& directory)
   {
      m_directory = directory;
   }

   apx::error_t FileCache::store(NodeInstance const* node_instance)
   {
      if (node_instance == nullptr)
      {
         return APX_INVALID_ARGUMENT_ERROR;
      }
      auto const* definition_data = node_instance->get_definition_data();
      auto const definition_size = node_instance->get_definition_size();
      if (definition_data == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      if (definition_size == 0)
      {
         return APX_LENGTH_ERROR;
      }
      std::array<std::uint8_t, SHA256_HASH_SIZE> hash;
      if (!sha256::calc(hash.data(), hash.size(), definition_data, definition_size))
      {
         return APX_INTERNAL_ERROR;
      }
      std::string const hash_suffix = create_hash_suffix(hash.data(), hash.size());
      if (hash_suffix.size() == 0)
      {
         return APX_INTERNAL_ERROR;
      }
      auto retval = create_apx_definition_file(node_instance->get_name(), hash_suffix, definition_data, definition_size);
      if (retval == APX_NO_ERROR)
      {
         retval = serialize_apx_node(node_instance, hash_suffix, hash.data(), hash.size());
      }
      return retval;
   }

   std::string FileCache::create_hash_suffix(std::uint8_t const* hash_data, std::size_t hash_size) const
   {
      std::string retval{ "-" };
      if ((hash_data != nullptr) && (hash_size == SHA256_HASH_SIZE))
      {
         constexpr int hex_base = 16;
         constexpr std::size_t num_chars = sizeof(char) * 2;
         std::array<char, 3> buf = { '0', 0, 0 }; //need 1 extra byte since std::to_chars does not perform null-padding
         char* begin = buf.data() + sizeof(char); //By default, write in the middle of the buffer
         char* end = begin + buf.size() - sizeof(char);
         char const* with_leading_zero = buf.data();
         for (std::size_t i = (SHA256_HASH_SIZE - SHA_BYTES_IN_SUFFIX); i < SHA256_HASH_SIZE; i++)
         {
            auto [ptr, err] = std::to_chars(begin, end, hash_data[i], hex_base);
            if (err == std::errc())
            {
               if (ptr < end)
               {
                  retval.append(with_leading_zero, num_chars); //std::to_chars wrote 1 character to buf, manually add leading zero
               }
               else
               {
                  retval.append(begin, num_chars); //std::to_chars wrote 2 characters to buf, use both characters
               }
            }
            else
            {
               return std::string();
            }
         }
      }
      return retval;
   }

   apx::error_t FileCache::serialize_apx_node(NodeInstance const* node_instance, std::string const& hash_suffix, std::uint8_t const* hash_data, std::size_t hash_size)
   {
      fs::path path_to_file = m_directory / (node_instance->get_name() + hash_suffix + m_apx_node_file_extension);
      std::ofstream file(path_to_file, std::ios::out | std::ios::binary | std::ios::trunc);

      if (file.is_open())
      {
         write_version_header(&file);
         write_node_name_to_header(&file, node_instance);
         write_node_size_to_header(&file, node_instance);
         write_hash_to_header(&file, hash_data, hash_size);
         (void)write_number_of_data_elements(&file, node_instance);//TODO: Error handling
         (void)write_number_computation_lists(&file, node_instance);//TODO: Error handling
         (void)write_number_of_ports(&file, node_instance); //TODO: Error handling
         if (node_instance->get_num_data_elements() > 0u)
         {
            serialize_data_elements(&file, node_instance);
         }
         if (node_instance->get_num_provide_ports() > 0u)
         {
            serialize_provide_ports(&file, node_instance);
         }
         if (node_instance->get_num_require_ports() > 0u)
         {
            serialize_require_ports(&file, node_instance);
         }
         file.close();
      }
      else
      {
         return APX_FILE_NOT_OPEN_ERROR;
      }
      return APX_NO_ERROR;
   }

   apx::error_t FileCache::create_apx_definition_file(std::string const& node_name, std::string const& hash_suffix, std::uint8_t const* definition_data, std::size_t definition_size)
   {
      fs::path path_to_file = m_directory / (node_name + hash_suffix + m_apx_definition_file_extension);
      std::ofstream file(path_to_file, std::ios::out | std::ios::binary | std::ios::trunc);
      if (file.is_open())
      {
         file.write(reinterpret_cast<char const*>(definition_data), definition_size);
         file.close();
      }
      else
      {
         return APX_FILE_NOT_OPEN_ERROR;
      }
      return APX_NO_ERROR;
   }

   void FileCache::write_version_header(std::basic_ostream<char>* stream) const
   {
      std::array<char, bin::VERSION_HEADER_SIZE> header = { 'A', 'P', 'X', bin::HEADER_MAJOR_VERSION, bin::HEADER_MINOR_VERSION,
                                              'V', 'M', vm::MAJOR_VERSION, vm::MINOR_VERSION };
      stream->write(header.data(), header.size());
   }

   void FileCache::write_node_name_to_header(std::basic_ostream<char>* stream, NodeInstance const* node_instance) const
   {
      std::string const& name = node_instance->get_name();
      stream->write(name.data(), name.size());
      stream->put('\0');
   }

   bool FileCache::write_node_size_to_header(std::basic_ostream<char>* stream, NodeInstance const* node_instance) const
   {
      std::array<std::uint8_t, sizeof(std::uint32_t)> buf;
      std::size_t const definition_size = node_instance->get_definition_size();
      if constexpr (sizeof(std::size_t) > sizeof(std::uint32_t))
      {
         if (definition_size > UINT32_MAX)
         {
            return false;
         }
      }
      std::size_t bytes_written = numheader::encode32(buf.data(), buf.data() + buf.size(), static_cast<std::uint32_t>(definition_size));
      if (bytes_written == 0)
      {
         return false;
      }
      stream->write(reinterpret_cast<char const*>(buf.data()), bytes_written);
      return true;
   }

   void FileCache::write_hash_to_header(std::basic_ostream<char>* stream, std::uint8_t const* hash_data, std::size_t hash_size) const
   {
      stream->write(reinterpret_cast<char const*>(hash_data), hash_size);
   }

   apx::error_t FileCache::write_number_of_data_elements(std::basic_ostream<char>* stream, NodeInstance const* node_instance)
   {
      return write_integer_to_stream(stream, node_instance->get_num_data_elements());
   }

   apx::error_t FileCache::write_number_computation_lists(std::basic_ostream<char>* stream, NodeInstance const* node_instance)
   {
      return write_integer_to_stream(stream, node_instance->get_num_computation_lists());
   }

   apx::error_t FileCache::write_number_of_ports(std::basic_ostream<char>* stream, NodeInstance const* node_instance)
   {
      apx::error_t retval = write_integer_to_stream(stream, node_instance->get_num_provide_ports());
      if (retval == APX_NO_ERROR)
      {
         retval = write_integer_to_stream(stream, node_instance->get_num_require_ports());
      }
      return retval;
   }
   apx::error_t FileCache::serialize_data_elements(std::basic_ostream<char>* stream, NodeInstance const* node_instance)
   {
      stream->put(bin::START_OF_DATA_ELEMENTS);
      element_id_t const num_data_elements = static_cast<element_id_t>(node_instance->get_num_data_elements());
      for (element_id_t id = 0u; id < num_data_elements; id++)
      {
         auto const* data_element = node_instance->get_data_element(id);
         if (data_element == nullptr)
         {
            return APX_NULL_PTR_ERROR;
         }
         write_string_with_null_terminator(stream, data_element->to_string());
      }
      return APX_NO_ERROR;
   }

   apx::error_t FileCache::serialize_provide_ports(std::basic_ostream<char>* stream, NodeInstance const* node_instance)
   {
      stream->put(bin::START_OF_PROVIDE_PORTS);
      auto const port_data_size = node_instance->get_provide_port_init_data_size();
      auto retval = write_integer_to_stream(stream, port_data_size);
      if (port_data_size > 0)
      {
         //TODO: If init data just contains zeros, we don't actually need to store it
         stream->put(bin::START_OF_INIT_DATA);
         stream->write(reinterpret_cast<char const*>(node_instance->get_provide_port_init_data()), port_data_size);
      }
      std::size_t const num_ports = node_instance->get_num_provide_ports();
      stream->put(bin::START_OF_PORT_INSTANCES);
      for (std::size_t port_id = 0u; port_id < num_ports; port_id++)
      {
         auto const* port_instance = node_instance->get_provide_port(static_cast<port_id_t>(port_id));
         if (port_instance == nullptr)
         {
            return APX_NULL_PTR_ERROR;
         }
         retval = serialize_port_instance(stream, port_instance);
         if (retval != APX_NO_ERROR)
         {
            break;
         }
         stream->put(bin::END_OF_RECORD);
      }
      return retval;
   }
   apx::error_t FileCache::serialize_require_ports(std::basic_ostream<char>* stream, NodeInstance const* node_instance)
   {
      stream->put(bin::START_OF_REQUIRE_PORTS);
      auto const port_data_size = node_instance->get_require_port_init_data_size();
      auto retval = write_integer_to_stream(stream, port_data_size);
      if (port_data_size > 0)
      {
         //TODO: If init data just contains zeros, we don't actually need to store it
         stream->put(bin::START_OF_INIT_DATA);
         stream->write(reinterpret_cast<char const*>(node_instance->get_require_port_init_data()), port_data_size);
      }
      std::size_t const num_ports = node_instance->get_num_require_ports();
      stream->put(bin::START_OF_PORT_INSTANCES);
      for (std::size_t port_id = 0u; port_id < num_ports; port_id++)
      {
         auto const* port_instance = node_instance->get_require_port(static_cast<port_id_t>(port_id));
         if (port_instance == nullptr)
         {
            return APX_NULL_PTR_ERROR;
         }
         retval = serialize_port_instance(stream, port_instance);
         if (retval != APX_NO_ERROR)
         {
            break;
         }
         stream->put(bin::END_OF_RECORD);
      }
      return retval;
   }

   apx::error_t FileCache::serialize_port_instance(std::basic_ostream<char>* stream, PortInstance const* port_instance)
   {
      write_string_with_null_terminator(stream, port_instance->name());
      auto retval = write_integer_to_stream(stream, port_instance->data_element_id());
      if (retval == APX_NO_ERROR)
      {
         //TODO: Optional computation reference
         if (port_instance->port_type() == PortType::ProvidePort)
         {
            auto program = port_instance->pack_program();
            std::size_t program_size = program.size() - vm::INITIAL_HEADER_SIZE;
            stream->put(bin::START_OF_PACK_PROGRAM);
            retval = write_integer_to_stream(stream, program_size);
            if (retval == APX_NO_ERROR)
            {
               stream->write(reinterpret_cast<char const*>(program.data()) + vm::INITIAL_HEADER_SIZE, program_size);
            }
         }
         else
         {
            auto program = port_instance->unpack_program();
            std::size_t program_size = program.size() - vm::INITIAL_HEADER_SIZE;
            stream->put(bin::START_OF_UNPACK_PROGRAM);
            retval = write_integer_to_stream(stream, program_size);
            if (retval == APX_NO_ERROR)
            {
               stream->write(reinterpret_cast<char const*>(program.data()) + vm::INITIAL_HEADER_SIZE, program_size);
            }
         }
      }
      return retval;
   }

   apx::error_t FileCache::write_integer_to_stream(std::basic_ostream<char>* stream, std::size_t value)
   {
      std::array<std::uint8_t, sizeof(std::uint32_t)> bytes;
      if (value > static_cast<std::size_t>(INT32_MAX))
      {
         return APX_NUMBER_TOO_LARGE_ERROR;
      }
      std::size_t bytes_written = numheader::encode32(bytes.data(), bytes.data() + bytes.size(), static_cast<std::uint32_t>(value));
      assert(bytes_written > 0);
      stream->write(reinterpret_cast<char const*>(bytes.data()), bytes_written);
      return APX_NO_ERROR;
   }

   void FileCache::write_string_with_null_terminator(std::basic_ostream<char>* stream, std::string const& str)
   {
      stream->write(str.data(), str.size());
      stream->put('\0');
   }
}