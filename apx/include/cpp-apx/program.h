/*****************************************************************************
* \file      program.h
* \author    Conny Gustafsson
* \date      2020-09-02
* \brief     Common definitions and utilities for APX programs
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

namespace apx
{
   namespace vm
   {
      using Program = std::vector<std::uint8_t>;

      struct ProgramHeader
      {
         std::uint8_t major_version{ apx::vm::MAJOR_VERSION };
         std::uint8_t minor_version{ apx::vm::MINOR_VERSION };
         apx::ProgramType prog_type{ apx::ProgramType::Unpack };
         std::uint32_t data_size{ 0u };
         std::uint32_t element_size{ 0u };
         std::uint32_t queue_length{ 0u };
         bool is_dynamic_data{ false };
      };

      std::uint8_t const* parse_uint32_by_variant(std::uint8_t const* begin, std::uint8_t const* end, std::uint8_t variant, std::uint32_t& number);
      std::uint8_t const* parse_uint64_by_variant(std::uint8_t const* begin, std::uint8_t const* end, std::uint8_t variant, std::uint64_t& number);
      std::uint8_t const* parse_int32_by_variant(std::uint8_t const* begin, std::uint8_t const* end, std::uint8_t variant, std::int32_t& number);
      std::uint8_t const* parse_int64_by_variant(std::uint8_t const* begin, std::uint8_t const* end, std::uint8_t variant, std::int64_t& number);
      std::uint8_t const* parse_uint32_by_size_type(std::uint8_t const* begin, std::uint8_t const* end, apx::SizeType size_type, std::uint32_t& number);
      apx::error_t create_program_header(apx::vm::Program& header, apx::ProgramType program_type, std::uint32_t element_size, std::uint32_t queue_size, bool is_dynamic);
      apx::error_t decode_program_header(std::uint8_t const* begin, std::uint8_t const* end, std::uint8_t const*& next, ProgramHeader &header);
      apx::error_t decode_program_header(apx::vm::Program const& program, ProgramHeader& header);
      std::uint8_t encode_instruction(std::uint8_t opcode, std::uint8_t variant, bool flag);
      void decode_instruction(std::uint8_t instruction, std::uint8_t &opcode, std::uint8_t &variant, bool &flag);
      std::size_t variant_to_size_full(std::uint8_t variant);
      std::size_t variant_to_size_integer(std::uint8_t variant);
      TypeCode variant_to_type_code(std::uint8_t variant);
      std::size_t size_type_to_size(apx::SizeType size_type);
      apx::SizeType size_to_size_type(std::size_t size);
      std::uint8_t size_type_to_variant(apx::SizeType size_type);
   }
}



