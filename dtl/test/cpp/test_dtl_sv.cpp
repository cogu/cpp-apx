#include "pch.h"
#include <iostream>
#include "dtl.hpp"

using namespace std;
namespace dtl
{
   TEST(ScalarTest, CreateEmptyScalar)
   {
      auto sv1 = dtl::make_sv();
      EXPECT_FALSE(sv1->has_value());
      EXPECT_EQ(sv1->dv_type(), dtl::ValueType::Scalar);
      EXPECT_EQ(sv1->sv_type(), dtl::ScalarType::None);
   }

   TEST(ScalarTest, VerifyUpAndDownCasting)
   {
      auto sv1 = dtl::make_sv();
      EXPECT_EQ(sv1.use_count(), 1);

      auto dv = dynamic_pointer_cast<dtl::Value>(sv1);
      EXPECT_EQ(dv.use_count(), 2);

      auto sv2 = dynamic_pointer_cast<dtl::Scalar>(dv);
      EXPECT_EQ(sv2.use_count(), 3);
      EXPECT_EQ(sv2->dv_type(), dtl::ValueType::Scalar);
      EXPECT_EQ(sv2->sv_type(), dtl::ScalarType::None);
   }


   TEST(ScalarTest, CreateInt32Scalar)
   {
      bool ok;
      auto sv1 = dtl::make_sv_int32();
      EXPECT_TRUE(sv1->has_value());
      EXPECT_EQ(sv1->dv_type(), dtl::ValueType::Scalar);
      EXPECT_EQ(sv1->sv_type(), dtl::ScalarType::Int32);
      EXPECT_EQ(sv1.use_count(), 1);
      EXPECT_EQ(sv1->to_i32(ok), 0);
      EXPECT_TRUE(ok);


      auto sv2 = sv1;
      sv2->set(-1);
      EXPECT_EQ(sv2->dv_type(), dtl::ValueType::Scalar);
      EXPECT_EQ(sv2->sv_type(), dtl::ScalarType::Int32);
      EXPECT_EQ(sv2.use_count(), 2);
      EXPECT_EQ(sv2->to_i32(ok), -1);
      EXPECT_TRUE(ok);
   }

   TEST(ScalarTest, CreateUInt32Scalar)
   {
      bool ok;
      auto sv1 = dtl::make_sv_uint32();
      EXPECT_TRUE(sv1->has_value());
      EXPECT_EQ(sv1->dv_type(), dtl::ValueType::Scalar);
      EXPECT_EQ(sv1->sv_type(), dtl::ScalarType::UInt32);
      EXPECT_EQ(sv1.use_count(), 1);
      EXPECT_EQ(sv1->to_u32(ok), (uint32_t)0u);      
      EXPECT_TRUE(ok);
      EXPECT_EQ(sv1->to_i32(ok), 0);
      EXPECT_TRUE(ok);

      auto sv2 = sv1;
      sv2->set((uint32_t)0xFFFFFFFF);
      EXPECT_EQ(sv2->dv_type(), dtl::ValueType::Scalar);
      EXPECT_EQ(sv2->sv_type(), dtl::ScalarType::UInt32);
      EXPECT_EQ(sv2.use_count(), 2);
      EXPECT_EQ(sv2->to_u32(ok), (uint32_t)0xFFFFFFFF);
      EXPECT_TRUE(ok);
      EXPECT_EQ(sv1->to_i32(ok), -1);
      EXPECT_TRUE(ok);
   }

   TEST(ScalarTest, CreateInt64Scalar)
   {
      bool ok;
      auto sv1 = dtl::make_sv_int64();
      EXPECT_TRUE(sv1->has_value());
      EXPECT_EQ(sv1->dv_type(), dtl::ValueType::Scalar);
      EXPECT_EQ(sv1->sv_type(), dtl::ScalarType::Int64);
      EXPECT_EQ(sv1.use_count(), 1);
      EXPECT_EQ(sv1->to_i64(ok), (int64_t)0u);
      EXPECT_TRUE(ok);
      EXPECT_EQ(sv1->to_i32(ok), 0);
      EXPECT_TRUE(ok);

      auto sv2 = sv1;
      sv2->set((int64_t)-1LL);
      EXPECT_EQ(sv2->dv_type(), dtl::ValueType::Scalar);
      EXPECT_EQ(sv2->sv_type(), dtl::ScalarType::Int64);
      EXPECT_EQ(sv2.use_count(), 2);
      EXPECT_EQ(sv2->to_i64(ok), (int64_t)-1LL);
      EXPECT_TRUE(ok);
      EXPECT_EQ(sv1->to_i32(ok), -1);
      EXPECT_TRUE(ok);

   }

   TEST(ScalarTest, CreateUInt64Scalar)
   {
      bool ok;
      auto sv1 = dtl::make_sv_uint64();
      EXPECT_TRUE(sv1->has_value());
      EXPECT_EQ(sv1->dv_type(), dtl::ValueType::Scalar);
      EXPECT_EQ(sv1->sv_type(), dtl::ScalarType::UInt64);
      EXPECT_EQ(sv1.use_count(), 1);
      EXPECT_EQ(sv1->to_u64(ok), (uint64_t)0u);
      EXPECT_TRUE(ok);

      auto sv2 = sv1;
      sv2->set((uint64_t) 0xFFFFULL);
      EXPECT_EQ(sv2->dv_type(), dtl::ValueType::Scalar);
      EXPECT_EQ(sv2->sv_type(), dtl::ScalarType::UInt64);
      EXPECT_EQ(sv2.use_count(), 2);
      EXPECT_EQ(sv2->to_u64(ok), (uint64_t)0xFFFFULL);
      EXPECT_TRUE(ok);
      EXPECT_EQ(sv1->to_i32(ok), 0xFFFF);
      EXPECT_TRUE(ok);
   }

   TEST(ScalarTest, CreateNumericString)
   {
      auto sv = dtl::make_sv_string("255");
      bool ok = false;
      EXPECT_EQ(sv->to_string(), "255"s);
      EXPECT_EQ(sv->to_i32(ok), (int32_t)255);
      EXPECT_TRUE(ok);
      EXPECT_EQ(sv->to_u32(ok), (uint32_t)255);
      EXPECT_TRUE(ok);
      EXPECT_EQ(sv->to_i64(ok), (int64_t)255);
      EXPECT_TRUE(ok);
      EXPECT_EQ(sv->to_u64(ok), (uint64_t)255);
      EXPECT_TRUE(ok);
   }

   TEST(ScalarTest, CreateFromBoundedString)
   {
      const char* begin = "first, second";
      const char* end = begin + 5;
      bool ok = false;
      auto sv = dtl::make_sv_string(begin, end);
      EXPECT_EQ(sv->to_string(), "first");
      EXPECT_EQ(sv->to_i32(ok), 0);
      EXPECT_FALSE(ok);
   }

}