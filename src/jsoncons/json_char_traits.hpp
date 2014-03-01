// Copyright 2013 Daniel Parker
// Distributed under the Boost license, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See https://sourceforge.net/projects/jsoncons/files/ for latest version
// See https://sourceforge.net/p/jsoncons/wiki/Home/ for documentation.

#ifndef JSONCONS_JSON_CHAR_TRAITS_HPP
#define JSONCONS_JSON_CHAR_TRAITS_HPP

#include <string>
#include <sstream>
#include <vector>
#include <istream>
#include <cstdlib>
#include <cwchar>
#include <cstdint> 

namespace jsoncons {

const uint16_t min_lead_surrogate = 0xD800;
const uint16_t max_lead_surrogate = 0xDBFF;
const uint16_t min_trail_surrogate = 0xDC00;
const uint16_t max_trail_surrogate = 0xDFFF;

template <typename Char,size_t Size>
struct json_char_traits
{
};

template <>
struct json_char_traits<char,1>
{
    static size_t cstring_len(const char* s)
    {
        return std::strlen(s);
    }

    static int cstring_cmp(const char* s1, const char* s2, size_t n)
    {
        return std::strncmp(s1,s2,n);
    }

    static const std::string null_literal() {return "null";};

    static const std::string true_literal() {return "true";};

    static uint32_t convert_char_to_codepoint(std::string::const_iterator& it, std::string::const_iterator end)
    {
        char c = *it;
        uint32_t u(c >= 0 ? c : 256 + c );
        uint32_t cp = u;
        if (u < 0x80)
        {
        }
        else if ((u >> 5) == 0x6)
        {
            c = *(++it);
            u = (c >= 0 ? c : 256 + c );
            cp = ((cp << 6) & 0x7ff) + (u & 0x3f);
        }
        else if ((u >> 4) == 0xe)
        {
            c = *(++it);
            u = (c >= 0 ? c : 256 + c );
            cp = ((cp << 12) & 0xffff) + ((static_cast<uint32_t>(0xff & u) << 6) & 0xfff);
            c = *(++it);
            u = (c >= 0 ? c : 256 + c );
            cp += (u) & 0x3f;
        }
        else if ((u >> 3) == 0x1e)
        {
            c = *(++it);
            u = (c >= 0 ? c : 256 + c );
            cp = ((cp << 18) & 0x1fffff) + ((static_cast<uint32_t>(0xff & u) << 12) & 0x3ffff);
            c = *(++it);
            u = (c >= 0 ? c : 256 + c );
            cp += (static_cast<uint32_t>(0xff & u) << 6) & 0xfff;
            c = *(++it);
            u = (c >= 0 ? c : 256 + c );
            cp += (u) & 0x3f;
        }
        else
        {
        }
        return cp;
    }

    static const std::string false_literal() {return "false";};

    static void append_codepoint_to_string(uint32_t cp, std::string& s)
    {
        if (cp <= 0x7f)
        {
            s.push_back(static_cast<char>(cp));
        }
        else if (cp <= 0x7FF)
        {
            s.push_back(static_cast<char>(0xC0 | (0x1f & (cp >> 6))));
            s.push_back(static_cast<char>(0x80 | (0x3f & cp)));
        }
        else if (cp <= 0xFFFF)
        {
            s.push_back(0xE0 | static_cast<char>((0xf & (cp >> 12))));
            s.push_back(0x80 | static_cast<char>((0x3f & (cp >> 6))));
            s.push_back(static_cast<char>(0x80 | (0x3f & cp)));
        }
        else if (cp <= 0x10FFFF)
        {
            s.push_back(static_cast<char>(0xF0 | (0x7 & (cp >> 18))));
            s.push_back(static_cast<char>(0x80 | (0x3f & (cp >> 12))));
            s.push_back(static_cast<char>(0x80 | (0x3f & (cp >> 6))));
            s.push_back(static_cast<char>(0x80 | (0x3f & cp)));
        }
    }

};

template <>
struct json_char_traits<wchar_t,2> // assume utf16
{
    static size_t cstring_len(const wchar_t* s)
    {
        return std::wcslen(s);
    }

    static int cstring_cmp(const wchar_t* s1, const wchar_t* s2, size_t n)
    {
        return std::wcsncmp(s1,s2,n);
    }

    static const std::wstring null_literal() {return L"null";};

    static const std::wstring true_literal() {return L"true";};

    static const std::wstring false_literal() {return L"false";};

    static void append_codepoint_to_string(uint32_t cp, std::wstring& s)
    {
        if (cp <= 0xFFFF)
        {
            s.push_back(static_cast<wchar_t>(cp));
        }
        else if (cp <= 0x10FFFF)
        {
            s.push_back(static_cast<wchar_t>((cp >> 10) + min_lead_surrogate - (0x10000 >> 10)));
            s.push_back(static_cast<wchar_t>((cp & 0x3ff) + min_trail_surrogate));
        }
    }

    static uint32_t convert_char_to_codepoint(std::wstring::const_iterator& it, std::wstring::const_iterator end)
    {
        uint32_t cp = (0xffff & *it);
        if ((cp >= min_lead_surrogate && cp <= max_lead_surrogate)) // surrogate pair
        {
            uint32_t trail_surrogate = 0xffff & *(++it);
            cp = (cp << 10) + trail_surrogate + 0x10000u - (min_lead_surrogate << 10) - min_trail_surrogate;
        }
        return cp;
    }
};

template <>
struct json_char_traits<wchar_t,4> // assume utf32
{
    static size_t cstring_len(const wchar_t* s)
    {
        return std::wcslen(s);
    }

    static int cstring_cmp(const wchar_t* s1, const wchar_t* s2, size_t n)
    {
        return std::wcsncmp(s1,s2,n);
    }

    static const std::wstring null_literal() {return L"null";};

    static const std::wstring true_literal() {return L"true";};

    static const std::wstring false_literal() {return L"false";};

    static void append_codepoint_to_string(uint32_t cp, std::wstring& s)
    {
        if (cp <= 0xFFFF)
        {
            s.push_back(static_cast<wchar_t>(cp));
        }
        else if (cp <= 0x10FFFF)
        {
            s.push_back(static_cast<wchar_t>(cp));
        }
    }

    static uint32_t convert_char_to_codepoint(std::wstring::const_iterator& it, std::wstring::const_iterator end)
    {
        uint32_t cp = static_cast<uint32_t>(*it);
        return cp;
    }
};

inline
bool is_control_character(uint32_t c)
{
    return c <= 0x1F || c == 0x7f;
}

}
#endif
