#include "pch.h"
#include "cpp-apx/qt_serializer.h"
#include <array>

using namespace apx::vm;

namespace apx_test
{

   TEST(QSerializer, SetInvalidBuffer)
   {
      std::array<std::uint8_t, 1> buf;
      apx::vm::QSerializer serializer;
      ASSERT_EQ(serializer.set_write_buffer(nullptr, 1), APX_INVALID_ARGUMENT_ERROR);
      ASSERT_EQ(serializer.set_write_buffer(buf.data(), 0), APX_INVALID_ARGUMENT_ERROR);
   }

   TEST(QSerializer, SetScalar)
   {
      QSerializer serializer;
      QVariant sv(18);
      ASSERT_EQ(serializer.set_value(sv), APX_NO_ERROR);
   }
}
