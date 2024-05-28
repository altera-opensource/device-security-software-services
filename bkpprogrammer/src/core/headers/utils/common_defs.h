/*
 * This project is licensed as below.
 *
 * **************************************************************************
 *
 * Copyright 2020-2023 Intel Corporation. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * **************************************************************************
 */

#ifndef PROGRAMMER_COMMON_DEFS_H
#define PROGRAMMER_COMMON_DEFS_H

#ifndef NULL
#define NULL    0
#endif

#ifdef _WIN32
#include <windows.h>

#define int8_t      char
#define int16_t     short
#define int32_t     int
#define int64_t     __int64

#define uint8_t     unsigned char
#define uint16_t    unsigned short
#define uint32_t    unsigned int
#define uint64_t    unsigned __int64

#define _t(str) L##str
#define to_string_t(str) std::to_wstring(str)
#define _narrow(str) Narrow(str)
#define _widen(str) Widen(str)
typedef std::wstring string_t;
typedef wchar_t char_t;
typedef std::wstringstream stringstream_t;
#define cout_t std::wcout
#else
#include <linux/types.h>
#include <cwchar>
#include <cstdint>

#define _t(str) str
#define to_string_t(str) std::to_string(str)
#define _narrow(str) str
#define _widen(str) str
typedef std::string string_t;
typedef char char_t;
typedef std::stringstream stringstream_t;
#define cout_t std::cout
#endif //_WIN32

#ifdef _WIN32
/**
* Methods converts string format from wstring to string.
*
* @param wstr string in wide-string format - subject of conversion
*
* @return converted string
*/
inline std::string Narrow(const std::wstring& wstr) {
    std::locale loc = std::locale("");
    const auto &ctp = std::use_facet<std::ctype<wchar_t> >(loc);
    char *buffer = new char[wstr.length() + 1];
    size_t i = 0;
    for (; i < wstr.length(); i++) {
        buffer[i] = ctp.narrow(wstr[i], '\0');
    }
    buffer[i] = 0;
    std::string ret(buffer);
    delete[] buffer;

    return ret;
}

/**
* Methods converts string format from string to wstring.
*
* @param str string in narrow (regular) string format - subject of conversion
*
* @return converted wstring
*/
inline std::wstring Widen(const std::string& str) {
    wchar_t* buffer = new wchar_t[str.length() + 1];
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), buffer, (int)str.length() + 1);

    std::wstring ret(buffer, len);
    delete[] buffer;

    return ret;
}
#endif //_WIN32

#endif //PROGRAMMER_COMMON_DEFS_H
