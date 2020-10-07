#pragma once

#include <stack>
#include <string>
#include <iterator>
#include <variant>
#include "cpp-apx/vmdefs.h"
#include "cpp-apx/error.h"
#include <QVariant>

namespace apx
{
   namespace vm
   {
      class QSerializer
      {
      public:
         struct State
         {
            TypeCode type_code{ TypeCode::None };
         };
         struct WriteBuffer
         {
            std::uint8_t* begin{ nullptr };
            std::uint8_t* end{ nullptr };
            std::uint8_t* next{ nullptr };
            std::uint8_t* adjusted_next{ nullptr }; //Needed for dynamic arrays
         };
         QSerializer() { reset(); }
         ~QSerializer();
         void reset();
         apx::error_t set_write_buffer(std::uint8_t* buf, std::size_t len);
         apx::error_t set_value(QVariant const &sv);
      protected:
         WriteBuffer m_buffer;
         State* m_state{ nullptr };
         std::stack<State*> m_stack;
         void reset_buffer(std::uint8_t* buf, std::size_t len);

      };
   }
}
