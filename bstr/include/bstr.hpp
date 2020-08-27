#pragma once

#include <type_traits>
#include <functional>
#include <string>
#include <cstring>

namespace bstr
{      

   /*************** predicate functions ***************/
   template <typename charT> bool pred_is_whitespace(charT c)
   {
      return (c == (charT)'\t') || (c == (charT)'\n') || (c == (charT)'\r') || (c == (charT)' ');
   }

   /*************** bstr functions ***************/
   template<typename charT>  const charT* while_predicate(const charT* begin, const charT* end, bool(*pred_func)(charT))
   {
      const charT* next = begin;
      while (next < end)
      {
         if (!pred_func(*next)) {
            break;
         }
         next++;
      }
      return next;
   }

   template<typename charT>  const charT* while_predicate_reverse(const charT* begin, const charT* end, bool(*pred_func)(charT))
   {
      if (begin < end)
      {
         const charT* next = end;
         while (next > begin)
         {
            const charT* tmp = next - 1;
            if (!pred_func(*tmp)) {
               break;
            }
            next--;
         }
         return next;
      }
      return begin;
   }

   template<typename charT>  const charT* lstrip(const charT* begin, const charT* end)
   {
      return bstr::while_predicate(begin, end, bstr::pred_is_whitespace<charT>);
   }

   template<typename charT>  const charT* rstrip(const charT* begin, const charT* end)
   {
      return bstr::while_predicate_reverse(begin, end, bstr::pred_is_whitespace<charT>);
   }

   template<typename charT>  const charT* strip(const charT* begin, const charT* end, const charT*& stripped_begin, const charT*& stripped_end)
   {
      stripped_begin = bstr::lstrip(begin, end);
      stripped_end = bstr::rstrip(stripped_begin, end);
   }

   template<typename charT>  const charT* match_pair(const charT* begin, const charT* end, charT left, charT right, charT escape_char)
   {
      const charT* next = begin;
      uint32_t level_count = 0u;
      if (next < end) 
      {
         if (*next == left) 
         {
            next++;
            if (escape_char != 0) 
            {
               bool is_escape = false;
               while (next < end) 
               {
                  charT c = *next;
                  if (is_escape) 
                  {
                     //ignore this char
                     next++;
                     is_escape = false;
                     continue;
                  }
                  else 
                  {
                     if (c == escape_char) 
                     {
                        is_escape = true;
                     }
                     else if (c == right) 
                     {
                        if (level_count == 0) 
                        {
                           return next;
                        }
                        else 
                        {
                           level_count--;
                        }
                     }
                     else if (c == left)
                     {
                        level_count++;
                     }
                  }
                  next++;
               }
            }
            else 
            {
               while (next < end)
               {
                  charT c = *next;
                  if (c == right) 
                  {
                     if (level_count == 0) 
                     {
                        return next;
                     }
                     else 
                     {
                        level_count--;
                     }
                  }
                  else if (c == left)
                  {
                     level_count++;
                  }
                  next++;
               }
            }
         }
         else
         {
            return nullptr; //string does not start with left character
         }
      }
      return begin;
   }

   template<typename charT> const charT* match_str(const charT* begin, const charT* end, const charT* str_begin, const charT* str_end)
   {
      const charT* next = begin;
      const charT* str_next = str_begin;
      if ( (begin > end) || (str_begin > str_end) || 
         (begin == nullptr) ||
         (end == nullptr) ||
         (str_begin == nullptr) ||
         (str_end == nullptr) )
      {         
         return nullptr;
      }
      while (next < end) {
         if (str_next < str_end)
         {
            if (*next != *str_next)
            {
               return nullptr;
            }
         }
         else
         {
            //All characters in str has been successfully matched
            assert(std::distance(begin, next) == std::distance(str_begin, str_end));
            return next;
         }
         next++;
         str_next++;
      }
      if (str_next == str_end)
      {
         assert(std::distance(begin, next) == std::distance(str_begin, str_end));
         return next; //All characters in str has been successfully matched
      }
      return begin; //Partial match, end of sream reached before str was fully matched
   }

   template<typename charT> const charT* match_str(const charT* begin, const charT* end, const charT* str)
   {
      size_t sz = std::strlen(static_cast<const char*>(str));
      return match_str(begin, end, str, str + sz);
   }

}