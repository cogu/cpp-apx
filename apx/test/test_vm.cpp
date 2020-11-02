#include "pch.h"
#include <array>
#include <cstring>
#include "cpp-apx/vm.h"

using namespace apx::vm;

namespace apx_test
{
   TEST(VM, ParseSimpleHeader)
   {
      Program const program{ 'V', 'M', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT8_SIZE };
      apx::VirtualMachine vm;
      ASSERT_EQ(vm.select_program(program), APX_NO_ERROR);
   }

   TEST(VM, PackUint8Value)
   {
      Program const program{ 'V', 'M', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_PACK | VARIANT_U8, UINT8_SIZE,
         OPCODE_PACK | (VARIANT_U8 << INST_VARIANT_SHIFT)
      };
      std::array<std::uint8_t, UINT8_SIZE> buf;
      std::memset(buf.data(), 0, buf.size());
      apx::VirtualMachine vm;
      ASSERT_EQ(vm.select_program(program), APX_NO_ERROR);
      ASSERT_EQ(vm.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      auto sv = dtl::make_sv<std::uint32_t>(0xFF);
      ASSERT_EQ(vm.pack_value(sv), APX_NO_ERROR);
      ASSERT_EQ(buf[0], 0xFF);
      sv->set((uint32_t) 0u);
      ASSERT_EQ(vm.select_program(program), APX_NO_ERROR);
      ASSERT_EQ(vm.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(vm.pack_value(sv), APX_NO_ERROR);
      ASSERT_EQ(buf[0], 0x0u);
      sv->set((uint32_t) 0x12u);
      ASSERT_EQ(vm.select_program(program), APX_NO_ERROR);
      ASSERT_EQ(vm.set_write_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(vm.pack_value(sv), APX_NO_ERROR);
      ASSERT_EQ(buf[0], 0x12u);
   }

   TEST(VM, UnpackUint8Value)
   {
      Program const program{ 'V', 'M', MAJOR_VERSION, MINOR_VERSION, HEADER_PROG_TYPE_UNPACK | VARIANT_U8, UINT8_SIZE,
         OPCODE_UNPACK | (VARIANT_U8 << INST_VARIANT_SHIFT)
      };
      std::array<std::uint8_t, UINT8_SIZE> buf;
      std::memset(buf.data(), 0, buf.size());
      apx::VirtualMachine vm;
      buf[0] = 0x00U;
      ASSERT_EQ(vm.select_program(program), APX_NO_ERROR);
      ASSERT_EQ(vm.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      dtl::ScalarValue sv;
      ASSERT_EQ(sv.use_count(), 0);
      ASSERT_EQ(vm.unpack_value(sv), APX_NO_ERROR);
      ASSERT_EQ(sv.use_count(), 1);
      bool ok;
      ASSERT_EQ(sv->to_u32(ok), 0u);
      ASSERT_TRUE(ok);

      buf[0] = 0xffU;
      ASSERT_EQ(vm.select_program(program), APX_NO_ERROR);
      ASSERT_EQ(vm.set_read_buffer(buf.data(), buf.size()), APX_NO_ERROR);
      ASSERT_EQ(vm.unpack_value(sv), APX_NO_ERROR);
      ASSERT_EQ(sv.use_count(), 1);
      ASSERT_EQ(sv->to_u32(ok), 255u);
      ASSERT_TRUE(ok);
   }

}
