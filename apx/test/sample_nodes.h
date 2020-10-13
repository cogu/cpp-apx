/*****************************************************************************
* \file      sample_nodes.h
* \author    Conny Gustafsson
* \date      2020-10-12
* \brief     Sample APX definitions for unit test purposes
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
#include <cstddef>

namespace apx
{
   constexpr std::size_t TESTNODE1_IN_DATA_LEN = 1u;
   constexpr std::size_t TESTNODE1_OUT_DATA_LEN = 4u;
   constexpr std::size_t TESTNODE2_IN_DATA_LEN = 1u;
   constexpr std::size_t TESTNODE2_OUT_DATA_LEN = 3u;
   constexpr std::size_t TESTNODE5_IN_DATA_LEN = 3u;
   constexpr std::size_t TESTNODE5_OUT_DATA_LEN = 1u;

   extern const char* g_sample_node1;
   extern const char* g_sample_node2;
   extern const char* g_sample_node3;
   extern const char* g_sample_node4;
   extern const char* g_sample_node5;
   extern const char* g_sample_node6;

}
