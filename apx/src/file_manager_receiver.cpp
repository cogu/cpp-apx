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
#include <cstring>
#include <cassert>
#include "cpp-apx/file_manager_receiver.h"

namespace apx
{
   FileManagerReceiver::FileManagerReceiver()
   {
      reserve(rmf::CMD_AREA_SIZE);
   }

   void FileManagerReceiver::reset()
   {
      m_buf_pos = 0u;
      m_start_address = rmf::INVALID_ADDRESS;
      m_is_complete = false;
   }

   void FileManagerReceiver::reserve(std::size_t size)
   {
      if (m_receive_buffer.size() < size)
      {
         m_receive_buffer.resize(size);
      }
   }

   FileManagerReceptionResult FileManagerReceiver::write(std::uint32_t address, std::uint8_t const* data, std::size_t size, bool more_bit)
   {
      FileManagerReceptionResult result;
      if ((data != 0) && (address < rmf::INVALID_ADDRESS))
      {
         if (size > MAX_FILE_SIZE)
         {
            result.error = APX_FILE_TOO_LARGE_ERROR;
         }
         else
         {
            if (m_start_address == rmf::INVALID_ADDRESS)
            {
               start_new_reception(result, address, data, size, more_bit);
            }
            else
            {
               continue_reception(result, address, data, size, more_bit);
            }
         }
      }
      else
      {
         result.error = APX_INVALID_ARGUMENT_ERROR;
      }
      return result;
   }

   void FileManagerReceiver::start_new_reception(FileManagerReceptionResult& result, std::uint32_t address, std::uint8_t const* data, std::size_t size, bool more_bit)
   {
      if (m_receive_buffer.size() == 0)
      {
         result.error = APX_MISSING_BUFFER_ERROR;
      }
      else if (size > m_receive_buffer.size())
      {
         result.error = APX_BUFFER_FULL_ERROR;
      }
      else
      {
         if (size > 0)
         {
            std::memcpy(m_receive_buffer.data(), data, size);
            m_buf_pos = size;
         }
         m_start_address = address;
         process_more_bit(result, more_bit);
      }
   }

   void FileManagerReceiver::continue_reception(FileManagerReceptionResult& result, std::uint32_t address, std::uint8_t const* data, std::size_t size, bool more_bit)
   {
      assert(m_start_address != rmf::INVALID_ADDRESS);
      std::uint32_t expected_address = (m_start_address + static_cast<std::uint32_t>(m_buf_pos));
      if (expected_address != address)
      {
         result.error = APX_INVALID_ADDRESS_ERROR;
      }
      if (m_receive_buffer.size() == 0)
      {
         result.error = APX_MISSING_BUFFER_ERROR;
      }
      else if ((m_buf_pos + size) > m_receive_buffer.size())
      {
         result.error = APX_BUFFER_FULL_ERROR;
      }
      else
      {
         if (size > 0)
         {
            std::memcpy(m_receive_buffer.data() + m_buf_pos, data, size);
            m_buf_pos += size;
         }
         process_more_bit(result, more_bit);
      }
   }
   void FileManagerReceiver::process_more_bit(FileManagerReceptionResult& result, bool more_bit)
   {
      if (more_bit)
      {
         m_is_complete = false;
      }
      else
      {
         result.is_complete = m_is_complete = true;
         result.address = m_start_address;
         result.data = m_receive_buffer.data();
         result.size = m_buf_pos;
         reset();
      }
   }
}