/*****************************************************************************
* \file      computation.cpp
* \author    Conny Gustafsson
* \date      2020-10-25
* \brief     Computation classes
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
#include <iostream>
#include <sstream>
#include <iomanip>
#include <array>
#include <cassert>
#include "cpp-apx/computation.h"

namespace apx
{

   std::string Computation::limit_to_string()
   {
      std::string retval;
      if (is_signed_range)
      {
         retval.append(std::to_string(lower_limit.i32));
         retval.push_back(',');
         retval.append(std::to_string(upper_limit.i32));
      }
      else
      {
         retval.append(std::to_string(lower_limit.u32));
         retval.push_back(',');
         retval.append(std::to_string(upper_limit.u32));

      }
      return retval;
   }

   bool Computation::limit_equals(Computation const& other) const
   {
      if (is_signed_range)
      {
         if ((lower_limit.i32 != other.lower_limit.i32) || (upper_limit.i32 != other.upper_limit.i32) )
         {
            return false;
         }
      }
      else
      {
         if ((lower_limit.u32 != other.lower_limit.u32) || (upper_limit.u32 != other.upper_limit.u32))
         {
            return false;
         }
      }
      return true;
   }

   std::string ValueTable::to_string()
   {
      std::string retval{ "VT(" };
      retval.append(limit_to_string());
      retval.push_back(',');
      auto const num_items = values.size();
      for (std::size_t i=0u; i < num_items; i++)
      {
         if (i > 0u)
         {
            retval.push_back(',');
         }
         retval.push_back('"');
         retval.append(values.at(i));
         retval.push_back('"');
      }
      retval.push_back(')');
      return retval;
   }

   bool ValueTable::operator==(ValueTable const& other) const
   {
      if (!limit_equals(other))
      {
         return false;
      }
      if (values.size() != other.values.size())
      {
         return false;
      }
      auto const num_items = values.size();
      for (std::size_t i = 0u; i < num_items; i++)
      {
         auto const& left = values.at(0);
         auto const& right = other.values.at(0);
         if (left != right)
         {
            return false;
         }
      }

      return true;
   }

   std::string RationalScaling::to_string()
   {
      std::ostringstream ss;
      ss << std::setprecision(8) << std::noshowpoint << offset;
      std::string retval{ "RS(" };
      retval.append(limit_to_string());
      retval.push_back(',');
      retval.append(ss.str());
      retval.push_back(',');
      retval.append(std::to_string(numerator));
      retval.push_back(',');
      retval.append(std::to_string(denominator));
      retval.push_back(',');
      retval.push_back('"');
      retval.append(unit);
      retval.push_back('"');
      retval.push_back(')');
      return retval;

   }

   bool RationalScaling::operator==(RationalScaling const& other) const
   {
      if (!limit_equals(other))
      {
         return false;
      }
      if (offset != other.offset)
      {
         return false;
      }
      if ( (numerator != other.numerator) || (denominator != other.denominator) )
      {
         return false;
      }
      if (unit != other.unit)
      {
         return false;
      }
      return true;
   }

   apx::error_t ComputationList::append_clone_of_computation(Computation const* computation)
   {
      std::unique_ptr<apx::ValueTable> vt;
      std::unique_ptr<apx::RationalScaling> rs;
      apx::ValueTable const* vt_tmp;
      apx::RationalScaling const* rs_tmp;
      switch (computation->computation_type)
      {
      case ComputationType::ValueTable:
         vt_tmp = dynamic_cast<apx::ValueTable const*>(computation);
         assert(vt_tmp != nullptr);
         vt = std::make_unique<apx::ValueTable>(*vt_tmp);
         m_computations.push_back(std::move(vt));
         break;
      case ComputationType::RationalScaling:
         rs_tmp = dynamic_cast<apx::RationalScaling const*>(computation);
         assert(rs_tmp != nullptr);
         rs = std::make_unique<apx::RationalScaling>(*rs_tmp);
         m_computations.push_back(std::move(rs));
         break;
      default:
         return APX_NOT_IMPLEMENTED_ERROR;
      }
      return APX_NO_ERROR;
   }

   Computation const* ComputationList::get_computation(std::size_t id) const
   {
      if (id < m_computations.size())
      {
         return m_computations.at(id).get();
      }
      return nullptr;
   }
}