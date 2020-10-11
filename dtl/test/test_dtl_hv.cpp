#include "pch.h"
#include <iostream>
#include "dtl/dtl.hpp"

using namespace std;

namespace dtl
{
   TEST(HashTest, CreateEmptyHash)
   {
      auto hv = dtl::make_hv();
      EXPECT_EQ(hv->dv_type(), dtl::ValueType::Hash);
      EXPECT_EQ(hv->length(), 0u);
   }

   TEST(HashTest, InsertScalars)
   {
      bool ok = false;
      auto hv = dtl::make_hv();
      hv->insert( { "First"s, dtl::make_sv<std::uint32_t>(0u) });
      hv->insert( { "Second"s, dtl::make_sv<std::uint32_t>(1u) });
      hv->insert( { "Third"s, dtl::make_sv<std::uint32_t>(2u) });
      EXPECT_EQ(hv->length(), 3);
      auto sv = dynamic_pointer_cast<dtl::Scalar>(hv->at("Second"s));
      EXPECT_EQ(sv.use_count(), 2);
      EXPECT_EQ(sv->sv_type(), dtl::ScalarType::UInt32);
      EXPECT_EQ(sv->to_u32(ok), (uint32_t)1u);
      EXPECT_TRUE(ok);
   }
}