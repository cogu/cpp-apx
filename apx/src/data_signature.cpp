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

#include <typeinfo>
#include "cpp-apx/data_signature.h"
#include "cpp-apx/signature_parser.h"

namespace apx
{
   apx::error_t DataSignature::parse_effective(std::string const& str)
   {
      apx::error_t retval = APX_NO_ERROR;
      SignatureParser parser;
      char const* begin = str.data();
      char const* end = begin + str.size();
      auto result = parser.parse_data_signature(begin, end);
      if (result == end)
      {
         effective_element.reset(parser.take_data_element());
      }
      else
      {
         if ((result == nullptr) || (result == begin))
         {
            retval = APX_PARSE_ERROR;
         }
         else
         {
            retval = APX_STRAY_CHARACTERS_AFTER_PARSE_ERROR;
         }
      }
      return retval;
   }
}

