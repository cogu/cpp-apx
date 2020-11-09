#pragma once

#include <cstdint>
#include <cstddef>

namespace rmf
{
   enum class FileType
   {
      Fixed,
      Dynamic8,
      Dynamic16,
      Dynamic32,
      Device,
      Stream,
      CompressedFixed
   };

   enum class DigestType { None, SHA256 };

   constexpr std::uint32_t REMOTE_ADDRESS_BIT = 0x80000000ul; //This is overlayed with CMD_HIGH_BIT
   constexpr std::uint32_t INVALID_ADDRESS = 0x7FFFFFFFul;
   constexpr std::uint32_t ADDRESS_MASK_INTERNAL = 0x7FFFFFFFul;
   constexpr std::size_t SHA256_SIZE = 32u;
   constexpr std::uint32_t CMD_AREA_START_ADDRESS = 0x3FFFFC00;
   constexpr std::size_t CMD_AREA_SIZE = 1024; //1KB
   constexpr std::uint32_t CMD_AREA_END_ADDRESS = CMD_AREA_START_ADDRESS + CMD_AREA_SIZE; //First byte after last valid address

}