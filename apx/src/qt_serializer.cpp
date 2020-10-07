#include "cpp-apx/qt_serializer.h"
#include "cpp-apx/pack.h"
#include <cassert>
#include <iostream> //DEBUG

namespace apx
{
   namespace vm
   {
      QSerializer::~QSerializer(){}
      void QSerializer::reset()
      {
      }

      apx::error_t QSerializer::set_write_buffer(std::uint8_t* buf, std::size_t len)
      {
         if ((buf != nullptr) && (len > 0))
         {
            reset_buffer(buf, len);
            return APX_NO_ERROR;
         }
         return APX_INVALID_ARGUMENT_ERROR;
      }

      error_t QSerializer::set_value(QVariant const &sv)
      {
        std::cout << sv.type() << std::endl;
        return APX_NO_ERROR;
      }

      void QSerializer::reset_buffer(std::uint8_t* buf, std::size_t len)
      {
         m_buffer.begin = buf;
         m_buffer.end = buf + len;
         m_buffer.next = buf;
         m_buffer.adjusted_next = nullptr;
      }
   }
}
