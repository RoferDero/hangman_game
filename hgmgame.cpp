#include <iostream>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

namespace {

    // Понижение регистра для UTF-8 строки (кириллица)
    std::string toLowerUtf8(const std::string& s) {
        std::string result;
        result.reserve(s.size());
        size_t i = 0;
        while (i < s.size()) {
            unsigned char c = static_cast<unsigned char>(s[i]);
            size_t len = 1;
            if ((c & 0x80) == 0)         len = 1;
            else if ((c & 0xE0) == 0xC0) len = 2;
            else if ((c & 0xF0) == 0xE0) len = 3;
            else if ((c & 0xF8) == 0xF0) len = 4;

            if (i + len > s.size()) len = s.size() - i;
            std::string ch = s.substr(i, len);
            i += len;

            if (len == 1) {
                result += static_cast<char>(std::tolower(static_cast<unsigned char>(ch[0])));
            }
            else if (len == 2) {
                unsigned char b0 = static_cast<unsigned char>(ch[0]);
                unsigned char b1 = static_cast<unsigned char>(ch[1]);
                if (b0 == 0xD0 && b1 >= 0x90 && b1 <= 0xAF) {  // А-Я
                    result += static_cast<char>(0xD0);
                    result += static_cast<char>(b1 + 0x20);
                }
                else if (b0 == 0xD0 && b1 == 0x81) {         // Ё
                    result += static_cast<char>(0xD1);
                    result += static_cast<char>(0x91);
                }
                else {
                    result += ch;
                }
            }
            else {
                result += ch;
            }
        }
        return result;
    }




}

// Пара «слово — подсказка (определение)»
struct WordEntry {
    std::string word;
    std::string hint;
};