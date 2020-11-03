/*****************************************************************************
* \file      file_cache.h
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
#pragma once

#include <filesystem>
#include <memory>
#include <vector>
#include <iostream>
#include "cpp-apx/node_instance.h"
#include "cpp-apx/error.h"

namespace apx
{
   constexpr std::size_t NODE_VERSION_HEADER_SIZE = 9u;
   constexpr std::uint8_t NODE_HEADER_MAJOR_VERSION = 0u;
   constexpr std::uint8_t NODE_HEADER_MINOR_VERSION = 2u;
   constexpr std::size_t SHA256_HASH_SIZE = 32u;
   constexpr std::size_t SHA_BYTES_IN_SUFFIX = 4u; //Number of characters in suffix name is twice this number

   class FileCache
   {
   public:
      FileCache();
      void set_directory(std::filesystem::path const& directory);
      apx::error_t store(NodeInstance const* node_instance);
   protected:
      std::filesystem::path m_directory;
      std::string const m_apx_node_file_extension{ ".apxnode" };
      std::string const m_apx_definition_file_extension{ ".apx" };

      std::string create_hash_suffix(std::uint8_t const* hash_data, std::size_t hash_size) const;
      apx::error_t serialize_apx_node(NodeInstance const* node_instance, std::string const& hash_suffix, std::uint8_t const* hash_data, std::size_t hash_size);
      apx::error_t create_apx_definition_file(std::string const& node_name, std::string const& hash_suffix, std::uint8_t const* definition_data, std::size_t definition_size);
      void write_version_header(std::basic_ostream<char>* stream) const;
      void write_node_name_to_header(std::basic_ostream<char>* stream, NodeInstance const* node_instance) const;
      bool write_node_size_to_header(std::basic_ostream<char>* stream, NodeInstance const* node_instance) const;
      void write_hash_to_header(std::basic_ostream<char>* stream, std::uint8_t const* hash_data, std::size_t hash_size) const;
      apx::error_t write_number_of_data_elements(std::basic_ostream<char>* stream, NodeInstance const* node_instance);
      apx::error_t write_number_computation_lists(std::basic_ostream<char>* stream, NodeInstance const* node_instance);
      apx::error_t write_number_of_ports(std::basic_ostream<char>* stream, NodeInstance const* node_instance);
      apx::error_t serialize_data_elements(std::basic_ostream<char>* stream, NodeInstance const* node_instance);
      apx::error_t serialize_provide_port_data(std::basic_ostream<char>* stream, NodeInstance const* node_instance);
      apx::error_t serialize_require_port_data(std::basic_ostream<char>* stream, NodeInstance const* node_instance);
      apx::error_t serialize_port_instance(std::basic_ostream<char>* stream, PortInstance const* port_instance);
      apx::error_t write_byte_array_to_stream_with_size_header(std::basic_ostream<char>* stream, std::uint8_t const* data, std::size_t size);
      apx::error_t write_integer_to_stream(std::basic_ostream<char>* stream, std::size_t value);
      void write_string_with_null_terminator(std::basic_ostream<char>* stream, std::string const& str);
   };
}

