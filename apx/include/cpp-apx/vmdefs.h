/*****************************************************************************
* \file      vmdefs.h
* \author    Conny Gustafsson
* \date      2020-09-02
* \brief     APX virtual machine shared definitions
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

#include "cpp-apx/types.h"
#include <vector>

namespace apx
{

   /*
   * APX PROGRAM HEADER
   * bytes 0-2: Magic numbers 'A','P','X'
   * Byte 3: VM_MAJOR_VERSION
   * Byte 4: VM_MINOR_VERSION
   * Byte 5: (high nibble): program flags
   * Byte 6: (Low nibble) program type
   * Bytes 7-10: Maximum used data size of this program (encoded as uint32_le)
   */
   constexpr std::uint32_t VM_HEADER_SIZE = 8u;
   constexpr std::uint8_t APX_VM_MAGIC_NUMBER_0 = ((uint8_t)'A');
   constexpr std::uint8_t APX_VM_MAGIC_NUMBER_1 = ((uint8_t)'P');
   constexpr std::uint8_t APX_VM_MAGIC_NUMBER_2 = ((uint8_t)'X');
   constexpr std::uint8_t VM_MAJOR_VERSION = 2u;
   constexpr std::uint8_t VM_MINOR_VERSION = 0u;
   constexpr std::uint32_t VM_HEADER_DATA_SIZE_OFFSET = 7;
   constexpr std::uint8_t VM_HEADER_PROG_TYPE_UNPACK = 0x00;
   constexpr std::uint8_t VM_HEADER_PROG_TYPE_PACK = 0x01;
   constexpr std::uint8_t VM_HEADER_FLAG_DYNAMIC_DATA = 0x10;
   constexpr std::uint32_t VM_INSTRUCTION_SIZE = sizeof(std::uint8_t);

   /* APX INSTRUCTION

   +-------------+----------------+---------------+
   | 1 FLag bit  | 4 variant bits | 3 opcode bits |
   +-------------+----------------+---------------+

   OP CODES
   0: UNPACK:   13 variants
      FLAG: is_array(true,false)
      0: U8
      1: U16
      2: U32
      3: U64
      4: S8
      5: S16
      6: S32
      7: S64
      8: ARRAY
      9: RECORD
      10: BOOL
      11: BYTES (immutable bytes object)
      12: STRING


   1: PACK  13 variants
      FLAG: is_array(true,false)
      0: U8
      1: U16
      2: U32
      3: U64
      4: S8
      5: S16
      6: S32
      7: S64
      8: ARRAY
      9: RECORD
      10: BOOL
      11: BYTES (immutable bytes object)
      12: STRING

   2: ARRAY_SIZE     : 3 variants
      FLAG: is_dynamic_array(true, false)
      0: U8
      1: U16
      2: U32
   3: DATA_CTRL  : 1 variant
      0: RECORD_SELECT
         FLAG: When 1 it means this is the last record field.
      1: LIMIT_CHECK_U8
      2: LIMIT_CHECK_U16
      3: LIMIT_CHECK_U32
      4: LIMIT_CHECK_S8
      5: LIMIT_CHECK_S16
      6: LIMIT_CHECK_S32
   4: FLOW_CTRL     : 1 variant
      1: ARRAY_NEXT
   5: UNPACK2 (reserved for 16 additional data types)
   6: PACK2 (reserved for 16 additional data types)
   7: RESERVED
   */

   constexpr std::uint8_t OPCODE_UNPACK = 0;
   constexpr std::uint8_t VARIANT_U8 = 0;
   constexpr std::uint8_t VARIANT_U16 = 1;
   constexpr std::uint8_t VARIANT_U32 = 2;
   constexpr std::uint8_t VARIANT_U64 = 3;
   constexpr std::uint8_t VARIANT_S8 = 4;
   constexpr std::uint8_t VARIANT_S16 = 5;
   constexpr std::uint8_t VARIANT_S32 = 6;
   constexpr std::uint8_t VARIANT_S64 = 7;
   constexpr std::uint8_t VARIANT_ARRAY = 8; //This obviously must have instruction flag bit set
   constexpr std::uint8_t VARIANT_RECORD = 9;


   constexpr std::uint8_t OPCODE_PACK = 1;
      //same variants as OPCODE_UNPACK
   constexpr std::uint8_t OPCODE_ARRAY_SIZE = 2;
      // uses VARIANT_U8, VARIANT_U16 and VARIANT_U32 depending on array size
      // Maximum array size is always encoded into program.
      //If flag bit is active the current array size is serialized into data buffer (as next byte(s)). This is used for dynamic arrays.
   constexpr std::uint8_t OPCODE_DATA_CTRL = 3;

   constexpr std::uint8_t VARIANT_RECORD_SELECT = 0;
   constexpr std::uint8_t VARIANT_U8_LIMIT_CHECK = 1;
   constexpr std::uint8_t VARIANT_U16_LIMIT_CHECK = 2;
   constexpr std::uint8_t VARIANT_U32_LIMIT_CHECK = 3;
   constexpr std::uint8_t VARIANT_S8_LIMIT_CHECK = 4;
   constexpr std::uint8_t VARIANT_S16_LIMIT_CHECK = 5;
   constexpr std::uint8_t VARIANT_S32_LIMIT_CHECK = 6;

   constexpr std::uint8_t OPCODE_FLOW_CTRL = 4;
   constexpr std::uint8_t VARIANT_ARRAY_NEXT = 0;

   constexpr std::uint32_t INST_SIZE = sizeof(std::uint8_t);
   constexpr std::uint8_t INST_OPCODE_MASK = 7u;
   constexpr std::uint8_t INST_VARIANT_SHIFT = 3u;
   constexpr std::uint8_t INST_VARIANT_MASK = 0xf; //applies after shift
   constexpr std::uint8_t INST_FLAG = 0x80;
   constexpr std::uint8_t ARRAY_FLAG = INST_FLAG;
   constexpr std::uint8_t DYN_ARRAY_FLAG = INST_FLAG;
   constexpr std::uint8_t LAST_FIELD_FLAG = INST_FLAG;

   using Program = std::vector<std::uint8_t>;
}


