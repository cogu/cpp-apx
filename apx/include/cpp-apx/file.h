/*****************************************************************************
* \file      file.h
* \author    Conny Gustafsson
* \date      2020-11-06
* \brief     APX file class
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
#include "cpp-apx/remotefile.h"
#include "cpp-apx/file_info.h"
#include "cpp-apx/types.h"

namespace apx
{
   enum class FileType : unsigned char
   {
      Unknown,
      Definition,            //".apx"
      ProvidePortData,       //".out"
      RequirePortData,       //".in"
      ProvidePortCount,      //".cout"
      RequirePortCount,      //".cin"
   };

   std::string const& file_type_to_extension(FileType file_type);

   class File;
   class FileManager;

   class FileNotificationHandler
   {
   public:
      virtual error_t file_open_notify(File* file) = 0;
      virtual error_t file_close_notify(File* file) = 0;
      virtual error_t file_write_notify(File* file, std::uint32_t offset, std::uint8_t const* data, std::size_t size) = 0;
   };


   class File
   {
   public:
      File() = delete;
      File(File const& other) = default;
      File(rmf::FileInfo const& file_info);
      bool is_local() { return (m_file_info.address_without_flags() != rmf::INVALID_ADDRESS) && ((m_file_info.address & rmf::REMOTE_ADDRESS_BIT) == 0); }
      bool is_remote() { return (m_file_info.address_without_flags() != rmf::INVALID_ADDRESS) && ((m_file_info.address & rmf::REMOTE_ADDRESS_BIT) != 0); }
      bool has_valid_address() { return (m_file_info.address_without_flags() != rmf::INVALID_ADDRESS); }
      FileType get_apx_file_type() { return m_apx_file_type; }
      std::uint8_t const* get_digest_data() { return m_file_info.digest_data.data(); }
      static bool less_than(File const& a, File const& b);
      static bool ptr_less_than(File const*& a, File const*& b);
      std::uint32_t get_address() const { return m_file_info.address; }
      void set_address(std::uint32_t address) { m_file_info.address = address; }
      std::uint32_t get_address_without_flags() const { return m_file_info.address_without_flags(); }
      std::string const& get_name() const { return m_file_info.name; }
      std::uint32_t get_end_address() const { return (m_file_info.address + m_file_info.size); }
      std::uint32_t get_end_address_without_flags() const { return (m_file_info.address_without_flags() + m_file_info.size); }
      bool address_in_range(std::uint32_t address) const { return m_file_info.address_in_range(address); }
      rmf::FileInfo const& get_file_info() const { return m_file_info; }
      rmf::FileInfo* clone_file_info() const { return new rmf::FileInfo(m_file_info); }
      bool is_open() const { return m_is_file_open; }
      void open() { m_is_file_open = true; }
      void close() { m_is_file_open = false; }
      void open_notify();
      void close_notify();
      error_t write_notify(std::uint32_t offset, std::uint8_t const* data, std::size_t size);
      void set_file_manager(FileManager* file_manager) { m_file_manager = file_manager; }
      FileManager* get_file_manager() { return m_file_manager; }
      void set_notification_handler(FileNotificationHandler* handler) { m_notification_handler = handler; }
      FileNotificationHandler* get_notification_handler() { return m_notification_handler; }

   protected:
      bool m_is_file_open{ false };
      bool m_has_first_write{ false }; //only applies to remotely openend files

      FileType m_apx_file_type { FileType::Unknown };
      rmf::FileInfo m_file_info;
      FileManager* m_file_manager{ nullptr };
      FileNotificationHandler* m_notification_handler{ nullptr };

   };

}

