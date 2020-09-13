/*****************************************************************************
* \file      program.h
* \author    Conny Gustafsson
* \date      2020-09-02
* \brief     Common definitions and utilities for APX programs
*
* Copyright (c) 2019 Conny Gustafsson
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

namespace apx
{
   namespace vm
   {
      using Program = std::vector<std::uint8_t>;

      struct ProgramHeader
      {
         std::uint8_t major_version{ apx::vm::MAJOR_VERSION };
         std::uint8_t minor_version{ apx::vm::MINOR_VERSION };
         std::uint8_t prog_type{ HEADER_PROG_TYPE_UNKNOWN };
         std::uint8_t prog_flags{ 0u };
         std::uint32_t data_size{ 0u };
      };

      apx::error_t init_program_header(apx::vm::Program& program, std::uint8_t prog_type);
      apx::error_t decode_program_header(std::uint8_t const* program, ProgramHeader& header);
      void reserve_elem_size_instruction(apx::vm::Program& program, apx::SizeType elem_size);
      std::uint8_t encode_instruction(std::uint8_t opcode, std::uint8_t variant, bool flag);
      apx::error_t decode_instruction(std::uint8_t instruction, std::uint8_t &opcode, std::uint8_t &variant, bool &flag);

   }
}



