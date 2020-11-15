/*****************************************************************************
* \file      file_info.cpp
* \author    Conny Gustafsson
* \date      2020-11-05
* \brief     Remotefile file info struct
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
#include <cstring>
#include <cassert>
#include "cpp-apx/file_info.h"
#include "cpp-apx/pack.h"

namespace rmf
{
   FileInfo::FileInfo(char const* _name, std::uint32_t _size, std::uint32_t _address):
      address{ _address }, size{ _size }, name{ _name }
   {
      std::memset(digest_data.data(), 0, digest_data.size());
   }
   FileInfo::FileInfo(char const* _name, std::uint32_t _size, DigestType _digest_type, std::uint8_t const* _digest_data):
      address{ INVALID_ADDRESS }, size{ _size }, rmf_file_type{FileType::Fixed}, digest_type{_digest_type}, name{ _name }
   {
      std::memset(digest_data.data(), 0, digest_data.size());
      if (_digest_data != nullptr)
      {
         if (digest_type == DigestType::SHA256)
         {
            std::memcpy(digest_data.data(), _digest_data, digest_data.size());
         }
      }
   }

   FileInfo::FileInfo(char const* _name, std::uint32_t _size, std::uint32_t _address, DigestType _digest_type, std::uint8_t const* _digest_data):
      address{ _address }, size{ _size }, rmf_file_type{ FileType::Fixed }, digest_type{ _digest_type }, name{ _name }
      {
         std::memset(digest_data.data(), 0, digest_data.size());
         if (_digest_data != nullptr)
         {
            if (digest_type == DigestType::SHA256)
            {
               std::memcpy(digest_data.data(), _digest_data, digest_data.size());
            }
         }
      }

   FileInfo::FileInfo(std::string const& _name, std::uint32_t _size, std::uint32_t _address):
      address{ _address }, size{ _size }, name{_name}
   {
      std::memset(digest_data.data(), 0, digest_data.size());
   }
   FileInfo::FileInfo(std::string const& _name, std::uint32_t _size, std::uint32_t _address, FileType file_type, DigestType _digest_type, std::uint8_t const* _digest_data):
      address{ _address }, size{ _size }, rmf_file_type{file_type}, digest_type{_digest_type}, name{ _name }
   {
      std::memset(digest_data.data(), 0, digest_data.size());
      if (_digest_data != nullptr)
      {
         if (digest_type == DigestType::SHA256)
         {
            std::memcpy(digest_data.data(), _digest_data, digest_data.size());
         }
      }
   }

   bool FileInfo::name_ends_with(char const* suffix) const
   {
      std::size_t const suffix_size = std::strlen(suffix);
      std::size_t const name_size = name.size();
      return (name_size >= suffix_size) && (0 == name.compare(name_size - suffix_size, suffix_size, suffix));
   }

   bool FileInfo::name_ends_with(std::string const& suffix) const
   {
      std::size_t const suffix_size = suffix.size();
      std::size_t const name_size = name.size();
      return (name_size >= suffix_size) && (0 == name.compare(name_size - suffix_size, suffix_size, suffix));
   }

   std::string FileInfo::base_name() const
   {
      if(name.size() > 0u)
      {
         std::size_t dot = name.find('.');
         if (dot != std::string::npos)
         {
            return name.substr(0u, dot);
         }
      }
      return std::string();
   }

   apx::error_t encode_publish_file_cmd(std::uint8_t* buf, std::size_t buf_size, rmf::FileInfo const* file, std::size_t& cmd_size)
   {
      std::size_t const name_size = file->name.length();
      std::size_t const required_size = rmf::FILE_INFO_HEADER_SIZE + name_size + 1u ; //Add 1 for null-terminator
      if (required_size > buf_size)
      {
         return APX_BUFFER_TOO_SMALL_ERROR;
      }
      std::uint8_t* p{ buf };
      apx::packLE<std::uint32_t>(p, rmf::CMD_PUBLISH_FILE_MSG); p += sizeof(std::uint32_t);
      apx::packLE<std::uint32_t>(p, file->address_without_flags()); p += sizeof(std::uint32_t);
      apx::packLE<std::uint32_t>(p, file->size); p += sizeof(std::uint32_t);
      apx::packLE<std::uint16_t>(p, static_cast<std::uint16_t>(file->rmf_file_type)); p += sizeof(std::uint16_t);
      apx::packLE<std::uint16_t>(p, static_cast<std::uint16_t>(file->digest_type)); p += sizeof(std::uint16_t);
      switch (file->digest_type)
      {
      case DigestType::None:
         std::memset(p, 0, SHA256_SIZE);
         break;
      case DigestType::SHA1:
         std::memcpy(p, file->digest_data.data(), SHA1_SIZE);
         std::memset(p + SHA1_SIZE, 0, SHA256_SIZE - SHA1_SIZE);
         break;
      case DigestType::SHA256:
         std::memcpy(p, file->digest_data.data(), SHA256_SIZE);
         break;
      }
      p += SHA256_SIZE;
      assert((p + name_size + 1) == buf + required_size);
      std::memcpy(p, reinterpret_cast<std::uint8_t const*>(file->name.data()), name_size); p += name_size;
      *p = 0u;
      cmd_size = required_size;
      return APX_NO_ERROR;
   }
}
