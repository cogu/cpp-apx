/*****************************************************************************
* \file      transmit_handler.h
* \author    Conny Gustafsson
* \date      2020-11-11
* \brief     Connection interface class
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

#include "cpp-apx/types.h"
#include "cpp-apx/error.h"
#include "cpp-apx/file.h"

namespace apx
{
   class ConnectionInterface
   {
   public:
      //Transmit methods
      virtual std::int32_t transmit_max_bytes_avaiable() const = 0;
      virtual std::int32_t transmit_current_bytes_avaiable() const = 0;
      virtual void transmit_begin() = 0;
      virtual void transmit_end() = 0;
      virtual error_t transmit_data_message(std::uint32_t write_address, bool more_bit, std::uint8_t const* data, std::int32_t size, std::int32_t& bytes_available) = 0;
      virtual error_t transmit_direct_message(std::uint8_t const* data, std::int32_t size, std::int32_t& bytes_available) = 0;

      // Notification callbacks
      virtual error_t remote_file_published_notification(File* file) = 0;
      virtual error_t remote_file_write_notification(File* file, std::uint32_t offset, std::uint8_t const* data, std::size_t size) = 0;
   };
}