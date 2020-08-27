#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace apx
{
   enum class TypeCode : unsigned char {
      None,
      UInt8,       //'C'
      UInt16,      //'S'
      UInt32,      //'L'
      UInt64,      //'U'
      Int8,        //'c'
      Int16,       //'s'
      Int32,       //'l'
      Int64,       //'u'
      Char,        //'a'
      Bool,        //'b'
      Byte,        //'B'
      Record,      // {}
      TypeRefId,   // TYPE ID (integer)
      TypeRefName, // TYPE NAME (string)
      TypeRefPtr   // Pointer to element
   };
   enum class TokenClass : unsigned char {
      None,
      DataElement,         //APX IDL v1.3
      GroupDeclaration,    //APX IDL v1.4
      FunctionDeclaration  //APX IDL v1.4
   };
   enum class PortType : unsigned char { RequirePort, ProvidePort };
   enum class ArrayType: unsigned char { None, UInt8, UInt16, UInt32};

   using type_id_t = std::uint32_t;
   using port_id_t = std::uint32_t;
   constexpr type_id_t INVALID_TYPE_ID = static_cast<type_id_t>(-1);
   constexpr port_id_t INVALID_PORT_ID = static_cast<port_id_t>(-1);
}