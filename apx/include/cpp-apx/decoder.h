/*****************************************************************************
* \file      decoder.h
* \author    Conny Gustafsson
* \date      2020-09-27
* \brief     APX VM instruction decoder
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

#include "cpp-apx/vmdefs.h"
#include "cpp-apx/error.h"
#include "cpp-apx/program.h"

namespace apx
{
   namespace vm
   {
      class Decoder
      {
      public:
         apx::error_t select_program(std::uint8_t const* begin, std::uint8_t const* end);
         apx::error_t parse_next_operation(OperationType& operation_type);
         PackUnpackOperationInfo const& get_pack_unpack_info() { return m_pack_unpack_info; }
         LimitCheckUInt32OperationInfo const& get_limit_check_uint32() { return m_limit_check_uint32_info; }
         LimitCheckInt32OperationInfo const& get_limit_check_int32() { return m_limit_check_int32_info; }
         LimitCheckUInt64OperationInfo const& get_limit_check_uint64() { return m_limit_check_uint64_info; }
         LimitCheckInt64OperationInfo const& get_limit_check_int64() { return m_limit_check_int64_info; }

      protected:
         std::uint8_t const* m_program_next{ nullptr };
         std::uint8_t const* m_program_end{ nullptr };
         OperationType m_operation_type{ OperationType::ProgramEnd };
         PackUnpackOperationInfo m_pack_unpack_info;
         LimitCheckUInt32OperationInfo m_limit_check_uint32_info{ 0u, 0u };
         LimitCheckInt32OperationInfo m_limit_check_int32_info{ 0, 0 };
         LimitCheckUInt64OperationInfo m_limit_check_uint64_info{ 0u, 0u };
         LimitCheckInt64OperationInfo m_limit_check_int64_info{ 0, 0 };


         apx::error_t decode_next_instruction_internal();
         void Decoder::reset_pack_unpack_info(apx::TypeCode type_code);
         apx::error_t decode_array_size();
         apx::error_t decode_limit_check_uint32(std::uint8_t variant);
         apx::error_t decode_limit_check_uint64(std::uint8_t variant);
         apx::error_t decode_limit_check_int32(std::uint8_t variant);
         apx::error_t decode_limit_check_int64(std::uint8_t variant);

      };
   }
}