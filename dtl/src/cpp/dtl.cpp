#include <stdexcept>
#include "dtl.hpp"

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

namespace dtl
{
   constexpr int I32_ID = 0;
   constexpr int U32_ID = 1;
   constexpr int I64_ID = 2;
   constexpr int U64_ID = 3;
   constexpr int STR_ID = 4;

   ScalarType Scalar::sv_type()
   {
      dtl::ScalarType retval{ dtl::ScalarType::None };
      if (m_sv_data.has_value())
      {
         std::visit(overloaded{
            [&retval](int32_t v) { (void)v; retval = dtl::ScalarType::Int32; },
            [&retval](uint32_t v) { (void)v; retval = dtl::ScalarType::UInt32; },
            [&retval](int64_t v) { (void)v; retval = dtl::ScalarType::Int64; },
            [&retval](uint64_t v) { (void)v; retval = dtl::ScalarType::UInt64; },
            [&retval](std::string& v) { (void)v; retval = dtl::ScalarType::String; }
            }, m_sv_data.value());
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

   int32_t Scalar::to_i32(bool& ok) const
   {
      ok = false;
      int32_t retval = 0;
      if (m_sv_data.has_value())
      {
         switch (m_sv_data.value().index())
         {
         case I32_ID:
            retval = std::get<int32_t>(m_sv_data.value());
            ok = true;
            break;
         case U32_ID:
            retval = static_cast<int32_t>(std::get<uint32_t>(m_sv_data.value()));
            ok = true;
            break;
         case I64_ID:
            retval = static_cast<int32_t>(std::get<int64_t>(m_sv_data.value()));
            ok = true;
            break;
         case U64_ID:
            retval = static_cast<int32_t>(std::get<uint64_t>(m_sv_data.value()));
            ok = true;
            break;
         case STR_ID:
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
         }
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
         case I32_ID:
            retval = static_cast<uint32_t>(std::get<int32_t>(m_sv_data.value()));
            ok = true;
            break;
         case U32_ID:
            retval = std::get<uint32_t>(m_sv_data.value());
            ok = true;
            break;
         case I64_ID:
            retval = static_cast<uint32_t>(std::get<int64_t>(m_sv_data.value()));
            ok = true;
            break;
         case U64_ID:
            retval = static_cast<uint32_t>(std::get<uint64_t>(m_sv_data.value()));
            ok = true;
            break;
         case STR_ID:
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
         }
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
         case I32_ID:
            retval = static_cast<int64_t>(std::get<int32_t>(m_sv_data.value()));
            ok = true;
            break;
         case U32_ID:
            retval = static_cast<int64_t>(std::get<uint32_t>(m_sv_data.value()));
            ok = true;
            break;
         case I64_ID:
            retval = std::get<int64_t>(m_sv_data.value());
            ok = true;
            break;
         case U64_ID:
            retval = static_cast<int64_t>(std::get<uint64_t>(m_sv_data.value()));
            ok = true;
            break;
         case STR_ID:
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
         }
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
         case I32_ID:
            retval = static_cast<uint64_t>(std::get<int32_t>(m_sv_data.value()));
            ok = true;
            break;
         case U32_ID:
            retval = static_cast<uint64_t>(std::get<uint32_t>(m_sv_data.value()));
            ok = true;
            break;
         case I64_ID:
            retval = static_cast<uint64_t>(std::get<uint32_t>(m_sv_data.value()));
            ok = true;
            break;
         case U64_ID:
            retval = std::get<uint64_t>(m_sv_data.value());
            ok = true;
            break;
         case STR_ID:
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
         }
      }
      return retval;
   }

   std::string Scalar::to_string() const
   {
      std::string retval;
      if (m_sv_data.has_value())
      {
         switch (m_sv_data.value().index())
         {
         case I32_ID:
            retval = std::to_string(std::get<int32_t>(m_sv_data.value()));
            break;
         case U32_ID:
            retval = std::to_string(std::get<uint32_t>(m_sv_data.value()));
            break;
         case I64_ID:
            retval = std::to_string(std::get<int64_t>(m_sv_data.value()));
            break;
         case U64_ID:
            retval = std::to_string(std::get<uint64_t>(m_sv_data.value()));
            break;
         case STR_ID:
            retval = std::get<std::string>(m_sv_data.value());
            break;
         }
      }
      return retval;
   }


   std::shared_ptr<Scalar> make_sv()
   {
      return std::make_shared<Scalar>();
   }

   std::shared_ptr<Scalar> make_sv_int32(int32_t value)
   {
      auto sv = std::make_shared<Scalar>();
      sv->set(value);
      return sv;
   }
   std::shared_ptr<Scalar> make_sv_uint32(uint32_t value)
   {
      auto sv = std::make_shared<Scalar>();
      sv->set(value);
      return sv;
   }

   std::shared_ptr<Scalar> make_sv_int64(int64_t value)
   {
      auto sv = std::make_shared<Scalar>();
      sv->set(value);
      return sv;
   }
   std::shared_ptr<Scalar> make_sv_uint64(uint64_t value)
   {
      auto sv = std::make_shared<Scalar>();
      sv->set(value);
      return sv;
   }

   std::shared_ptr<Scalar> make_sv_string(const std::string& value)
   {
      auto sv = std::make_shared<Scalar>();
      sv->set(value);
      return sv;
   }
   ScalarValue make_sv_string(const char* value)
   {
      auto sv = std::make_shared<Scalar>();
      sv->set(value);
      return sv;
   }
   ScalarValue make_sv_string(const char* begin, const char* end)
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

   std::shared_ptr<Hash> make_hv()
   {
      return std::make_shared<Hash>();
   }
}