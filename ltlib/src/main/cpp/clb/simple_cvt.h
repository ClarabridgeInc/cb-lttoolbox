#pragma once

#include <string>
#include <iostream>
#include <iomanip>

namespace simple_cvt
{

template <typename el_t>
inline void
dump_hex(const char *pref, const el_t *pStr, size_t len)
{
    std::cout << pref;
    size_t el_size = sizeof(el_t);
    std::cout << std::hex << std::setfill('0');  // setfill needs to be set only once
    unsigned char *ptr = reinterpret_cast<unsigned char *>(const_cast<el_t *>(pStr));
    for (size_t i = 0; i < len * el_size; ++i, ptr++)
    {
        std::cout << std::setw(2) << static_cast<unsigned>(*ptr) << " ";
    }
    std::cout << std::endl << std::endl;
}

inline bool
utf8_to_utf16(const std::string &text, std::wstring &result)
{
#define cur_ch static_cast<unsigned char>(*itr)
    for (std::string::const_iterator itr = text.begin(); itr != text.end(); ++itr)
    {
        int val = 0;
        if ((cur_ch & 0x80) == 0x0)
        {
            val = static_cast<wchar_t>(cur_ch);
        }
        else if ((cur_ch & 0xE0) == 0xC0)
        {
            val = (cur_ch & 0x1F) << 6;
            ++itr;
            val += cur_ch & 0x7F;
        }
        else if ((cur_ch & 0xF0) == 0xE0)
        {
            val = (cur_ch & 0x0F) << 6;
            ++itr;
            val += cur_ch & 0x7F;
            val = val << 6;
            ++itr;
            val += cur_ch & 0x7F;
        }
        else if ((cur_ch & 0xF8) == 0xF0)
        {
            val = (cur_ch & 0x07) << 6;
            ++itr;
            val += cur_ch & 0x7F;
            val = val << 6;
            ++itr;
            val += cur_ch & 0x7F;
            val = val << 6;
            ++itr;
            val += cur_ch & 0x7F;
        }
        else
        {
            return false;
        }
        result += static_cast<wchar_t>(val);
    }
#undef cur_ch
    return true;
}

inline bool
utf16_to_utf8(const std::wstring &text, std::string &result)
{
    for (std::wstring::const_iterator itr = text.begin(); itr != text.end(); ++itr)
    {
        unsigned int code = static_cast<unsigned int>(*itr); // std::cout << "idx: " << i << " code: " << std::setw(8) << code << std::endl;

        if (code <= 0x7F) {
            result += static_cast<char>(code);
        }
        else if (code <= 0x7FF) {
            result += static_cast<char>(0xC0 | (code >> 6));            /* 110xxxxx */
            result += static_cast<char>(0x80 | (code & 0x3F));          /* 10xxxxxx */
        }
        else if (code <= 0xFFFF) {
            result += static_cast<char>(0xE0 | (code >> 12));           /* 1110xxxx */
            result += static_cast<char>(0x80 | ((code >> 6) & 0x3F));   /* 10xxxxxx */
            result += static_cast<char>(0x80 | (code & 0x3F));          /* 10xxxxxx */
        }
        else if (code <= 0x10FFFF) {
            result += static_cast<char>(0xF0 | (code >> 18));           /* 11110xxx */
            result += static_cast<char>(0x80 | ((code >> 12) & 0x3F));  /* 10xxxxxx */
            result += static_cast<char>(0x80 | ((code >> 6) & 0x3F));   /* 10xxxxxx */
            result += static_cast<char>(0x80 | (code & 0x3F));          /* 10xxxxxx */
        } else {
            return false;
        }
    }
    return true;
}

} // simple_cvt
