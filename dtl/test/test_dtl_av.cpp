#include "pch.h"
#include <iostream>
#include "dtl/dtl.hpp"

using namespace std;

namespace dtl
{
   TEST(ArrayTest, CreateEmptyArray)
   {
      auto av = dtl::make_av();
      EXPECT_EQ(av->dv_type(), dtl::ValueType::Array);
      EXPECT_EQ(av->length(), 0u);
   }

   TEST(ArrayTest, PushScalars)
   {
      {
         auto ok{ false };
         auto av = dtl::make_av();
         av->push(dtl::make_sv<std::uint32_t>(0u));
         av->push(dtl::make_sv<std::uint32_t>(1u));
         av->push(dtl::make_sv<std::uint32_t>(2u));
         EXPECT_EQ(av->length(), 3u);
         auto sv = dtl::sv_cast(av->at(0));
         EXPECT_EQ(sv->to_u32(ok), (uint32_t)0u);
         EXPECT_TRUE(ok);
         sv = dtl::sv_cast(av->at(1));
         EXPECT_EQ(sv->to_u32(ok), (uint32_t)1u);
         EXPECT_TRUE(ok);
         sv = dtl::sv_cast(av->at(2));
         EXPECT_EQ(sv->to_u32(ok), (uint32_t)2u);
         EXPECT_TRUE(ok);
      }
   }

   TEST(ArrayTest, CreateArrayUsingInitializer)
   {
      auto ok{ false };
      auto av = dtl::make_av({
         dtl::make_sv<std::uint32_t>(10u),
         dtl::make_sv<std::uint32_t>(20u),
         dtl::make_sv<std::uint32_t>(30u)
         });
      EXPECT_EQ(av->length(), 3u);
      EXPECT_EQ(dtl::sv_cast(av->at(0))->to_u32(ok), (uint32_t) 10u);
      EXPECT_TRUE(ok);
      EXPECT_EQ(dtl::sv_cast(av->at(1))->to_u32(ok), (uint32_t) 20u);
      EXPECT_TRUE(ok);
      EXPECT_EQ(dtl::sv_cast(av->at(2))->to_u32(ok), (uint32_t) 30u);
      EXPECT_TRUE(ok);
   }
}
