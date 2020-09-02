#include "cpp-apx/data_type.h"

apx::TypeCode apx::DataType::resolve_type_code()
{
   apx::DataElement* data_element = dsg.element.get();
   if (data_element != nullptr)
   {
      return data_element->resolve_type_code();
   }
   return apx::TypeCode::None;
}
