#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cstdint>
#include <stdexcept>

namespace apx
{
   enum class TypeAttributeType
   {
      None,
      ValueTable,  
      RationalScaling,
      DynamicLength
   };

   struct Computation
   {
      Computation() : lower_range{ 0u }, upper_range{ 0u }, is_signed_range{ false }{}
      Computation(uint32_t lower) { set_range(lower, lower); }
      Computation(int32_t lower) { set_range(lower, lower); }
      Computation(uint32_t lower, uint32_t upper) { set_range(lower, upper); }
      Computation(int32_t lower, int32_t upper) { set_range(lower, upper); }
      virtual ~Computation() {}
      void set_range(uint32_t lower, uint32_t upper)
      {         
         if (upper < lower)
         {
            throw std::invalid_argument("upper limit cannot be lower than lower limit");
         }
         lower_range.u32 = lower;
         upper_range.u32 = upper;
         is_signed_range = false;
      }
      void set_range(int32_t lower, int32_t upper)
      {
         if (upper < lower)
         {
            throw std::invalid_argument("upper limit cannot be lower than lower limit");
         }
         lower_range.i32 = lower;
         upper_range.i32 = upper;
         is_signed_range = true;
      }
      union
      {
         uint32_t u32; 
         int32_t i32;         
      } lower_range;
      union 
      {
         uint32_t u32; 
         int32_t i32;         
      } upper_range;
      bool is_signed_range;
   };

   struct ValueTable : public Computation
   {       
      ValueTable() = default;
      ValueTable(int32_t lower_range, int32_t upper_range):Computation{ lower_range, upper_range } {}
      std::vector<std::string> values;
   };

   struct RationalScaling : public Computation
   {
      RationalScaling() = default;
      RationalScaling(double offset_, int32_t numerator_, int32_t denominator_, std::string& unit_) :
         offset{ offset_ },
         numerator{ numerator_ },
         denominator{ denominator_ },
         unit{ unit_ } {}
      RationalScaling(double offset_, int32_t numerator_, int32_t denominator_, const char* unit_) :
         offset{ offset_ },
         numerator{ numerator_ },
         denominator{ denominator_ },
         unit{ unit_ } {}
      double offset = 0.0;
      int32_t numerator = 1;
      int32_t denominator = 0;      
      std::string unit;
   };
   
   struct TypeAttributes
   {
      std::vector<std::unique_ptr<Computation>> computations;
      uint32_t dynamic_length = 0u;
   };

}