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
#include "bstr/bstr.hpp"

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

   std::size_t encode_publish_file_cmd(std::uint8_t* buf, std::size_t buf_size, rmf::FileInfo const* file)
   {
      std::size_t const name_size = file->name.length();
      std::size_t const required_size = rmf::FILE_INFO_HEADER_SIZE + name_size + 1u ; //Add 1 for null-terminator
      if (required_size > buf_size)
      {
         return 0u;
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
      return required_size;
   }

   std::size_t decode_publish_file_cmd(std::uint8_t const* buf, std::size_t buf_size, FileInfo& file_info)
   {
      std::uint8_t const* next{ buf };
      std::uint8_t const* end{ buf + buf_size };
      if ((next + FILE_INFO_HEADER_SIZE) < end)
      {
         std::uint32_t const cmd_type = apx::unpackLE<std::uint32_t>(next); next += sizeof(std::uint32_t);
         if (cmd_type != CMD_PUBLISH_FILE_MSG)
         {
            //Invalid command type
            return 0u;
         }
         file_info.address = apx::unpackLE<std::uint32_t>(next); next += sizeof(std::uint32_t);
         file_info.size = apx::unpackLE<std::uint32_t>(next); next += sizeof(std::uint32_t);
         std::uint16_t value = apx::unpackLE<std::uint16_t>(next); next += sizeof(std::uint16_t);
         if (!rmf::value_to_file_type(value, file_info.rmf_file_type))
         {
            return 0u;
         }
         value = apx::unpackLE<std::uint16_t>(next); next += sizeof(std::uint16_t);
         if (!rmf::value_to_digest_type(value, file_info.digest_type))
         {
            return 0u;
         }
         switch (file_info.digest_type)
         {
         case DigestType::None:
            std::memset(file_info.digest_data.data(), 0, SHA256_SIZE);
            break;
         case DigestType::SHA1:
            std::memcpy(file_info.digest_data.data(), next, SHA1_SIZE);
            std::memset(file_info.digest_data.data() + SHA1_SIZE, 0, SHA256_SIZE - SHA1_SIZE);
            break;
         case DigestType::SHA256:
            std::memcpy(file_info.digest_data.data(), next, SHA256_SIZE);
            break;
         }
         next += SHA256_SIZE;
         std::uint8_t const* result = bstr::while_predicate<std::uint8_t>(next, end, [](auto c) -> bool { return c != 0; });
         if ((result > next) && (result < end) )
         {
            file_info.name.assign(next, result);
         }
         return (result - buf);
      }
      return 0u;
   }

   bool value_to_file_type(std::uint16_t value, FileType& file_type)
   {
      switch (value)
      {
      case FILE_TYPE_FIXED:
         file_type = FileType::Fixed;
         break;
      case FILE_TYPE_DYNAMIC8:
         file_type = FileType::Dynamic8;
         break;
      case FILE_TYPE_DYNAMIC16:
         file_type = FileType::Dynamic16;
         break;
      case FILE_TYPE_DYNAMIC32:
         file_type = FileType::Dynamic32;
         break;
      case FILE_TYPE_DEVICE:
         file_type = FileType::Device;
         break;
      case FILE_TYPE_STREAM:
         file_type = FileType::Stream;
         break;
      default:
         return false;
      }
      return true;
   }

   bool value_to_digest_type(std::uint16_t value, DigestType& digest_type)
   {
      switch (value)
      {
      case DIGEST_TYPE_NONE:
         digest_type = DigestType::None;
         break;
      case DIGEST_TYPE_SHA1:
         digest_type = DigestType::SHA1;
         break;
      case DIGEST_TYPE_SHA256:
         digest_type = DigestType::SHA256;
         break;
      default:
         return false;
      }
      return true;
   }
}
