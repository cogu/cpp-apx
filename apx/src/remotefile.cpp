/*****************************************************************************
* \file      remotefile.cpp
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
#include <cassert>
#include "cpp-apx/remotefile.h"
#include "cpp-apx/pack.h"

namespace rmf
{
   std::size_t needed_encoding_size(std::uint32_t address)
   {
      return (address < HIGH_ADDR_MIN) ? sizeof(std::uint16_t) : sizeof(std::uint32_t);
   }

   std::size_t address_encode(std::uint8_t* buf, std::size_t buf_size, std::uint32_t address, bool more_bit)
   {
      if ((buf == nullptr) || (buf_size == 0) || (address > HIGH_ADDR_MAX))
      {
         return 0; //Invalid argument
      }
      std::size_t encoding_size = needed_encoding_size(address);
      if (encoding_size <= buf_size)
      {
         if (encoding_size == sizeof(std::uint16_t))
         {
            std::uint16_t value = more_bit ? MORE_BIT_LOW_ADDR : 0u;
            value |= static_cast<std::uint16_t>(address);
            apx::packBE<std::uint16_t>(buf, value);
         }
         else
         {
            assert(encoding_size == sizeof(std::uint32_t));
            std::uint32_t value = more_bit ? (HIGH_ADDR_BIT | MORE_BIT_HIGH_ADDR) : HIGH_ADDR_BIT;
            value |= address;
            apx::packBE<std::uint32_t>(buf, value);
         }
      }
      else
      {
         return 0u; //Not enough bytes in buffer
      }
      return encoding_size;
   }

   std::size_t address_decode(std::uint8_t const* begin, std::uint8_t const* end, std::uint32_t& address, bool& more_bit)
   {
      std::size_t retval{ 0 };
      if ((begin == nullptr) || (end == nullptr) || (begin >= end) )
      {
         return 0u; //Invalid argument
      }
      std::uint8_t const first_byte = *begin;
      more_bit = (first_byte & U8_MORE_BIT) ? true : false;
      if (first_byte & U8_HIGH_ADDR_BIT)
      {
         if (begin + sizeof(std::uint32_t) <= end)
         {
            std::uint32_t value = apx::unpackBE<std::uint32_t>(begin);
            address = value & HIGH_ADDR_MASK;
            retval = sizeof(value);
         }
      }
      else
      {
         if (begin + sizeof(std::uint16_t) <= end)
         {
            std::uint16_t value = apx::unpackBE<std::uint16_t>(begin);
            address = static_cast<std::uint32_t>(value & LOW_ADDR_MASK);
            retval = sizeof(value);
         }
      }
      return retval;
   }

   std::size_t encode_open_file_cmd(std::uint8_t* buf, std::size_t buf_size, std::uint32_t address)
   {
      std::size_t const required_size = rmf::CMD_TYPE_SIZE + rmf::FILE_OPEN_CMD_SIZE;
      if ( (address > HIGH_ADDR_MAX) || (required_size > buf_size) )
      {
         return 0;
      }
      std::uint8_t* p{ buf };
      apx::packLE<std::uint32_t>(p, rmf::CMD_OPEN_FILE_MSG); p += sizeof(std::uint32_t);
      apx::packLE<std::uint32_t>(p, address); p += sizeof(std::uint32_t);
      return required_size;
   }

   std::size_t encode_acknowledge_cmd(std::uint8_t* buf, std::size_t buf_size)
   {
      std::size_t const required_size = CMD_TYPE_SIZE;
      if (required_size > buf_size)
      {
         return 0u;
      }
      apx::packLE<std::uint32_t>(buf, rmf::CMD_ACK_MSG);
      return required_size;
   }

   std::size_t decode_cmd_type(std::uint8_t const* begin, std::uint8_t const* end, std::uint32_t& cmd_type)
   {
      if (begin + CMD_TYPE_SIZE <= end)
      {
         cmd_type = apx::unpackLE<std::uint32_t>(begin);
         return CMD_TYPE_SIZE;
      }
      return 0u;
   }
}