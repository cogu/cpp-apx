#pragma once

#include <cstdint>
#include <memory>
#include <variant>
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>


namespace dtl
{
   class Value;
   class Scalar;
   class Array;
   class Hash;

   enum class ValueType : uint8_t
   {
      NoneType,
      Scalar,
      Array,
      Hash
   };

   /* Value */

   class Value
   {
   public:
      Value(ValueType dv_type) : m_dv_type{ dv_type } {}
      virtual ~Value() {}
      dtl::ValueType dv_type() const { return m_dv_type; }
   protected:
      ValueType m_dv_type;
   };

   using DynamicValue = std::shared_ptr<dtl::Value>;

   DynamicValue make_dv();

   enum class ScalarType : uint8_t
   {
      None,
      Int32,
      UInt32,
      Int64,
      UInt64,
      Float,
      Double,
      Bool,
      String,
      Bytes,
      ByteArray,
      Ptr
   };

   /* Scalar */
   class Scalar : public dtl::Value
   {
   public:
      using ScalarData = std::optional<std::variant<
         int32_t,             //index 0
         uint32_t,            //index 1
         int64_t,             //index 2
         uint64_t,            //index 3
         std::string> >;      //index 4
      Scalar() : Value{ dtl::ValueType::Scalar } {}
      Scalar(int32_t value) : Value{ dtl::ValueType::Scalar } { set(value);  }
      Scalar(uint32_t value) : Value{ dtl::ValueType::Scalar } { set(value); }
      Scalar(int64_t value) : Value{ dtl::ValueType::Scalar } { set(value); }
      Scalar(uint64_t value) : Value{ dtl::ValueType::Scalar } { set(value); }
      Scalar(const std::string& value) : Value{ dtl::ValueType::Scalar } { set(value); }
      Scalar(const char* value) : Value{ dtl::ValueType::Scalar } { set(value); }
      Scalar(const char* begin, const char* end) : Value{ dtl::ValueType::Scalar } { set(begin, end); }

      ~Scalar() {}
      ScalarType sv_type();
      bool has_value() { return m_sv_data.has_value(); }
      void reset();
      void set(int32_t value);
      void set(uint32_t value);
      void set(int64_t value);
      void set(uint64_t value);
      void set(const std::string& value);
      void set(const char* value);
      void set(const char* begin, const char* end);
      int32_t to_i32(bool& ok) const;
      uint32_t to_u32(bool& ok) const;
      int64_t to_i64(bool& ok) const;
      uint64_t to_u64(bool& ok) const;
      std::string to_string() const;
   protected:
      ScalarData m_sv_data;
   };

   using ScalarValue = std::shared_ptr<Scalar>;

   ScalarValue make_sv();
   ScalarValue make_sv_int32(int32_t value = 0);
   ScalarValue make_sv_uint32(uint32_t value = 0u);
   ScalarValue make_sv_int64(int64_t value = 0LL);
   ScalarValue make_sv_uint64(uint64_t value = 0ULL);
   ScalarValue make_sv_string(const std::string& value);
   ScalarValue make_sv_string(const char* value);
   ScalarValue make_sv_string(const char* begin, const char* end);

   /* Array */
   class Array : public dtl::Value
   {
   public:
      Array(std::size_t initial_length = 0u);
      std::size_t length() const { return m_av_data.size(); }
      void push(std::shared_ptr<dtl::Value> dv) { m_av_data.push_back(dv); }
      std::shared_ptr<dtl::Value>& at(std::size_t pos) { return m_av_data.at(pos); }
      std::shared_ptr<dtl::Value>const & at(std::size_t pos) const { return m_av_data.at(pos); }
      bool is_empty() const { return m_av_data.empty(); }
   protected:
      std::vector<std::shared_ptr<dtl::Value>> m_av_data;
   };

   using ArrayValue = std::shared_ptr<dtl::Array>;
   ArrayValue make_av(std::size_t length = 0u);

   /* Hash*/

   class Hash : public dtl::Value
   {
   public:
      using value_type = std::pair<std::string, DynamicValue>;
      Hash();
      std::size_t length() { return m_hv_data.size(); }
      void insert(value_type& v);
      void insert(value_type&& v);
      void set(std::string const& key, DynamicValue value);
      std::shared_ptr<dtl::Value>& at(std::string const& key) { return m_hv_data.at(key); }
      std::shared_ptr<dtl::Value>& at(char const* key) { return m_hv_data.at(key); }
      dtl::Value const* get(std::string const& key) const { auto &tmp = m_hv_data.at(key); return tmp.get(); }
      dtl::Value const* get(char const* key) const { auto &tmp = m_hv_data.at(key); return tmp.get(); }
      bool is_empty() const { return m_hv_data.empty(); }
   protected:
      std::unordered_map<std::string, std::shared_ptr<dtl::Value>> m_hv_data;
   };

   using HashValue = std::shared_ptr<dtl::Hash>;
   HashValue make_hv();

}