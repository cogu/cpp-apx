#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

namespace apx
{
   enum class TypeCode : unsigned char {
      None,
      UInt8,       //'C'
      UInt16,      //'S'
      UInt32,      //'L'
      UInt64,      //'Q' From APX IDL v1.3
      Int8,        //'c'
      Int16,       //'s'
      Int32,       //'l'
      Int64,       //'q' From APX IDL v1.3
      Char,        /*'a' Latin1-encoded character. From APX IDL v1.3.
                         In APX IDL v1.2, the encoding of 'a' just meant "string" with unspecified encoding */
      Char8,       //'A' UTF-8 character (char8_t in C++20, uint8_t otherwise). From APX IDL v1.3
      Char16,      //'u' UTF-16 character (char16_t). Reserved for APX IDL v1.4
      Char32,      //'U' UTF-32 character (char32_t). Reserved for APX IDL v1.4
      Bool,        //'b' From APX IDL v1.3
      Byte,        //'B' From APX IDL v1.3
      Record,      // {}
      TypeRefId,   // TYPE ID (integer)
      TypeRefName, // TYPE NAME (string)
      TypeRefPtr   // Pointer to element
   };
   enum class TokenClass : unsigned char {
      None,
      DataElement,         //APX IDL v1.3
      GroupDeclaration,    //APX IDL v1.4 (RESERVED)
      FunctionDeclaration  //APX IDL v1.4 (RESERVED)
   };

   enum class SizeType : unsigned char {
      None,
      UInt8,
      UInt16,
      UInt32,
   };

   enum class ScalarStorageType : unsigned char {
      None,
      Int32,
      UInt32,
      Int64,
      UInt64,
      Bool
   };

   enum class RangeCheckState : unsigned char {
      NotChecked,
      CheckOK,
      CheckFail
   };

   enum class ProgramType : unsigned char { Unpack, Pack };
   enum class PortType : unsigned char { RequirePort, ProvidePort };
   enum class ArrayType : unsigned char { None, UInt8, UInt16, UInt32 };
   enum class Mode : unsigned char { Client, Server };

   using type_id_t = std::uint32_t;
   using port_id_t = std::uint32_t;
   using element_id_t = std::uint32_t;
   using computation_id_t = std::uint32_t;
   constexpr type_id_t INVALID_TYPE_ID = static_cast<type_id_t>(-1);
   constexpr port_id_t INVALID_PORT_ID = static_cast<port_id_t>(-1);
   constexpr element_id_t INVALID_ELEMENT_ID = static_cast<element_id_t>(-1);
   constexpr computation_id_t INVALID_COMPUTATION_ID = static_cast<computation_id_t>(-1);

   constexpr std::uint16_t MAX_TYPE_REF_FOLLOW_COUNT = 255U;

   constexpr std::uint32_t PORT_DATA_ADDRESS_START        = 0x0u;
   constexpr std::uint32_t PORT_DATA_ADDRESS_ALIGNMENT    = 0x400u;      //1KB
   constexpr std::uint32_t DEFINITION_ADDRESS_START       = 0x4000000u;  //64MB
   constexpr std::uint32_t DEFINITION_ADDRESS_ALIGNMENT   = 0x40000u;    //256kB
   constexpr std::uint32_t PORT_COUNT_ADDRESS_START       = 0x8000000u;  //128MB
   constexpr std::uint32_t PORT_COUNT_ADDRESS_ALIGNMENT   = 0x400u;      //1KB
   constexpr std::uint32_t USER_DEFINED_ADDRESS_START     = 0x20000000u; //128MB
   constexpr std::uint32_t USER_DEFINED_ADDRESS_ALIGNMENT = 0x1000u;     //4KB

}