#pragma once

#include <istream>
#include <string>
#include <regex>
#include "cpp-apx/error.h"
#include "cpp-apx/types.h"
#include "cpp-apx/attribute_parser.h"
#include "cpp-apx/signature_parser.h"
#include "cpp-apx/data_type.h"
#include "cpp-apx/port.h"
#include "cpp-apx/node.h"

namespace apx
{
   enum class FileSection {Version, Node, Type, Port};

   using char_range_t = std::pair<const char*, const char*>;
   class Parser
   {
   public:
      struct ParseState
      {
         FileSection accept_next = FileSection::Version;  //What statements do we currently accept while parsing?
         int major_version = 0;
         int minor_version = 0;
         int lineno = 0;
         std::unique_ptr<apx::Node> node;
         std::unique_ptr<apx::DataElement> data_element;
         std::unique_ptr<apx::TypeAttributes> type_attributes;
         std::unique_ptr<apx::PortAttributes> port_attributes;
      };

      Parser();
      bool parse(std::basic_istream<char> &is);
      apx::Node* take_last_node() { return m_state.node.release(); }
      apx::error_t get_last_error() const { return m_last_error; }
      const std::string& get_parse_error_str() const { return m_parse_error_guide; }
      int get_line_number() const { return m_state.lineno; }


   protected:
      void reset();
      bool parse_version_line(std::string const& line);
      bool accept_apx_version(int majorVersion, int minorVersion);
      bool accept_node_declaration(std::string const& line);
      bool accept_type_or_port_declaration(std::string const& line);
      bool accept_type_declaration(const std::sub_match<const char*> *name_bstr, const std::sub_match<const char*> *dsg_bstr, const std::sub_match<const char*> *attr_bstr);
      bool accept_port_declaration(const std::string& line);
      bool accept_port_declaration(apx::PortType port_type, const std::sub_match<const char*> *name_bstr, const std::sub_match<const char*>* dsg_bstr, const std::sub_match<const char*>* attr_bstr);
      bool parse_data_signature(char const* begin, char const* end);
      bool parse_type_attributes(const char* begin, const char* end);
      bool parse_port_attributes(const char* begin, const char* end);
      void set_error(apx::error_t error_code) { m_last_error = error_code; m_parse_error_guide.clear(); }
      void set_error(apx::error_t error_code, const char* begin, const char* end) { m_last_error = error_code; m_parse_error_guide.assign(begin, end); }
      struct
      {
         std::regex const type_declaration_line;
         std::regex const port_declaration_line;
      } m_regex;
      ParseState m_state;
      AttributeParser m_attribute_parser;
      SignatureParser m_signature_parser;
      apx::error_t m_last_error;
      std::string m_parse_error_guide;
   };
}