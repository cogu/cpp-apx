/*****************************************************************************
* \file      file_info.h
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
#pragma once

#include <string>
#include <array>
#include "cpp-apx/remotefile.h"

namespace rmf
{


   struct FileInfo
   {
      FileInfo() = default;
      FileInfo(FileInfo const& other) = default;
      FileInfo(char const* _name, std::uint32_t _size, std::uint32_t _address = INVALID_ADDRESS);
      FileInfo(char const* _name, std::uint32_t _size, DigestType _digest_type, std::uint8_t const* _digest_data);
      FileInfo(char const* _name, std::uint32_t _size, std::uint32_t _address, DigestType _digest_type, std::uint8_t const* _digest_data);
      FileInfo(std::string const& _name, std::uint32_t _size, std::uint32_t _address = INVALID_ADDRESS);
      FileInfo(std::string const& _name, std::uint32_t _size, std::uint32_t _address,
         FileType file_type, DigestType _digest_type, std::uint8_t const* _digest_data);
      std::uint32_t address_without_flags() const { return address & ADDRESS_MASK_INTERNAL; }
      bool is_remote_address() const { return (address & REMOTE_ADDRESS_BIT) != 0ul; }
      bool name_ends_with(char const* suffix) const;
      bool name_ends_with(std::string const& suffix) const;
      std::string base_name() const;
      bool address_in_range(std::uint32_t addr) const { return ( (address_without_flags() <= addr) && (addr < (address_without_flags() + size)) ); }

      std::uint32_t address{ INVALID_ADDRESS };
      std::uint32_t size{ 0u };
      FileType rmf_file_type { FileType::Fixed };
      DigestType digest_type { DigestType::None };
      std::array <std::uint8_t, SHA256_SIZE> digest_data;
      std::string name;
   };

   std::size_t encode_publish_file_cmd(std::uint8_t* buf, std::size_t buf_size, FileInfo const* file);
   std::size_t decode_publish_file_cmd(std::uint8_t const* buf, std::size_t buf_size, FileInfo &file_info);
   bool value_to_file_type(std::uint16_t value, FileType& file_type);
   bool value_to_digest_type(std::uint16_t value, DigestType& digest_type);
}
