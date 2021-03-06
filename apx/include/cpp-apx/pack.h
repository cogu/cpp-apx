/*****************************************************************************
* \file      pack.h
* \author    Conny Gustafsson
* \date      2020-09-02
* \brief     C++ integer serialization/deserialization routines
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
#include <cstring>
#include <version>
#include <limits>
#include <iostream>
#if __has_include(<bit>) && __cpp_lib_endian
#include <bit>
constexpr bool _is_big_endian = std::endian::native == std::endian::big;
constexpr bool _is_little_endian = std::endian::native == std::endian::little;
#else
#ifdef BYTE_ORDER
constexpr bool _is_big_endian = BYTE_ORDER == 1u;
constexpr bool _is_little_endian = BYTE_ORDER == 0u;
#else
constexpr bool _is_big_endian = false;
constexpr bool _is_little_endian = false;
#endif
#endif


namespace apx
{
   template<typename T> void packBE(std::uint8_t* p, T value)
   {
      static_assert(std::is_integral<T>::value, "Value type must be integer");
      if constexpr(_is_big_endian)
      {
         constexpr std::size_t size = sizeof(T);
         std::memcpy(p, &value, size);
      }
      else
      {
         std::size_t size = sizeof(T);
         p += (size - 1);
         while (size > 0)
         {
            *(p--) = static_cast<std::uint8_t>(value);
            value >>= 8;
            size--;
         }
      }
   }

   template<> inline void packBE(std::uint8_t* p, std::uint8_t value)
   {
      *p = value;
   }

   template<typename T> T unpackBE(const std::uint8_t* p)
   {
      static_assert(std::is_integral<T>::value, "Value type must be integer");
      T value = 0u;
      if constexpr (_is_big_endian)
      {
         constexpr std::size_t size = sizeof(T);
         std::memcpy(&value, p, size);
      }
      else
      {
         std::size_t size = sizeof(T);
         while (size > 0)
         {
            value = (value << 8) | static_cast<T>(*p++);
            size--;
         }
      }
      return value;
   }

   template<> inline std::uint8_t unpackBE(const std::uint8_t* p)
   {
      return *p;
   }

   template<typename T> void packLE(std::uint8_t* p, T value)
   {
      static_assert(std::is_integral<T>::value, "Value type must be integer");
      if constexpr (_is_little_endian)
      {
         constexpr std::size_t size = sizeof(T);
         std::memcpy(p, &value, size);
      }
      else
      {
         std::size_t size = sizeof(T);
         while (size > 0)
         {
            *(p++) = static_cast<std::uint8_t>(value);
            value >>= 8;
            size--;
         }
      }
   }

   template<> inline void packLE(std::uint8_t* p, std::uint8_t value)
   {
      *p = value;
   }

   template<typename T> T unpackLE(const std::uint8_t* p)
   {
      static_assert(std::is_integral<T>::value, "Value type must be integer");
      T value = 0u;
      if constexpr (_is_little_endian)
      {
         constexpr std::size_t size = sizeof(T);
         std::memcpy(&value, p, size);
      }
      else
      {
         std::size_t size = sizeof(T);
         p += (size-1);
         while (size > 0)
         {
            value = (value << 8) | static_cast<T>(*p--);
            size--;
         }
      }
      return value;
   }

   template<> inline std::uint8_t unpackLE(const std::uint8_t* p)
   {
      return *p;
   }
}