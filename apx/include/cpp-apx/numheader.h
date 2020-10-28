/*****************************************************************************
* \file      numheader.h
* \author    Conny Gustafsson
* \date      2020-10-28
* \brief     C++ implementation of the numheader protocol
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

namespace numheader
{
   constexpr std::uint16_t SHORT_MAX = 127u;
   constexpr std::uint16_t LONG16_MAX = INT16_MAX+INT8_MAX + 1u;
   constexpr std::uint32_t LONG32_MAX = INT32_MAX;
   constexpr std::size_t SHORT_SIZE = sizeof(std::uint8_t);
   constexpr std::size_t LONG16_SIZE = sizeof(std::uint16_t);
   constexpr std::size_t LONG32_SIZE = sizeof(std::uint32_t);
   constexpr std::uint16_t LONG16_FLAG = 0x8000ul;
   constexpr std::uint16_t LONG16_MASK = 0x7ffful;
   constexpr std::uint32_t LONG32_FLAG = 0x80000000ul;
   constexpr std::uint32_t LONG32_MASK = 0x7ffffffful;
   constexpr std::uint8_t  LONG8_FLAG = 0x80u;

   std::size_t encode16(std::uint8_t* begin, std::uint8_t* end, std::uint16_t value);
   std::size_t decode16(std::uint8_t const* begin, std::uint8_t const* end, std::uint16_t& value);
   std::size_t encode32(std::uint8_t* begin, std::uint8_t* end, std::uint32_t value);
   std::size_t decode32(std::uint8_t const* begin, std::uint8_t const* end, std::uint32_t& value);
}
