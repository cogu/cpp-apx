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
#ifdef QT_API
#error("QT API NOT YET IMPLEMENTED");
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
   protected:
      apx::error_t parse_program_header();
      std::uint8_t const* m_begin{ nullptr };
      std::uint8_t const* m_end{ nullptr };
      std::uint8_t const* m_next{ nullptr };
   };

}