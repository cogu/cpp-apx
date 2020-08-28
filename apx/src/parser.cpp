#include "cpp-apx/parser.h"
#include <iostream>
#include <cassert>
#include <cstdlib>


namespace apx
{
   Parser::Parser() :m_regex{ std::regex{ R"/(T"(\w+)"([^:]+)|T"(\w+)"(.+?):(.+))/" }, // type_declaration_line
      std::regex { R"/(([RP])"(\w+)"([^:]+)|([RP])"(\w+)"(.+?):(.+))/" } }            // port_declaration_line
   {

   }
   bool Parser::parse(std::basic_istream<char> &is)
   {
      reset();
      for (std::string line; std::getline(is, line); )
      {
         ++m_state.lineno;
         bool result = false;
         switch (m_state.accept_next)
         {
         case FileSection::Version:
            result = parse_version_line(line);
            break;
         case FileSection::Node:
            result = accept_node_declaration(line);
            break;
         case FileSection::Type:
            //As type declaration section is optional we also accept ports declarations
            result = accept_type_or_port_declaration(line);
            break;
         case FileSection::Port:
            result = accept_port_declaration(line);
         }
         if (!result)
         {
            return false;
         }
      }
      return (m_state.lineno > 0);
   }

   void Parser::reset()
   {
      m_state.accept_next = FileSection::Version;
      m_state.lineno = 0;
      m_state.major_version = 0;
      m_state.minor_version = 0;
      m_state.data_element.reset();
      m_state.type_attributes.reset();
      m_state.port_attributes.reset();
      m_state.node.reset();
   }

   bool Parser::parse_version_line(std::string const& line)
   {
      std::regex const version_statement{ R"(APX/(\d+)\.(\d+))" };
      std::cmatch m;
      bool result = std::regex_match(line.c_str(), m, version_statement);
      if (result)
      {
         m_state.major_version = std::stoi(m[1]);
         m_state.minor_version = std::stoi(m[2]);
         result = accept_apx_version(m_state.major_version, m_state.minor_version);
         if (result)
         {
            m_state.accept_next = FileSection::Node;
         }
      }
      return result;
   }

   bool Parser::accept_apx_version(int majorVersion, int minorVersion)
   {
      if ( (majorVersion == 1) && ( (minorVersion > 1) && (minorVersion <= 3) ) )
      {
         return true;
      }
      return false;
   }
   bool Parser::accept_node_declaration(std::string const& line)
   {
      std::regex const node_statement{ R"~(N"(\w+)")~" };
      std::cmatch m;
      bool result = std::regex_match(line.c_str(), m, node_statement);
      if (result)
      {
         std::string node_name{ m[1] };
         m_state.accept_next = FileSection::Type;
         m_state.node = std::make_unique<apx::Node>(std::move(node_name));
      }
      return result;
   }
   bool Parser::accept_type_or_port_declaration(std::string const& line)
   {
      std::cmatch match;
      bool result = std::regex_match(line.c_str(), match, m_regex.type_declaration_line);
      if (result)
      {
         if (match[1].length() > 0)
         {
            result = accept_type_declaration(&match[1], &match[2], nullptr);
         }
         else
         {
            result = accept_type_declaration(&match[3], &match[4], &match[5]);
         }
      }
      else
      {
         result = accept_port_declaration(line);
         if (result)
         {
            m_state.accept_next = FileSection::Port;
         }
      }
      return result;
   }
   bool Parser::accept_type_declaration(const std::sub_match<const char*>* name_bstr, const std::sub_match<const char*>* dsg_bstr, const std::sub_match<const char*>* attr_bstr)
   {
      bool result = parse_data_signature(dsg_bstr->first, dsg_bstr->second);
      bool has_attr = (attr_bstr != nullptr);
      if (result && has_attr)
      {
         result = parse_type_attributes(attr_bstr->first, attr_bstr->second);
      }
      if (result)
      {
         std::string name{ name_bstr->first, name_bstr->second };
         auto data_type = new apx::DataType(std::move(name), m_state.lineno);
         data_type->dsg.element = std::move(m_state.data_element);
         if (has_attr)
         {
            data_type->attr = std::move(m_state.type_attributes);
         }
         apx::error_t rc = m_state.node->append(data_type);
         if (rc != APX_NO_ERROR)
         {
            set_error(rc);
            result = false;
            delete data_type;
         }
      }
      return result;
   }

   bool Parser::accept_port_declaration(const std::string& line)
   {
      std::cmatch match;
      bool result = std::regex_match(line.c_str(), match, m_regex.port_declaration_line);
      if (result)
      {
         if (match[1].length() > 0)
         {
            //Port without attribute string
            apx::PortType port_type = *match[1].first == 'R' ? apx::PortType::RequirePort : apx::PortType::ProvidePort;
            result = accept_port_declaration(port_type, &match[2], &match[3], nullptr);
         }
         else
         {
            //Port with attribute string
            apx::PortType port_type = *match[4].first == 'R' ? apx::PortType::RequirePort : apx::PortType::ProvidePort;
            result = accept_port_declaration(port_type, &match[5], &match[6], &match[7]);
         }
      }
      return result;
   }

   bool Parser::accept_port_declaration(apx::PortType port_type, const std::sub_match<const char*>* name_bstr, const std::sub_match<const char*>* dsg_bstr, const std::sub_match<const char*>* attr_bstr)
   {
      bool result = parse_data_signature(dsg_bstr->first, dsg_bstr->second);
      bool has_attr = (attr_bstr != nullptr);
      if (result && has_attr)
      {
         result = parse_port_attributes(attr_bstr->first, attr_bstr->second);
      }
      if (result)
      {
         std::string name{ name_bstr->first, name_bstr->second };
         auto port = new apx::Port(port_type, std::move(name), m_state.lineno);
         port->dsg.element = std::move(m_state.data_element);
         if (has_attr)
         {
            port->attr = std::move(m_state.port_attributes);
         }
         apx::error_t rc = m_state.node->append(port);
         if (rc != APX_NO_ERROR)
         {
            set_error(rc);
            result = false;
            delete port;
         }
      }
      return result;
   }

   bool apx::Parser::parse_data_signature(char const* begin, char const* end)
   {
      bool retval = false;
      const char* result = m_signature_parser.parse_data_signature(begin, end);
      if (result == end)
      {
         m_state.data_element.reset(m_signature_parser.take_data_element());
         retval = true;
      }
      else if (result > begin)
      {
         //Stray characters after attributes have been parsed
         set_error(APX_STRAY_CHARACTERS_AFTER_PARSE_ERROR, result, end);
      }
      else
      {
         //Parse failure
         const char* error_next = nullptr;
         auto rc = m_signature_parser.get_last_error(error_next);
         if (error_next != nullptr)
         {
            set_error(rc, begin, error_next);
         }
         else
         {
            set_error(rc);
         }
         return false;
      }
      return retval;
   }

   bool Parser::parse_type_attributes(const char* begin, const char* end)
   {
      bool retval = false;
      auto type_attributes = std::make_unique<apx::TypeAttributes>();
      const char* result = m_attribute_parser.parse_type_attributes(begin, end, *type_attributes.get());
      if (result == end)
      {
         m_state.type_attributes = std::move(type_attributes);
         retval = true;
      }
      else if (result > begin)
      {
         //Stray characters after attributes have been parsed
         set_error(APX_STRAY_CHARACTERS_AFTER_PARSE_ERROR, result, end);
      }
      else
      {
         //Parse failure
         const char* error_next = nullptr;
         auto rc = m_attribute_parser.get_last_error(error_next);
         if (error_next != nullptr)
         {
            set_error(rc, begin, error_next);
         }
         else
         {
            set_error(rc);
         }
         return false;
      }
      return retval;
   }

   bool Parser::parse_port_attributes(const char* begin, const char* end)
   {
      bool retval = false;
      auto port_attributes = std::make_unique<apx::PortAttributes>();
      const char* result = m_attribute_parser.parse_port_attributes(begin, end, *port_attributes.get());
      if (result == end)
      {
         m_state.port_attributes = std::move(port_attributes);
         retval = true;
      }
      else if (result > begin)
      {
         //Stray characters after attributes have been parsed
         set_error(APX_STRAY_CHARACTERS_AFTER_PARSE_ERROR, result, end);
      }
      else
      {
         //Parse failure
         const char* error_next = nullptr;
         auto rc = m_attribute_parser.get_last_error(error_next);
         if (error_next != nullptr)
         {
            set_error(rc, begin, error_next);
         }
         else
         {
            set_error(rc);
         }
         return false;
      }
      return retval;
   }
}
