#include "pch.h"
#include "bstr.hpp"
#include <cstring>

using namespace std;
TEST(BoundedString, lstrip) 
{
   const char* str = "  AA  ";
   const char* begin = &str[0];
   const char* end = begin + strlen(begin);
   const char* result = bstr::lstrip<char>(begin, end);
   EXPECT_EQ(result, begin+2);
}

TEST(BoundedString, rstrip)
{
   const char* str = "  AA  ";
   const char* begin = &str[0];
   const char* end = begin + strlen(begin);
   const char* result = bstr::rstrip<char>(begin, end);
   EXPECT_STREQ(result, end-2);
}

TEST(BoundedString, stringLiteral)
{
   const char* str = "\"Hello World\"";
   const char* begin = &str[0];
   const char* end = begin + strlen(begin);
   const char* result = bstr::match_pair<char>(begin, end, '"', '"', '\0');
   EXPECT_EQ(result, end-1);
}

TEST(BoundedString, stringLiteralWithEscape)
{
   const char* str = "\"Hello \\\"World\\\"\"";
   const char* begin = &str[0];
   const char* end = begin + strlen(begin);
   const char* result = bstr::match_pair<char>(begin, end, '"', '"', '\\');
   EXPECT_EQ(result, end - 1);
}

TEST(BoundedString, subStringMatch)
{
   const char* begin1 = "RNG";
   const char* end1 = begin1 + strlen(begin1);
   const char* result = bstr::match_str<char>(begin1, end1, "RNG");
   EXPECT_EQ(result, end1);

   const char* begin2 = "RNG(0,1)";
   const char* end2 = begin2 + strlen(begin2);
   result = bstr::match_str<char>(begin2, end2, "RNG");
   EXPECT_EQ(result, end2 - 5);
}