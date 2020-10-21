/*****************************************************************************
* \file      file_client.cc
* \author    Conny Gustafsson
* \date      2020-10-21
* \brief     An APX client that operates on files in local file system
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
#include <fstream>
#include "cpp-apx/file_client.h"

namespace fs = std::filesystem;

namespace apx
{
   static std::unique_ptr<std::uint8_t[]> read_buffer_from_file(fs::path const& path_to_file, std::size_t& size)
   {

      std::unique_ptr<std::uint8_t[]> retval;

      std::ifstream file(path_to_file, std::ios::in | std::ios::binary | std::ios::ate);
      if (file.is_open())
      {
         std::streampos file_size = file.tellg();
         if (file_size > 0u)
         {
            size = static_cast<std::size_t>(file_size);
            retval.reset(new std::uint8_t[size]);
            file.seekg(0, std::ios::beg);
            file.read(reinterpret_cast<char*>(retval.get()), size);
            file.close();
         }
         else
         {
            //TODO: What to do when file is not open?
         }
      }
      return std::move(retval);
   }

   FileClient::FileClient()
   {
      m_directory = fs::current_path();
   }

   apx::error_t FileClient::build_node(char const* apx_definition)
   {
      auto result = m_node_manager.build_node(apx_definition);
      if (result != APX_NO_ERROR)
      {
         return result;
      }
      m_node = m_node_manager.get_last_attached();
      return read_data_from_filesystem();
   }

   apx::error_t FileClient::save_all()
   {
      auto retval = save_provide_port_data();
      if (retval == APX_NO_ERROR)
      {
         retval = save_require_port_data();
      }
      return retval;
   }

   apx::error_t FileClient::save_if_new()
   {
      if (m_node == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      apx::error_t retval = APX_NO_ERROR;
      std::string const& node_name = m_node->get_name();
      auto file_path = m_directory / (node_name + ".out");
      if (!fs::exists(file_path))
      {
         retval = save_provide_port_data();
      }
      file_path = m_directory / (node_name + ".in");
      if ( (retval == APX_NO_ERROR) && (!fs::exists(file_path)) )
      {
         retval = save_require_port_data();
      }
      return retval;
   }

   apx::error_t FileClient::save_provide_port_data()
   {
      if (m_node == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      std::string const& node_name = m_node->get_name();
      auto const* node_data = m_node->get_const_node_data();
      auto data_size = node_data->provide_port_data_size();
      apx::error_t retval = APX_NO_ERROR;
      auto const file_path = m_directory / (node_name + ".out");
      if (data_size > 0u)
      {
         std::uint8_t* data = new std::uint8_t[data_size];
         retval = node_data->read_provide_port_data(0, data, data_size);
         if (retval == APX_NO_ERROR)
         {
            save_buffer_to_file(file_path, data, data_size);
         }
         delete[] data;
      }
      return retval;
   }

   apx::error_t FileClient::save_require_port_data()
   {
      if (m_node == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      std::string const& node_name = m_node->get_name();
      auto const* node_data = m_node->get_const_node_data();
      auto data_size = node_data->require_port_data_size();
      apx::error_t retval = APX_NO_ERROR;
      auto const file_path = m_directory / (node_name + ".in");
      if (data_size > 0u)
      {
         std::uint8_t* data = new std::uint8_t[data_size];
         retval = node_data->read_require_port_data(0, data, data_size);
         if (retval == APX_NO_ERROR)
         {
            save_buffer_to_file(file_path, data, data_size);
         }
         delete[] data;
      }
      return retval;
   }

   bool FileClient::has_provide_port_data()
   {
      return (m_node != nullptr) && m_node->has_provide_port_data();
   }

   bool FileClient::has_require_port_data()
   {
      return (m_node != nullptr) && m_node->has_require_port_data();
   }

   apx::error_t FileClient::write_port(apx::port_id_t port_id, dtl::ScalarValue sv)
   {
      if (m_node == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      auto* port = m_node->get_provide_port(static_cast<std::size_t>(port_id));
      if (port == nullptr)
      {
         return APX_INVALID_ARGUMENT_ERROR;
      }
      auto const data_size = port->get_data_size();
      if (m_buffer.size() < data_size)
      {
         m_buffer.resize(data_size);
      }
      apx::error_t retval = APX_NO_ERROR;
      retval = m_vm.set_write_buffer(m_buffer.data(), data_size);
      if (retval == APX_NO_ERROR)
      {
         retval = m_vm.select_program(port->get_pack_program());
      }
      if (retval == APX_NO_ERROR)
      {
         retval = m_vm.pack_value(sv);
      }
      if (retval == APX_NO_ERROR)
      {
         auto node_data = m_node->get_node_data();
         retval = node_data->write_provide_port_data(port->get_data_offset(), m_buffer.data(), data_size);
      }
      return retval;
   }

   apx::error_t FileClient::read_port(apx::port_id_t port_id, dtl::ScalarValue& sv)
   {
      if (m_node == nullptr)
      {
         return APX_NULL_PTR_ERROR;
      }
      auto* port = m_node->get_require_port(static_cast<std::size_t>(port_id));
      if (port == nullptr)
      {
         return APX_INVALID_ARGUMENT_ERROR;
      }
      auto const data_size = port->get_data_size();
      if (m_buffer.size() < data_size)
      {
         m_buffer.resize(data_size);
      }
      apx::error_t retval = APX_NO_ERROR;
      auto node_data = m_node->get_node_data();
      retval = node_data->read_require_port_data(port->get_data_offset(), m_buffer.data(), data_size);
      if (retval == APX_NO_ERROR)
      {
         retval = m_vm.set_read_buffer(m_buffer.data(), data_size);
      }
      if (retval == APX_NO_ERROR)
      {
         retval = m_vm.select_program(port->get_unpack_program());
      }
      if (retval == APX_NO_ERROR)
      {
         retval = m_vm.unpack_value(sv);
      }
      return retval;
   }

   void FileClient::save_buffer_to_file(std::filesystem::path file_path, std::uint8_t const* buffer, std::size_t size)
   {
      std::ofstream stream;
      stream.open(file_path, std::ios::out | std::ios::trunc | std::ios::binary);
      if (stream.is_open())
      {
         stream.write(reinterpret_cast<char const*>(buffer), size);
         stream.close();
      }
   }

   apx::error_t FileClient::read_data_from_filesystem()
   {
      apx::error_t retval = APX_NO_ERROR;
      if (m_node->has_require_port_data())
      {
         retval = read_require_port_data_from_file();
      }
      if (retval == APX_NO_ERROR && m_node->has_provide_port_data())
      {
         retval = read_provide_port_data_from_file();
      }
      return retval;
   }

   apx::error_t FileClient::read_require_port_data_from_file()
   {
      auto path_to_file = m_directory / (m_node->get_name() + ".in");
      if (fs::exists(path_to_file))
      {
         std::size_t size{ 0u };
         auto buf = read_buffer_from_file(path_to_file, size);
         if (buf.get() != nullptr && (size > 0))
         {
            auto node_data = m_node->get_node_data();
            if (node_data->require_port_data_size() != size)
            {
               return APX_LENGTH_ERROR;
            }
            node_data->write_require_port_data(0u, buf.get(), size);
         }
         else
         {
            return APX_READ_ERROR;
         }
      }
      else
      {
         //Use (already) calculated port init values
      }
      return APX_NO_ERROR;
   }

   apx::error_t FileClient::read_provide_port_data_from_file()
   {
      auto path_to_file = m_directory / (m_node->get_name() + ".out");
      if (fs::exists(path_to_file))
      {
         std::size_t size{ 0u };
         auto buf = read_buffer_from_file(path_to_file, size);
         if (buf.get() != nullptr && (size > 0))
         {
            auto node_data = m_node->get_node_data();
            if (node_data->provide_port_data_size() != size)
            {
               return APX_LENGTH_ERROR;
            }
            node_data->write_provide_port_data(0u, buf.get(), size);
         }
         else
         {
            return APX_READ_ERROR;
         }
      }
      else
      {
         //Use (already) calculated port init values
      }
      return APX_NO_ERROR;
   }
}