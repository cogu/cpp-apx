/*****************************************************************************
* \file      vmdefs.h
* \author    Conny Gustafsson
* \date      2020-09-02
* \brief     APX virtual machine shared definitions
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

namespace apx
{

   namespace vm
   {
      /*
      * APX PROGRAM HEADER
      * bytes 0-2: Magic numbers 'A','P','X'
      * Byte 3: VM_MAJOR_VERSION
      * Byte 4: VM_MINOR_VERSION
      * Byte 5: (high nibble): program flags
      * Byte 5: (Low nibble) program type
      * Bytes 6-9: Maximum used data size required by this program (encoded as uint32_le)
      *             If no program flags are active this is the exact data size required by the program.
      */
      constexpr std::uint32_t HEADER_SIZE = 10u;
      constexpr std::uint8_t HEADER_MAGIC_NUMBER_0 = ((uint8_t)'A');
      constexpr std::uint8_t HEADER_MAGIC_NUMBER_1 = ((uint8_t)'P');
      constexpr std::uint8_t HEADER_MAGIC_NUMBER_2 = ((uint8_t)'X');
      constexpr std::uint32_t MAGIC_NUMBER_SIZE = 3u;
      constexpr std::uint8_t MAJOR_VERSION = 2u;
      constexpr std::uint8_t MINOR_VERSION = 0u;
      constexpr std::uint32_t VERSION_SIZE = 2u;
      constexpr std::uint32_t HEADER_SIZE_OFFSET = 6;
      constexpr std::uint8_t HEADER_PROG_TYPE_UNPACK = 0x01;
      constexpr std::uint8_t HEADER_PROG_TYPE_PACK = 0x02;
      constexpr std::uint8_t HEADER_PROG_TYPE_UNKNOWN = 0x0F;
      constexpr std::uint8_t HEADER_FLAG_DYNAMIC_DATA = 0x10; //This is just an indicator if any dynamic arrays are present inside the data.
      constexpr std::uint8_t HEADER_FLAG_QUEUED_DATA = 0x20; //When this is active, the very next instruction must be OPCODE_DATA_SIZE.


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

      2: DATA_SIZE     : 6 variants
         FLAG: is_dynamic_array(true, false)
         0: ARRAY_SIZE_U8
         1: ARRAY_SIZE_U16
         2: ARRAY_SIZE_U32
         3: ELEMENT_SIZE_U8
         4: ELEMENT_SIZE_U16
         5: ELEMENT_SIZE_U32

      3: DATA_CTRL  : 9 variants
         0: RECORD_SELECT
         1: LIMIT_CHECK_U8
         2: LIMIT_CHECK_U16
         3: LIMIT_CHECK_U32
         4: LIMIT_CHECK_U64
         5: LIMIT_CHECK_S8
         6: LIMIT_CHECK_S16
         7: LIMIT_CHECK_S32
         8: LIMIT_CHECK_S64
         FLAG(variant 0): When true: This is the last record field.
              When false: More record fields to follow
         FLAG (variant 1..8): When true, the limit check applies to non-scalar value (such as array of u8, u16 etc.)
      4: FLOW_CTRL     : 1 variant
         1: ARRAY_NEXT
      5: UNPACK2 (reserved for 16 additional data types)
      6: PACK2 (reserved for 16 additional data types)
      7: RESERVED
      */

      constexpr std::uint8_t OPCODE_UNPACK = 0;
      //Flag bit indicates if data element is an array
      constexpr std::uint8_t VARIANT_U8 = 0;
      constexpr std::uint8_t VARIANT_U16 = 1;
      constexpr std::uint8_t VARIANT_U32 = 2;
      constexpr std::uint8_t VARIANT_U64 = 3;
      constexpr std::uint8_t VARIANT_S8 = 4;
      constexpr std::uint8_t VARIANT_S16 = 5;
      constexpr std::uint8_t VARIANT_S32 = 6;
      constexpr std::uint8_t VARIANT_S64 = 7;
      constexpr std::uint8_t VARIANT_ARRAY = 8; //RESERVED FOR FUTURE USE (e.g. array of arrays)
      constexpr std::uint8_t VARIANT_RECORD = 9;

      constexpr std::uint8_t OPCODE_PACK = 1;
      //same variants as OPCODE_UNPACK

      constexpr std::uint8_t OPCODE_DATA_SIZE = 2;
      constexpr std::uint8_t VARIANT_ARRAY_SIZE_U8 = 0;
      constexpr std::uint8_t VARIANT_ARRAY_SIZE_U16 = 1;
      constexpr std::uint8_t VARIANT_ARRAY_SIZE_U32 = 2;
      constexpr std::uint8_t VARIANT_ELEMENT_SIZE_U8 = 4;
      constexpr std::uint8_t VARIANT_ELEMENT_SIZE_U16 = 5;
      constexpr std::uint8_t VARIANT_ELEMENT_SIZE_U32 = 6;
      // For variants 0..2: Maximum array size is always encoded into program.
      // If flag bit is set then the current array size is serialized into data buffer (as next byte(s)). This is used for dynamic arrays.

      constexpr std::uint8_t OPCODE_DATA_CTRL = 3;
      constexpr std::uint8_t VARIANT_RECORD_SELECT = 0;
      constexpr std::uint8_t VARIANT_LIMIT_CHECK_NONE = 0; //overlays with VARIANT_RECORD_SELECT
      constexpr std::uint8_t VARIANT_LIMIT_CHECK_U8  = 1;
      constexpr std::uint8_t VARIANT_LIMIT_CHECK_U16 = 2;
      constexpr std::uint8_t VARIANT_LIMIT_CHECK_U32 = 3;
      constexpr std::uint8_t VARIANT_LIMIT_CHECK_U64 = 4;
      constexpr std::uint8_t VARIANT_LIMIT_CHECK_S8  = 5;
      constexpr std::uint8_t VARIANT_LIMIT_CHECK_S16 = 6;
      constexpr std::uint8_t VARIANT_LIMIT_CHECK_S32 = 7;
      constexpr std::uint8_t VARIANT_LIMIT_CHECK_S64 = 8;

      constexpr std::uint8_t OPCODE_FLOW_CTRL = 4;
      constexpr std::uint8_t VARIANT_ARRAY_NEXT = 0;

      constexpr std::uint32_t INST_SIZE = sizeof(std::uint8_t);
      constexpr std::uint8_t INST_OPCODE_MASK = 7u;
      constexpr std::uint8_t INST_VARIANT_SHIFT = 3u;
      constexpr std::uint8_t INST_VARIANT_MASK = 0xf;
      constexpr std::uint8_t INST_FLAG = 0x80;
      constexpr std::uint8_t ARRAY_FLAG = INST_FLAG;
      constexpr std::uint8_t DYN_ARRAY_FLAG = INST_FLAG;
      constexpr std::uint8_t LAST_FIELD_FLAG = INST_FLAG;

      constexpr std::uint32_t UINT8_SIZE = sizeof(std::uint8_t);
      constexpr std::uint32_t UINT16_SIZE = sizeof(std::uint16_t);
      constexpr std::uint32_t UINT32_SIZE = sizeof(std::uint32_t);
      constexpr std::uint32_t INT8_SIZE = sizeof(std::int8_t);
      constexpr std::uint32_t INT16_SIZE = sizeof(std::int16_t);
      constexpr std::uint32_t INT32_SIZE = sizeof(std::int32_t);

   }

}


