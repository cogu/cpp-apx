#pragma once

#include <cstdint>
#include <memory>
#include <variant>
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>
#include <initializer_list>


namespace dtl
{
   class Value;
   class Scalar;
   class Array;
   class Hash;

   using ByteArray = std::vector<std::uint8_t>;

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
      String,
      Bool,
      ByteArray
   };

   /* Scalar */
   class Scalar : public dtl::Value
   {
   public:
      using ScalarData = std::optional<std::variant<
         int32_t,             //storage id 0
         uint32_t,            //storage id 1
         int64_t,             //storage id 2
         uint64_t,            //storage id 3
         std::string,         //storage id 4
         bool,                //storage id 5
         ByteArray            //storage id 6
      > >;
      Scalar() : Value{ dtl::ValueType::Scalar } {}
      ~Scalar() {}
      ScalarType sv_type() const;
      bool has_value() { return m_sv_data.has_value(); }
      void reset();
      void set(int32_t value);
      void set(uint32_t value);
      void set(int64_t value);
      void set(uint64_t value);
      void set(const std::string& value);
      void set(const char* value);
      void set(const char* begin, const char* end);
      void set(bool value);
      void set(const ByteArray&value);
      int32_t to_i32(bool& ok) const;
      uint32_t to_u32(bool& ok) const;
      int64_t to_i64(bool& ok) const;
      uint64_t to_u64(bool& ok) const;
      std::string to_string(bool& ok) const;
      bool to_bool(bool& ok) const;
      ByteArray const& get_byte_array() const;
   protected:
      ScalarData m_sv_data;
   };

   using ScalarValue = std::shared_ptr<Scalar>;

   std::shared_ptr<Scalar> make_sv();
   template <typename T> ScalarValue make_sv(T value)
   {
      auto sv = std::make_shared<Scalar>();
      sv->set(value);
      return sv;
   }

   ScalarValue make_sv(const char* begin, const char* end);

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
      DynamicValue& at(std::string const& key) { return m_hv_data.at(key); }
      DynamicValue& at(char const* key) { return m_hv_data.at(key); }
      dtl::Value const* cget(std::string const& key) const;
      dtl::Value const* cget(char const* key) const;
      bool is_empty() const { return m_hv_data.empty(); }
      std::unordered_map<std::string, DynamicValue>::iterator begin() { return m_hv_data.begin(); }
      std::unordered_map<std::string, DynamicValue>::const_iterator cbegin() { return m_hv_data.cbegin(); }
      std::unordered_map<std::string, DynamicValue>::iterator end() { return m_hv_data.end(); }
      std::unordered_map<std::string, DynamicValue>::const_iterator cend() { return m_hv_data.end(); }

   protected:
      std::unordered_map<std::string, DynamicValue> m_hv_data;
   };

   using HashValue = std::shared_ptr<dtl::Hash>;
   HashValue make_hv();
   HashValue make_hv(std::initializer_list<std::pair<std::string, DynamicValue>> initializer);


   /* Helper functions */

   inline ScalarValue sv_cast(DynamicValue& dv)
   {
      return std::dynamic_pointer_cast<Scalar>(dv);
   }

   inline DynamicValue dv_cast(ScalarValue& sv)
   {
      return std::dynamic_pointer_cast<Value>(sv);
   }

   inline DynamicValue dv_cast(ArrayValue& av)
   {
      return std::dynamic_pointer_cast<Value>(av);
   }

   inline DynamicValue dv_cast(HashValue& hv)
   {
      return std::dynamic_pointer_cast<Value>(hv);
   }

}