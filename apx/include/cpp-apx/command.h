/*****************************************************************************
* \file      command.h
* \author    Conny Gustafsson
* \date      2020-11-11
* \brief     Command definitions
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
   enum class CmdType : std::uint32_t {
      Exit,
      SendAcknowledge,
      SendErrorCode,
      PublishLocalFile,
      RevokeLocalFile,
      OpenRemoteFile,
      CloseRemoteFile,
      SendLocalConstData,
      SendLocalData,
   };

   struct Command
   {
      Command(CmdType ct, std::uint32_t d1, std::uint32_t d2, void* d3, void* d4)
         :cmd_type(ct), data1(d1), data2(d2), data4{d4}
      {
         data3.ptr = d3;
      }
      Command(CmdType ct, std::uint32_t d1, std::uint32_t d2, std::uint8_t const* b, void* d4);
      CmdType cmd_type;
      std::uint32_t data1; //generic uint32 value
      std::uint32_t data2; //generic uint32 value
      union {
         void* ptr; //This is used when data size is bigger than SMALL_DATA_SIZE
         std::uint8_t bytes[SMALL_DATA_SIZE]; //This is used when data size is less than or equal to SMALL_DATA_SIZE
      } data3;
      void* data4; //generic pointer value
   };

   constexpr std::size_t COMMAND_SIZE = sizeof(Command);

}
