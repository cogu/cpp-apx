#include "pch.h"
#include <iostream>
#include "dtl/dtl.hpp"

using namespace std;

namespace dtl
{
   TEST(ArrayTest, CreateEmptyArray)
   {
      auto av1 = dtl::make_av();
      EXPECT_EQ(av1->dv_type(), dtl::ValueType::Array);
      EXPECT_EQ(av1->length(), 0u);
   }

   TEST(ArrayTest, PushScalars)
   {
      {
         bool ok;
         auto av1 = dtl::make_av();
         av1->push(dtl::make_sv_uint32(0u));
         av1->push(dtl::make_sv_uint32(1u));
         av1->push(dtl::make_sv_uint32(2u));
         EXPECT_EQ(av1->length(), 3u);
         auto sv = std::dynamic_pointer_cast<Scalar>(av1->at(0));
         EXPECT_EQ(sv.use_count(), 2);
         EXPECT_EQ(sv->to_u32(ok), (uint32_t)0u);
         EXPECT_TRUE(ok);
         sv = std::dynamic_pointer_cast<Scalar>(av1->at(1));
         EXPECT_EQ(sv.use_count(), 2);
         EXPECT_EQ(sv->to_u32(ok), (uint32_t)1u);
         EXPECT_TRUE(ok);
      }
   }
}
