#include "pch.h"
#include <string>
#include "cpp-apx/computation.h"
#include "cpp-apx/attribute_parser.h"

using namespace apx;
using namespace std::string_literals;

namespace apx_test
{
   TEST(Computation, LoadStoreValueTable)
   {
      std::string const expected{ "VT(0,1,\"First\",\"Second\")" };
      auto computation = std::make_unique<ValueTable>();
      computation->values.push_back("First"s);
      computation->values.push_back("Second"s);
      computation->set_range(static_cast<std::uint32_t>(0u), static_cast<std::uint32_t>(1u));
      EXPECT_EQ(computation->to_string(), expected);
      AttributeParser parser;
      apx::TypeAttributes attr;
      EXPECT_EQ(parser.parse_type_attributes(expected, attr), APX_NO_ERROR);
      EXPECT_EQ(attr.computations.size(), 1u);
      auto const* parsed_computation = dynamic_cast<ValueTable*>(attr.computations.at(0).get());
      EXPECT_TRUE(parsed_computation);
      EXPECT_EQ(*computation, *parsed_computation);
   }

   TEST(Computation, LoadStoreRationalScaling)
   {
      std::string const expected{ "RS(0,65280,0,1,64,\"km/h\")" };
      auto computation = std::make_unique<RationalScaling>(0.0, 1, 64, "km/h");
      computation->set_range(static_cast<std::uint32_t>(0u), static_cast<std::uint32_t>(65280u));
      EXPECT_EQ(computation->to_string(), expected);
      AttributeParser parser;
      apx::TypeAttributes attr;
      EXPECT_EQ(parser.parse_type_attributes(expected, attr), APX_NO_ERROR);
      EXPECT_EQ(attr.computations.size(), 1u);
      auto const* parsed_computation = dynamic_cast<RationalScaling*>(attr.computations.at(0).get());
      EXPECT_TRUE(parsed_computation);
      EXPECT_EQ(*computation, *parsed_computation);
   }
}