#include <stdexcept>
#include <cassert>
#include <algorithm>
#include <typeinfo>
#include "dtl/dtl.hpp"

namespace dtl
{
   constexpr std::size_t I32_STORAGE_ID = 0;
   constexpr std::size_t U32_STORAGE_ID = 1;
   constexpr std::size_t I64_STORAGE_ID = 2;
   constexpr std::size_t U64_STORAGE_ID = 3;
   constexpr std::size_t STR_STORAGE_ID = 4;
   constexpr std::size_t BOOL_STORAGE_ID = 5;
   constexpr std::size_t BYTEARRAY_STORAGE_ID = 6;

   static bool string_iequals(std::string const& a, std::string const& b)
   {
      if (a.size() != b.size())
      {
         return false;
      }
      return std::equal(a.begin(), a.end(), b.begin(),
         [](char a, char b)
         {
            return tolower(a) == tolower(b);
         });
   }

   ScalarType Scalar::sv_type() const
   {
      dtl::ScalarType retval{ dtl::ScalarType::None };
      if (m_sv_data.has_value())
      {
         switch (m_sv_data.value().index())
         {
         case I32_STORAGE_ID:
            retval = dtl::ScalarType::Int32;
            break;
         case U32_STORAGE_ID:
            retval = dtl::ScalarType::UInt32;
            break;
         case I64_STORAGE_ID:
            retval = dtl::ScalarType::Int64;
            break;
         case U64_STORAGE_ID:
            retval = dtl::ScalarType::UInt64;
            break;
         case STR_STORAGE_ID:
            retval = dtl::ScalarType::String;
            break;
         case BOOL_STORAGE_ID:
            retval = dtl::ScalarType::Bool;
            break;
         case BYTEARRAY_STORAGE_ID:
            retval = dtl::ScalarType::ByteArray;
            break;
         default:
            assert(0); //NOT IMPLEMENTED
         }
      }
      return retval;
   }

   void Scalar::reset()
   {
      m_sv_data.reset();
   }

   void Scalar::set(int32_t value)
   {
      m_sv_data = value;
   }

   void Scalar::set(uint32_t value)
   {
      m_sv_data = value;
   }

   void Scalar::set(int64_t value)
   {
      m_sv_data = value;
   }

   void Scalar::set(uint64_t value)
   {
      m_sv_data = value;
   }

   void Scalar::set(const std::string& value)
   {
      m_sv_data = value;
   }

   void Scalar::set(const char* value)
   {
      m_sv_data = std::string{ value };
   }

   void Scalar::set(const char* begin, const char* end)
   {
      std::size_t count = std::distance(begin, end);
      m_sv_data = std::string{ begin, count };
   }

   void Scalar::set(bool value)
   {
      m_sv_data = value;
   }

   void Scalar::set(const ByteArray& value)
   {
       m_sv_data = ByteArray();
       auto& tmp = std::get<ByteArray>(m_sv_data.value());
       tmp.assign(value.begin(), value.end());
   }

   int32_t Scalar::to_i32(bool& ok) const
   {
      ok = false;
      int32_t retval = 0;
      if (m_sv_data.has_value())
      {
         switch (m_sv_data.value().index())
         {
         case I32_STORAGE_ID:
            retval = std::get<int32_t>(m_sv_data.value());
            ok = true;
            break;
         case U32_STORAGE_ID:
         {
            uint32_t value = std::get<uint32_t>(m_sv_data.value());
            if (value <= UINT32_MAX)
            {
               retval = static_cast<int32_t>(value);
               ok = true;
            }
         }
         break;
         case I64_STORAGE_ID:
            retval = static_cast<int32_t>(std::get<int64_t>(m_sv_data.value()));
            ok = true;
            break;
         case U64_STORAGE_ID:
            retval = static_cast<int32_t>(std::get<uint64_t>(m_sv_data.value()));
            ok = true;
            break;
         case STR_STORAGE_ID:
            try
            {
               retval = static_cast<int32_t>(std::stol(std::get<std::string>(m_sv_data.value())));
               ok = true;
            }
            catch (std::invalid_argument)
            {

            }
            catch (std::out_of_range)
            {

            }
            break;
         case BOOL_STORAGE_ID:
            retval = std::get<bool>(m_sv_data.value()) ? 1 : 0;
            ok = true;
            break;
         case BYTEARRAY_STORAGE_ID:
            //NO CONVERSION ALLOWED
            break;
         }
      }
      else
      {
         ok = false;
      }
      return retval;
   }

   uint32_t Scalar::to_u32(bool& ok) const
   {
      ok = false;
      uint32_t retval = 0;
      if (m_sv_data.has_value())
      {
         switch (m_sv_data.value().index())
         {
         case I32_STORAGE_ID:
         {
            int32_t value = std::get<int32_t>(m_sv_data.value());
            if (value >= 0)
            {
               retval = static_cast<uint32_t>(value);
               ok = true;
            }
         }
         break;
         case U32_STORAGE_ID:
            retval = std::get<uint32_t>(m_sv_data.value());
            ok = true;
            break;
         case I64_STORAGE_ID:
            retval = static_cast<uint32_t>(std::get<int64_t>(m_sv_data.value()));
            ok = true;
            break;
         case U64_STORAGE_ID:
            retval = static_cast<uint32_t>(std::get<uint64_t>(m_sv_data.value()));
            ok = true;
            break;
         case STR_STORAGE_ID:
            try
            {
               retval = static_cast<uint32_t>(std::stoul(std::get<std::string>(m_sv_data.value())));
               ok = true;
            }
            catch (std::invalid_argument)
            {

            }
            catch (std::out_of_range)
            {

            }
            break;
         case BOOL_STORAGE_ID:
            retval = std::get<bool>(m_sv_data.value()) ? 1u : 0u;
            break;
         case BYTEARRAY_STORAGE_ID:
            //NO CONVERSION ALLOWED
            break;
         }
      }
      else
      {
         ok = false;
      }
      return retval;
   }

   int64_t Scalar::to_i64(bool& ok) const
   {
      ok = false;
      int64_t retval = 0;
      if (m_sv_data.has_value())
      {
         switch (m_sv_data.value().index())
         {
         case I32_STORAGE_ID:
            retval = static_cast<int64_t>(std::get<int32_t>(m_sv_data.value()));
            ok = true;
            break;
         case U32_STORAGE_ID:
            retval = static_cast<int64_t>(std::get<uint32_t>(m_sv_data.value()));
            ok = true;
            break;
         case I64_STORAGE_ID:
            retval = std::get<int64_t>(m_sv_data.value());
            ok = true;
            break;
         case U64_STORAGE_ID:
            retval = static_cast<int64_t>(std::get<uint64_t>(m_sv_data.value()));
            ok = true;
            break;
         case STR_STORAGE_ID:
            try
            {
               retval = static_cast<int64_t>(std::stoll(std::get<std::string>(m_sv_data.value())));
               ok = true;
            }
            catch (std::invalid_argument)
            {

            }
            catch (std::out_of_range)
            {

            }
            break;
         case BOOL_STORAGE_ID:
            retval = std::get<bool>(m_sv_data.value()) ? 1 : 0;
            break;
         case BYTEARRAY_STORAGE_ID:
            //NO CONVERSION ALLOWED
            break;
         }
      }
      else
      {
         ok = false;
      }
      return retval;
   }

   uint64_t Scalar::to_u64(bool& ok) const
   {
      ok = false;
      uint64_t retval = 0;
      if (m_sv_data.has_value())
      {
         switch (m_sv_data.value().index())
         {
         case I32_STORAGE_ID:
            retval = static_cast<uint64_t>(std::get<int32_t>(m_sv_data.value()));
            ok = true;
            break;
         case U32_STORAGE_ID:
            retval = static_cast<uint64_t>(std::get<uint32_t>(m_sv_data.value()));
            ok = true;
            break;
         case I64_STORAGE_ID:
            retval = static_cast<uint64_t>(std::get<uint32_t>(m_sv_data.value()));
            ok = true;
            break;
         case U64_STORAGE_ID:
            retval = std::get<uint64_t>(m_sv_data.value());
            ok = true;
            break;
         case STR_STORAGE_ID:
            try
            {
               retval = static_cast<uint64_t>(std::stoull(std::get<std::string>(m_sv_data.value())));
               ok = true;
            }
            catch (std::invalid_argument)
            {

            }
            catch (std::out_of_range)
            {

            }
            break;
         case BOOL_STORAGE_ID:
            retval = std::get<bool>(m_sv_data.value()) ? 1u : 0u;
            break;
         case BYTEARRAY_STORAGE_ID:
            //NO CONVERSION ALLOWED
            break;
         }
      }
      else
      {
         ok = false;
      }
      return retval;
   }

   std::string Scalar::to_string(bool& ok) const
   {
      std::string retval;
      if (m_sv_data.has_value())
      {
         ok = true;
         switch (m_sv_data.value().index())
         {
         case I32_STORAGE_ID:
            retval = std::to_string(std::get<int32_t>(m_sv_data.value()));
            break;
         case U32_STORAGE_ID:
            retval = std::to_string(std::get<uint32_t>(m_sv_data.value()));
            break;
         case I64_STORAGE_ID:
            retval = std::to_string(std::get<int64_t>(m_sv_data.value()));
            break;
         case U64_STORAGE_ID:
            retval = std::to_string(std::get<uint64_t>(m_sv_data.value()));
            break;
         case STR_STORAGE_ID:
            retval = std::get<std::string>(m_sv_data.value());
            break;
         case BOOL_STORAGE_ID:
            retval = std::get<bool>(m_sv_data.value()) ? std::string("true") : std::string("false");
            break;
         case BYTEARRAY_STORAGE_ID:
            //NO CONVERSION ALLOWED
            ok = false;
            break;
         }
      }
      else
      {
         ok = false;
      }
      return retval;
   }

   bool Scalar::to_bool(bool &ok) const
   {
      bool retval = false;
      std::string const true_string("true");
      std::string const false_string("false");
      std::string tmp;
      if (m_sv_data.has_value())
      {
         ok = true;
         switch (m_sv_data.value().index())
         {
         case I32_STORAGE_ID:
            retval = std::get<int32_t>(m_sv_data.value()) == 0? false : true;
            break;
         case U32_STORAGE_ID:
            retval = std::get<uint32_t>(m_sv_data.value()) == 0 ? false : true;
            break;
         case I64_STORAGE_ID:
            retval = std::get<int64_t>(m_sv_data.value()) == 0 ? false : true;
            break;
         case U64_STORAGE_ID:
            retval = std::get<uint64_t>(m_sv_data.value()) == 0 ? false : true;
            break;
         case STR_STORAGE_ID:
            tmp = std::get<std::string>(m_sv_data.value());
            if (string_iequals(tmp, true_string))
            {
               retval = true;
            }
            else if (string_iequals(tmp, false_string))
            {
               retval = false;
            }
            else
            {
               ok = false;
            }
            break;
         case BOOL_STORAGE_ID:
            retval = std::get<bool>(m_sv_data.value());
            break;
         case BYTEARRAY_STORAGE_ID:
            //NO CONVERSION ALLOWED
            break;
         default:
            ok = false;
         }
      }
      else
      {
         ok = false;
      }
      return retval;
   }

   dtl::ByteArray const & Scalar::get_byte_array() const
   {
      if (!m_sv_data.has_value() || (m_sv_data.value().index() != BYTEARRAY_STORAGE_ID) )
      {
         throw std::bad_typeid();
      }
      return std::get<dtl::ByteArray>(m_sv_data.value());
   }

   DynamicValue make_dv()
   {
      return std::make_shared<Value>(ValueType::NoneType);
   }

   std::shared_ptr<Scalar> make_sv()
   {
      return std::make_shared<Scalar>();
   }

   ScalarValue make_sv(const char* begin, const char* end)
   {
      auto sv = std::make_shared<Scalar>();
      sv->set(begin, end);
      return sv;
   }

   Array::Array(std::size_t initial_length) :dtl::Value(ValueType::Array)
   {
      if (initial_length > 0u)
      {
         m_av_data.resize(initial_length);
      }
   }

   std::shared_ptr<Array> make_av(std::size_t length)
   {
      return std::make_shared<Array>(length);
   }

   ArrayValue make_av(std::initializer_list<DynamicValue> initializer)
   {
      auto av = std::make_shared<Array>();
      for (auto v : initializer)
      {
         av->push(v);
      }
      return av;
   }

   Hash::Hash() : dtl::Value(ValueType::Hash)
   {
   }

   void Hash::insert(value_type& v)
   {
      m_hv_data.insert(v);
   }

   void Hash::insert(value_type&& v)
   {
      m_hv_data.insert(v);
   }

   void Hash::set(std::string const& key, DynamicValue value)
   {
      insert(std::make_pair(key, value));
   }

   dtl::Value const* Hash::cget(std::string const& key) const
   {
      auto it = m_hv_data.find(key);
      if (it != m_hv_data.end())
      {
         return it->second.get();
      }
      return nullptr;
   }


   dtl::Value const* Hash::cget(char const* key) const
   {
      auto it = m_hv_data.find(key);
      if (it != m_hv_data.end())
      {
         return it->second.get();
      }
      return nullptr;
   }

   std::shared_ptr<Hash> make_hv()
   {
      return std::make_shared<Hash>();
   }

   HashValue make_hv(std::initializer_list<std::pair<std::string, DynamicValue>> initializer)
   {
      auto hv = std::make_shared<Hash>();
      for (auto v : initializer)
      {
         hv->insert(v);
      }
      return hv;
   }
}