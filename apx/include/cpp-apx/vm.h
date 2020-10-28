/*****************************************************************************
* \file      vm.h
* \author    Conny Gustafsson
* \date      2020-09-27
* \brief     APX Virtual Machine
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

#include <vector>
#include "cpp-apx/vmdefs.h"
#include "cpp-apx/error.h"
#include "cpp-apx/program.h"
#include "cpp-apx/types.h"
#include "cpp-apx/decoder.h"
#ifdef QT_API
#include "cpp-apx/qt_serializer.h"
#else
#include "cpp-apx/serializer.h"
#include "cpp-apx/deserializer.h"
#endif

namespace apx
{
   class VirtualMachine
   {
   public:
      apx::error_t select_program(apx::vm::Program const& program);
      apx::error_t set_write_buffer(std::uint8_t* data, std::size_t size);
      apx::error_t set_read_buffer(std::uint8_t const* data, std::size_t size);
#ifdef QT_API
      //TODO: ADD QT API
#else
      apx::error_t pack_value(dtl::Value const* value);
      apx::error_t pack_value(dtl::ScalarValue const& value);
      apx::error_t unpack_value(dtl::ScalarValue& value);
#endif
   protected:

      vm::ProgramHeader m_program_header;
      vm::Decoder m_decoder;
#ifdef QT_API
      vm::QSerializer m_serializer;
#else
      vm::Serializer m_serializer;
      vm::Deserializer m_deserializer;
#endif

      apx::error_t run_pack_program();
      apx::error_t run_unpack_program();
      apx::error_t run_pack_instruction();
      apx::error_t run_unpack_instruction();
      apx::error_t run_range_check_pack_int32();
      apx::error_t run_range_check_pack_uint32();
      apx::error_t run_range_check_pack_int64();
      apx::error_t run_range_check_pack_uint64();
      apx::error_t run_range_check_unpack_int32();
      apx::error_t run_range_check_unpack_uint32();
      apx::error_t run_range_check_unpack_int64();
      apx::error_t run_range_check_unpack_uint64();
      apx::error_t run_pack_record_select();
      apx::error_t run_unpack_record_select();
      apx::error_t run_array_next();
      bool is_pack_prog() { return m_program_header.prog_type == ProgramType::Pack; }

   };
}