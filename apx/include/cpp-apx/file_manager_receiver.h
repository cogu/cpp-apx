/*****************************************************************************
* \file      file_manager_receiver.cpp
* \author    Conny Gustafsson
* \date      2020-11-05
* \brief     Message receiver component of file manager
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
#include "cpp-apx/remotefile.h"

namespace apx
{
   struct FileManagerReceptionResult
   {
      error_t error{ APX_NO_ERROR };
      bool is_complete{ false };
      std::uint32_t address{ rmf::INVALID_ADDRESS };
      std::uint8_t const* data{ nullptr };
      std::size_t size{ 0u };
   };

   class FileManagerReceiver
   {
   public:
      FileManagerReceiver();
      void reset();
      void reserve(std::size_t size);
      FileManagerReceptionResult write(std::uint32_t address, std::uint8_t const* data, std::size_t size, bool more_bit);
      std::size_t buffer_size() const { return m_receive_buffer.size(); }
   protected:

      void start_new_reception(FileManagerReceptionResult& result, std::uint32_t address, std::uint8_t const* data, std::size_t size, bool more_bit);
      void continue_reception(FileManagerReceptionResult& result, std::uint32_t address, std::uint8_t const* data, std::size_t size, bool more_bit);
      void process_more_bit(FileManagerReceptionResult& result, bool more_bit);
      apx::ByteArray m_receive_buffer;
      std::size_t m_buf_pos{ 0u };
      std::uint32_t m_start_address{ rmf::INVALID_ADDRESS };
      bool m_is_complete{ false };
   };
}