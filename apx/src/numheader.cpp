/*****************************************************************************
* \file      numheader.h
* \author    Conny Gustafsson
* \date      2020-10-28
* \brief     C++ implementation of the numheader specification
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
#include "cpp-apx/numheader.h"
#include "cpp-apx/pack.h"

namespace numheader
{
   constexpr std::uint16_t LONG16_OFFSET = 128u;
   std::size_t encode16(std::uint8_t* begin, std::uint8_t* end, std::uint16_t value)
   {
      if ((begin != nullptr) && (end != nullptr) && (begin < end) && (value <= static_cast<std::uint16_t>(LONG16_MAX)))
      {
         std::size_t const required_size = (value <= SHORT_MAX) ? SHORT_SIZE : LONG16_SIZE;
         if (begin + required_size <= end)
         {
            if (required_size == SHORT_SIZE)
            {
               apx::packBE<std::uint8_t>(begin, static_cast<std::uint8_t>(value));
            }
            else
            {
               apx::packBE<std::uint16_t>(begin, (value-LONG16_OFFSET) | LONG16_FLAG);
            }
         }
         return required_size;
      }
      return 0u;
   }
   std::size_t decode16(std::uint8_t const* begin, std::uint8_t const* end, std::uint16_t& value)
   {

      if ((begin != nullptr) && (end != nullptr) && (begin < end))
      {
         std::uint8_t const first_byte = *begin;
         std::size_t required_size = SHORT_SIZE;
         if (first_byte & LONG8_FLAG)
         {
            required_size = LONG16_SIZE;
            if (begin + required_size <= end)
            {
               auto tmp = apx::unpackBE<std::uint16_t>(begin);
               value = (tmp & LONG16_MASK) + LONG16_OFFSET;
            }
            else
            {
               required_size = 0u; //0 indicates failure
            }
         }
         else
         {
            value = static_cast<std::uint16_t>(first_byte);
         }
         return required_size;
      }
      return 0u;
   }

   std::size_t encode32(std::uint8_t* begin, std::uint8_t* end, std::uint32_t value)
   {
      if ( (begin != nullptr) && (end != nullptr) && (begin < end) && (value <= static_cast<std::uint32_t>(LONG32_MAX)))
      {
         std::size_t const required_size = (value <= SHORT_MAX) ? SHORT_SIZE : LONG32_SIZE;
         if (begin + required_size <= end)
         {
            if (required_size == SHORT_SIZE)
            {
               apx::packBE<std::uint8_t>(begin, static_cast<std::uint8_t>(value));
            }
            else
            {
               apx::packBE<std::uint32_t>(begin, value | LONG32_FLAG);
            }
         }
         return required_size;
      }
      return 0u;
   }

   std::size_t decode32(std::uint8_t const* begin, std::uint8_t const* end, std::uint32_t& value)
   {
      if ( (begin != nullptr) && (end != nullptr) && (begin < end))
      {
         std::uint8_t const first_byte = *begin;
         std::size_t required_size = SHORT_SIZE;
         if (first_byte & LONG8_FLAG)
         {
            required_size = LONG32_SIZE;
            if (begin + required_size <= end)
            {
               auto tmp = apx::unpackBE<std::uint32_t>(begin);
               value = tmp & LONG32_MASK;
            }
            else
            {
               required_size = 0u; //0 indicates failure
            }
         }
         else
         {
            value = static_cast<std::uint32_t>(first_byte);
         }
         return required_size;
      }
      return 0u;
   }
}