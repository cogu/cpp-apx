/*****************************************************************************
* \file      remotefile.h
* \author    Conny Gustafsson
* \date      2020-11-11
* \brief     Remotefile protocol and definitions
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

#include <cstdint>
#include <cstddef>
#include "cpp-apx/error.h"

namespace rmf
{
   enum class FileType
   {
      Fixed,
      Dynamic8,
      Dynamic16,
      Dynamic32,
      Device,
      Stream,
      CompressedFixed
   };

   enum class DigestType { None, SHA1, SHA256 };

   constexpr std::uint32_t REMOTE_ADDRESS_BIT = 0x80000000ul; //This is overlayed with HIGH_ADDR_BIT
   constexpr std::uint32_t INVALID_ADDRESS = 0x7FFFFFFFul;
   constexpr std::uint32_t ADDRESS_MASK_INTERNAL = 0x7FFFFFFFul;
   constexpr std::size_t SHA1_SIZE = 20u;
   constexpr std::size_t SHA256_SIZE = 32u;
   constexpr std::uint32_t CMD_AREA_START_ADDRESS = 0x3FFFFC00;
   constexpr std::size_t CMD_AREA_SIZE = 1024; //1KB
   constexpr std::uint32_t CMD_AREA_END_ADDRESS = CMD_AREA_START_ADDRESS + CMD_AREA_SIZE; //First byte after last valid address
   constexpr std::uint32_t LOW_ADDR_MAX = 16383;
   constexpr std::uint32_t HIGH_ADDR_MIN = 16384;
   constexpr std::uint32_t HIGH_ADDR_MAX = 0x3FFFFFFFu;
   constexpr std::uint32_t HIGH_ADDR_BIT = 0x80000000ul; //This is overlayed with REMOTE_ADDRESS_BIT
   constexpr std::uint16_t MORE_BIT_LOW_ADDR = 0x4000u;
   constexpr std::uint32_t MORE_BIT_HIGH_ADDR = 0x40000000u;
   constexpr std::uint16_t LOW_ADDR_MASK = 0x3FFFu;
   constexpr std::uint32_t HIGH_ADDR_MASK = HIGH_ADDR_MAX;
   constexpr std::uint8_t U8_MORE_BIT = 0x40;
   constexpr std::uint8_t U8_HIGH_ADDR_BIT = 0x80u;
   constexpr std::size_t LOW_ADDR_SIZE = sizeof(std::uint16_t);
   constexpr std::size_t HIGH_ADDR_SIZE = sizeof(std::uint32_t);
   constexpr std::size_t FILE_NAME_MAX_SIZE = 255;
   constexpr std::size_t FILE_INFO_HEADER_SIZE = 48u;

   constexpr std::uint32_t CMD_FILE_ACK_MSG = 0u;
   constexpr std::uint32_t CMD_FILE_NACK_MSG = 1u;
   constexpr std::uint32_t CMD_PUBLISH_FILE_MSG = 3u;
   constexpr std::uint32_t CMD_FILE_INFO_MSG = 3u;
   constexpr std::uint32_t CMD_REVOKE_FILE_MSG = 4u;
   constexpr std::uint32_t CMD_OPEN_FILE_MSG = 10u;
   constexpr std::uint32_t CMD_CLOSE_FILE_MSG = 11u;
   constexpr std::uint32_t FILE_OPEN_CMD_SIZE = 8u;
   constexpr std::uint32_t FILE_CLOSE_CMD_SIZE = 8u;

   std::size_t needed_encoding_size(std::uint32_t address);
   std::size_t address_encode(std::uint8_t* begin, std::uint8_t* end, std::uint32_t address, bool more_bit);
   std::size_t address_decode(std::uint8_t const* begin, std::uint8_t const* end, std::uint32_t& address, bool& more_bit);
   apx::error_t encode_open_file_cmd(std::uint8_t* buf, std::size_t buf_size, std::uint32_t address);

}