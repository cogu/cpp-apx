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
      * APX PROGRAM HEADER (Varies between 7 and 14 bytes)
      * bytes 0-2: Magic numbers 'A','P','X'
      * Byte 3: VM_MAJOR_VERSION
      * Byte 4: VM_MINOR_VERSION
      * Byte 5 (bits 4-7): program flags
      * Byte 5 (bit 3): pack or unpack program (0-1)
      * Byte 5 (bits 0-2): data size variant (VARIANT_U8, VARIANT_U16, VARIANT_U32)
      * Bytes 6-9: DataSize (Maximum data size required by this program) (variable-size encoded integer)
      *            - If Byte 5 (bits 0-2) has value VARIANT_U8 this is encoded as uint8.
      *            - If Byte 5 (bits 0-2) has value VARIANT_U16 this is encoded as uint16le (little endian).
      *            - If Byte 5 (bits 0-2) has value VARIANT_U32 this is encoded as uint32le.
      * After the the previous integer is encoded the header usually ends. However, if HEADER_FLAG_QUEUED_DATA was set among program flags (Byte 5)
      * The header continues with an encoded DATA_SIZE instruction where variant must be value 2 or above.
      * Since length of previous header field varies we call the first byte after the encoded integer "Byte N".
      * Byte N: DATA_SIZE instruction header
      * Bytes (N+1)-(N+4): ElementSize (variable-size encoded integer)
      *
      * When HEADER_FLAG_QUEUED_DATA is set the queue size can be calculated from using the DataSize and ElementSize numbers.
      * The DataSize value has been previously incremented by the value 1, 2 or 4. Which of these it is can be determined from variant in the DATA_SIZE instruction.
      * The queue length can be calculated by using this formula:
      *
      * Number of queued elements = (DataSize-QueueStorageSize)/ElementSize
      *
      * where QueueStorageSize is either 1, 2, or 4 (which can be determined from the variant on the DATA_SIZE instruction).
      */

      constexpr std::uint8_t HEADER_MAGIC_NUMBER_0 = ((uint8_t)'A');
      constexpr std::uint8_t HEADER_MAGIC_NUMBER_1 = ((uint8_t)'P');
      constexpr std::uint8_t HEADER_MAGIC_NUMBER_2 = ((uint8_t)'X');
      constexpr std::uint32_t MAGIC_NUMBER_SIZE = 3u;
      constexpr std::uint8_t MAJOR_VERSION = 2u;
      constexpr std::uint8_t MINOR_VERSION = 0u;
      constexpr std::uint32_t VERSION_SIZE = 2u;
      constexpr std::uint32_t FIXED_HEADER_SIZE = 6;
      constexpr std::uint8_t HEADER_PROG_TYPE_UNPACK = 0x00;
      constexpr std::uint8_t HEADER_PROG_TYPE_PACK = 0x08;
      constexpr std::uint8_t HEADER_DATA_VARIANT_MASK = 0x07;

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
         11: BYTE (immutable bytes object)
         12: CHAR


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
         11: BYTE (immutable bytes object)
         12: CHAR

      2: DATA_SIZE     : 6 variants
         FLAG: is_dynamic_array(true, false)
         0: ARRAY_SIZE_U8
         1: ARRAY_SIZE_U16
         2: ARRAY_SIZE_U32
         4: ELEMENT_SIZE_U8_QUEUE_SIZE_U8
         5: ELEMENT_SIZE_U8_QUEUE_SIZE_U16
         6: ELEMENT_SIZE_U8_QUEUE_SIZE_U32
         7: ELEMENT_SIZE_U16_QUEUE_SIZE_U8
         8: ELEMENT_SIZE_U16_QUEUE_SIZE_U16
         9: ELEMENT_SIZE_U16_QUEUE_SIZE_U32
         10: ELEMENT_SIZE_U32_QUEUE_SIZE_U8
         11: ELEMENT_SIZE_U32_QUEUE_SIZE_U16
         12: ELEMENT_SIZE_U32_QUEUE_SIZE_U32

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
      constexpr std::uint8_t VARIANT_BOOL = 10;
      constexpr std::uint8_t VARIANT_BYTE = 11;
      constexpr std::uint8_t VARIANT_CHAR = 12;

      constexpr std::uint8_t OPCODE_PACK = 1;
      //same variants as OPCODE_UNPACK

      constexpr std::uint8_t OPCODE_DATA_SIZE = 2;
      constexpr std::uint8_t VARIANT_ARRAY_SIZE_U8 = 0;
      constexpr std::uint8_t VARIANT_ARRAY_SIZE_U16 = 1;
      constexpr std::uint8_t VARIANT_ARRAY_SIZE_U32 = 2;
      constexpr std::uint8_t VARIANT_ELEMENT_SIZE_U8_BASE = 3;
      constexpr std::uint8_t VARIANT_ELEMENT_SIZE_U8_QUEUE_SIZE_U8 = VARIANT_ELEMENT_SIZE_U8_BASE + VARIANT_U8;    // 3
      constexpr std::uint8_t VARIANT_ELEMENT_SIZE_U8_QUEUE_SIZE_U16 = VARIANT_ELEMENT_SIZE_U8_BASE + VARIANT_U16;  // 4
      constexpr std::uint8_t VARIANT_ELEMENT_SIZE_U8_QUEUE_SIZE_U32 = VARIANT_ELEMENT_SIZE_U8_BASE + VARIANT_U32;  // 5
      constexpr std::uint8_t VARIANT_ELEMENT_SIZE_U16_BASE = 6;
      constexpr std::uint8_t VARIANT_ELEMENT_SIZE_U16_QUEUE_SIZE_U8 = VARIANT_ELEMENT_SIZE_U16_BASE + VARIANT_U8;    // 6
      constexpr std::uint8_t VARIANT_ELEMENT_SIZE_U16_QUEUE_SIZE_U16 = VARIANT_ELEMENT_SIZE_U16_BASE + VARIANT_U16;  // 7
      constexpr std::uint8_t VARIANT_ELEMENT_SIZE_U16_QUEUE_SIZE_U32 = VARIANT_ELEMENT_SIZE_U16_BASE + VARIANT_U32;  // 8
      constexpr std::uint8_t VARIANT_ELEMENT_SIZE_U32_BASE = 9;
      constexpr std::uint8_t VARIANT_ELEMENT_SIZE_U32_QUEUE_SIZE_U8 = VARIANT_ELEMENT_SIZE_U32_BASE + VARIANT_U8;    // 9
      constexpr std::uint8_t VARIANT_ELEMENT_SIZE_U32_QUEUE_SIZE_U16 = VARIANT_ELEMENT_SIZE_U32_BASE + VARIANT_U16;  // 10
      constexpr std::uint8_t VARIANT_ELEMENT_SIZE_U32_QUEUE_SIZE_U32 = VARIANT_ELEMENT_SIZE_U32_BASE + VARIANT_U32;  // 11
      constexpr std::uint8_t VARIANT_ELEMENT_SIZE_LAST = VARIANT_ELEMENT_SIZE_U32_QUEUE_SIZE_U32;
      // For variants 0..2: Maximum array size is always encoded into program.
      // If flag bit is set then the current array size is serialized into data buffer (as next byte(s)). This is used for dynamic arrays.
      // Flag bit is not used for variants 3..11

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
      constexpr std::uint32_t BYTE_SIZE = sizeof(std::uint8_t); //TODO: Change to std::byte when C++20 has better support?
      constexpr std::uint32_t UINT16_SIZE = sizeof(std::uint16_t);
      constexpr std::uint32_t UINT32_SIZE = sizeof(std::uint32_t);
      constexpr std::uint32_t UINT64_SIZE = sizeof(std::uint64_t);
      constexpr std::uint32_t INT8_SIZE = sizeof(std::int8_t);
      constexpr std::uint32_t INT16_SIZE = sizeof(std::int16_t);
      constexpr std::uint32_t INT32_SIZE = sizeof(std::int32_t);
      constexpr std::uint32_t INT64_SIZE = sizeof(std::int64_t);
   }

}


