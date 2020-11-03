/*****************************************************************************
* \file      computation.h
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
#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <memory>
#include "cpp-apx/types.h"
#include "cpp-apx/error.h"

namespace apx
{
   enum class ComputationType
   {
      ValueTable,
      RationalScaling
   };

   struct Computation
   {
      Computation(ComputationType type) : computation_type{ type }, lower_limit { 0u }, upper_limit{ 0u }, is_signed_range{ false }{}
      Computation(ComputationType type, uint32_t lower) : computation_type{ type } { set_range(lower, lower); }
      Computation(ComputationType type, int32_t lower) : computation_type{ type } { set_range(lower, lower); }
      Computation(ComputationType type, uint32_t lower, uint32_t upper) : computation_type{ type } { set_range(lower, upper); }
      Computation(ComputationType type, int32_t lower, int32_t upper) : computation_type{ type } { set_range(lower, upper); }
      virtual ~Computation() {}
      void set_range(uint32_t lower, uint32_t upper)
      {
         if (upper < lower)
         {
            throw std::invalid_argument("upper limit cannot be lower than lower limit");
         }
         lower_limit.u32 = lower;
         upper_limit.u32 = upper;
         is_signed_range = false;
      }
      void set_range(int32_t lower, int32_t upper)
      {
         if (upper < lower)
         {
            throw std::invalid_argument("upper limit cannot be lower than lower limit");
         }
         lower_limit.i32 = lower;
         upper_limit.i32 = upper;
         is_signed_range = true;
      }
      virtual std::string to_string() = 0;

      ComputationType computation_type;
      union
      {
         uint32_t u32;
         int32_t i32;
      } lower_limit;
      union
      {
         uint32_t u32;
         int32_t i32;
      } upper_limit;
      bool is_signed_range;

   protected:
      std::string limit_to_string();
      bool limit_equals(Computation const& other) const;
   };

   struct ValueTable : public Computation
   {
      ValueTable():Computation(ComputationType::ValueTable){}
      ValueTable(int32_t lower_limit, int32_t upper_limit) :Computation{ ComputationType::ValueTable, lower_limit, upper_limit } {}
      std::string to_string() override;
      std::vector<std::string> values;
      bool operator ==(ValueTable const& other) const;
   };

   struct RationalScaling : public Computation
   {
      RationalScaling() : Computation(ComputationType::RationalScaling) {}
      RationalScaling(double offset_, int32_t numerator_, int32_t denominator_, std::string& unit_) :
         Computation(ComputationType::RationalScaling),
         offset{ offset_ },
         numerator{ numerator_ },
         denominator{ denominator_ },
         unit{ unit_ } {}
      RationalScaling(double offset_, int32_t numerator_, int32_t denominator_, const char* unit_) :
         Computation(ComputationType::RationalScaling),
         offset{ offset_ },
         numerator{ numerator_ },
         denominator{ denominator_ },
         unit{ unit_ } {}
      std::string to_string() override;
      bool operator ==(RationalScaling const& other) const;

      double offset = 0.0;
      int32_t numerator = 1;
      int32_t denominator = 0;
      std::string unit;
   };

   class ComputationList
   {
   public:
      void set_id(element_id_t new_id) { m_computation_id = new_id; }
      computation_id_t get_id() const { return m_computation_id; }
      apx::error_t append_clone_of_computation(Computation const* computation);
      std::size_t get_computation_length() const { return m_computations.size(); }
      Computation const* get_computation(std::size_t id) const;
   protected:
      apx::computation_id_t m_computation_id{ INVALID_COMPUTATION_ID };
      std::vector<std::unique_ptr<Computation>> m_computations;
   };

}
