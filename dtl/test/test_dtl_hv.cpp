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
      auto sv = dtl::sv_cast(hv->at("First"s));
      EXPECT_EQ(sv.use_count(), 2);
      EXPECT_EQ(sv->sv_type(), dtl::ScalarType::UInt32);
      EXPECT_EQ(sv->to_u32(ok), (uint32_t)0u);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(hv->at("Second"s));
      EXPECT_EQ(sv->to_u32(ok), (uint32_t)1u);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(hv->at("Third"s));
      EXPECT_EQ(sv->to_u32(ok), (uint32_t)2u);
      EXPECT_TRUE(ok);
   }
   TEST(HashTest, CreateHashValueUsingInitializer)
   {
      auto hv = dtl::make_hv({ {"First", dtl::make_sv<std::uint8_t>(10u)},
                               {"Second", dtl::make_sv<std::uint8_t>(20u)} });
      EXPECT_EQ(hv->length(), 2);
      auto ok = false;
      auto sv = dtl::sv_cast(hv->at("First"s));
      EXPECT_EQ(sv->to_u32(ok), (uint32_t)10u);
      EXPECT_TRUE(ok);
      sv = dtl::sv_cast(hv->at("Second"s));
      EXPECT_EQ(sv->to_u32(ok), (uint32_t)20u);
      EXPECT_TRUE(ok);
   }
}