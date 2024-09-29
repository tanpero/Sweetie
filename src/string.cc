#include "string.hh"

Char::Char() : data(), size(0) {}

Char::Char(const std::string& utf8) : data(utf8), size(utf8.size()) {}

Char::Char(char c) : data(1, c), size(1) {}

Char::Char(int c) {
    data = fromCodepoint(c).toUTF8();
    size = data.size();
}

Char::Char(const char* cstr) : data(cstr ? cstr : ""), size(cstr ? std::strlen(cstr) : 0) {}

Char& Char::operator=(const char* cstr) {
    if (cstr) {
        data = std::string(cstr);
        size = data.size();
    }
    else {
        data.clear();
        size = 0;
    }
    return *this;
}

Char& Char::operator=(char c) {
    data = std::string(1, c);
    size = 1;
    return *this;
}

bool Char::isASCII() const {
    return size == 1 && data[0] < 0x80;
}

char Char::toStdChar() const {
    return data[0];
}

bool Char::isStdDigit() const {
    return std::isdigit(data[0]);
}

bool Char::isStdAlpha() const {
    return std::isalpha(data[0]);
}

bool Char::isStdAlnum() const {
    return std::isalnum(data[0]);
}

bool Char::isSingle() const {
    return size == 1;
}

std::string Char::toUTF8() const {
    return data;
}

bool Char::operator==(const Char& other) const {
    return data == other.data;
}

bool Char::operator!=(const Char& other) const {
    return data != other.data;
}

bool Char::operator<(const Char& other) const
{
    std::string s1 = data;
    std::string s2 = other.data;
    if (s1.length() < s2.length()) {
        return true; // s1 小于 s2
    }
    else if (s1.length() > s2.length()) {
        return false; // s1 大于 s2
    }

    for (size_t i = 0; i < s1.length(); ++i) {
        if (s1[i] < s2[i]) {
            return true; // s1 小于 s2
        }
        else if (s1[i] > s2[i]) {
            return false; // s1 大于 s2
        }
    }
    return false;
}

int Char::toCodepoint() const {
    unsigned char bytes[4];
    std::memcpy(bytes, data.data(), data.size());
    int codepoint = 0;
    switch (data.size()) {
    case 1:
        codepoint = bytes[0];
        break;
    case 2:
        codepoint = (((bytes[0] & 0x1F) << 6) | (bytes[1] & 0x3F));
        break;
    case 3:
        codepoint = (((bytes[0] & 0x0F) << 12) | ((bytes[1] & 0x3F) << 6) | (bytes[2] & 0x3F));
        break;
    case 4:
        codepoint = (((bytes[0] & 0x07) << 18) | ((bytes[1] & 0x3F) << 12) | ((bytes[2] & 0x3F) << 6) | (bytes[3] & 0x3F));
        break;
    default:
        throw std::invalid_argument("Invalid UTF-8 encoding");
    }
    return codepoint;
}

std::ostream& operator<<(std::ostream& os, const Char& ch) {
    return os << ch.toUTF8();
}

String::String() : chars() {}

String::String(Char ch) : chars({ ch }) {}

String::String(std::vector<Char> vec) : chars(vec) {}

String::String(const std::string& utf8) {
    for (size_t i = 0; i < utf8.size();) {
        size_t charLen = 1;
        if ((utf8[i] & 0x80) == 0) {
        }
        else if ((utf8[i] & 0xE0) == 0xC0) {
            charLen = 2;
        }
        else if ((utf8[i] & 0xF0) == 0xE0) {
            charLen = 3;
        }
        else if ((utf8[i] & 0xF8) == 0xF0) {
            charLen = 4;
        }
        if (i + charLen > utf8.size()) {
            throw std::runtime_error("Invalid UTF-8 encoding");
        }
        chars.push_back(Char(utf8.substr(i, charLen)));
        i += charLen;
    }
}

String::String(int num) {
    std::string numStr = std::to_string(num);
    for (size_t i = 0; i < numStr.size();) {
        size_t charLen = 1;
        if ((numStr[i] & 0x80) == 0) {
        }
        else if ((numStr[i] & 0xE0) == 0xC0) {
            charLen = 2;
        }
        else if ((numStr[i] & 0xF0) == 0xE0) {
            charLen = 3;
        }
        else if ((numStr[i] & 0xF8) == 0xF0) {
            charLen = 4;
        }
        if (i + charLen > numStr.size()) {
            throw std::runtime_error("Invalid UTF-8 encoding");
        }
        chars.push_back(Char(numStr.substr(i, charLen)));
        i += charLen;
    }
}

String::String(const char* cstr) : String(std::string(cstr)) {}

String::String(char* cstr) : String(std::string(cstr)) {}

String& String::operator=(const char* cstr) {
    *this = String(cstr);
    return *this;
}

String& String::operator=(char* cstr) {
    *this = String(cstr);
    return *this;
}

String::Iterator::Iterator(const String* str, size_t index) : str_(str), index_(index) {}

std::string String::Iterator::operator*() const {
    if (index_ >= str_->chars.size()) {
        throw std::out_of_range("Iterator out of range");
    }
    return str_->chars[index_].toUTF8();
}

String::Iterator& String::Iterator::operator++() {
    if (index_ < str_->chars.size()) {
        ++index_;
    }
    return *this;
}

String::Iterator String::Iterator::operator++(int) {
    Iterator temp = *this;
    ++(*this);
    return temp;
}

bool String::Iterator::operator==(const Iterator& other) const {
    return str_ == other.str_ && index_ == other.index_;
}

bool String::Iterator::operator!=(const Iterator& other) const {
    return !(*this == other);
}

Char String::front() const {
    if (chars.empty()) {
        return {};
    }
    return chars.front();
}

String String::substr(size_t pos, size_t len) const {
    if (pos > chars.size()) {
        throw std::out_of_range("Starting position is beyond the end of the string");
    }
    size_t endPos = (len == std::string::npos) ? chars.size() : (pos + len);
    if (endPos > chars.size()) {
        endPos = chars.size();
    }std::vector<Char> resultChars;
    for (size_t i = pos; i < endPos; ++i) {
        resultChars.push_back(chars[i]);
    }

    return String(resultChars);
}

void String::erase(size_t pos, size_t len) {
    if (pos >= chars.size()) {
        return;
    }
    size_t currentPos = 0;
    auto it = chars.begin();
    while (it != chars.end() && (len == std::string::npos || currentPos < pos + len)) {
        if (currentPos >= pos) {
            it = chars.erase(it);
        }
        else {
            ++it;
        }
        currentPos++;
    }
}

bool String::starts_with(const String& other) const {
    if (chars.size() < other.chars.size()) {
        return false;
    }
    for (size_t i = 0; i < other.chars.size(); ++i) {
        if (chars[i] != other.chars[i]) {
            return false;
        }
    }
    return true;
}

void String::remove_prefix(size_t n) {
    if (n >= chars.size()) {
        chars.clear();
    }
    else {
        chars.erase(chars.begin(), chars.begin() + n);
    }
}

void String::push_back(const Char& ch) {
    chars.push_back(ch);
}

void String::clear() {
    chars.clear();
}

std::vector<String> String::split(const String input, Char delimiter) {
    std::vector<String> result;
    String current;
    for (Char ch : input) {
        if (ch == delimiter) {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        }
        else {
            current.push_back(ch);
        }
    }

    if (!current.empty()) {
        result.push_back(current);
    }

    return result;
}

bool String::empty() const {
    return chars.empty();
}

size_t String::length() const {
    return chars.size();
}

Char String::operator[](size_t index) const {
    if (index >= chars.size()) {
        return {};
    }
    return  { chars[index].toUTF8() };
}

std::string String::toUTF8() const {
    std::string result;
    for (const Char& ch : chars) {
        result += ch.toUTF8();
    }
    return result;
}

String::Iterator String::begin() const { return Iterator(this); }
String::Iterator String::end() const { return Iterator(this, chars.size()); }

bool String::operator==(const String& other) const {
    if (chars.size() != other.chars.size()) {
        return false;
    }
    for (size_t i = 0; i < chars.size(); ++i) {
        if (chars[i] != other.chars[i]) {
            return false;
        }
    }
    return true;
}

bool String::operator!=(const String& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const String& str) {
    return os << str.toUTF8();
}

String operator+(const String& lhs, const String& rhs) {
    std::vector<Char> newChars = lhs.chars;
    newChars.insert(newChars.end(), rhs.chars.begin(), rhs.chars.end());
    return String(newChars);
}

String& operator+=(String& lhs, const String& rhs) {
    lhs.chars.insert(lhs.chars.end(), rhs.chars.begin(), rhs.chars.end());
    return lhs;
}

Char fromCodepoint(int unicode) {
    std::string utf8;
    if (unicode < 0x80) {
        // 1-byte sequence
        utf8 += static_cast<char>(unicode);
    }
    else if (unicode < 0x800) {
        // 2-byte sequence
        utf8 += static_cast<char>(0xC0 | (unicode >> 6));
        utf8 += static_cast<char>(0x80 | (unicode & 0x3F));
    }
    else if (unicode < 0x10000) {
        // 3-byte sequence
        utf8 += static_cast<char>(0xE0 | (unicode >> 12));
        utf8 += static_cast<char>(0x80 | ((unicode >> 6) & 0x3F));
        utf8 += static_cast<char>(0x80 | (unicode & 0x3F));
    }
    else if (unicode <= 0x10FFFF) {
        // 4-byte sequence
        utf8 += static_cast<char>(0xF0 | (unicode >> 18));
        utf8 += static_cast<char>(0x80 | ((unicode >> 12) & 0x3F));
        utf8 += static_cast<char>(0x80 | ((unicode >> 6) & 0x3F));
        utf8 += static_cast<char>(0x80 | (unicode & 0x3F));
    }
    else {
        // Invalid Unicode code point
        throw std::invalid_argument("Invalid Unicode code point");
    }
    return { utf8 };
}

String toHexString(int number) {
    if (number == 0) {
        return String("0");
    }

    const char hexDigits[] = "0123456789ABCDEF";
    std::vector<Char> hexChars;

    bool isNegative = number < 0;
    if (isNegative) {
        number = -number;
    }
    while (number > 0) {
        int remainder = number % 16;
        hexChars.insert(hexChars.begin(), hexDigits[remainder]);
        number /= 16;
    }

    return String(hexChars);
}

int toInteger(String s) {
    return std::stoi(s.toUTF8());
}

