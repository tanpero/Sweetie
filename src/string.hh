#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstring>

class Char {
public:
    Char() : data(), size(0) {}
    Char(const std::string& utf8) : data(utf8), size(utf8.size()) {}
    Char(char c) : data(1, c), size(1) {}
    Char(const char* cstr) : data(cstr), size(cstr ? std::strlen(cstr) : 0) {}
    
    Char& operator=(const char* cstr) {
        if (cstr) {
            data = std::string(cstr);
            size = data.size();
        } else {
            data.clear();
            size = 0;
        }
        return *this;
    }
    
    Char& operator=(char c) {
        data = std::string(1, c);
        size = 1;
        return *this;
    }

    bool isASCII() const {
        return size == 1 && data[0] < 0x80;
    }

    bool isSingle() const {
        return size == 1;
    }

    std::string toUTF8() const {
        return data;
    }

    bool operator==(const Char& other) const {
        return data == other.data;
    }

    bool operator!=(const Char& other) const {
        return data != other.data;
    }

    friend std::ostream& operator<<(std::ostream& os, const Char& ch) {
        return os << ch.toUTF8();
    }

private:
    std::string data;
    size_t size;
};

class String {
public:
    String() : chars() {}
    String(std::vector<Char> vec) : chars(vec) {}
    String(const std::string& utf8) {
        for (size_t i = 0; i < utf8.size();) {
            size_t charLen = 1;
            if ((utf8[i] & 0x80) == 0) {
            } else if ((utf8[i] & 0xE0) == 0xC0) {
                charLen = 2;
            } else if ((utf8[i] & 0xF0) == 0xE0) {
                charLen = 3;
            } else if ((utf8[i] & 0xF8) == 0xF0) {
                charLen = 4;
            }
            if (i + charLen > utf8.size()) {
                throw std::runtime_error("Invalid UTF-8 encoding");
            }
            chars.push_back(Char(utf8.substr(i, charLen)));
            i += charLen;
        }
    }

    String(const char* cstr) : String(std::string(cstr)) {}

    // 通过 char* 构造
    String(char* cstr) : String(std::string(cstr)) {}

    // 通过 const char* 赋值
    String& operator=(const char* cstr) {
        *this = String(cstr);
        return *this;
    }

    // 通过 char* 赋值
    String& operator=(char* cstr) {
        *this = String(cstr);
        return *this;
    }


    class Iterator {
    public:
        Iterator(const String* str, size_t index = 0) : str_(str), index_(index) {}

        std::string operator*() const {
            if (index_ >= str_->chars.size()) {
                throw std::out_of_range("Iterator out of range");
            }
            return str_->chars[index_].toUTF8();
        }

        Iterator& operator++() {
            if (index_ < str_->chars.size()) {
                ++index_;
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const Iterator& other) const {
            return str_ == other.str_ && index_ == other.index_;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

    private:
        const String* str_;
        size_t index_;
    };

    Char front() const {
        if (chars.empty()) {
            return {};
        }
        return chars.front();
    }

    
    String substr(size_t pos, size_t len = std::string::npos) const {
        if (pos > chars.size()) {
            throw std::out_of_range("Starting position is beyond the end of the string");
        }
        size_t endPos = (len == std::string::npos) ? chars.size() : (pos + len);
        if (endPos > chars.size()) {
            endPos = chars.size();
        }

        std::vector<Char> resultChars;
        for (size_t i = pos; i < endPos; ++i) {
            resultChars.push_back(chars[i]);
        }

        return String(resultChars);
    }

    void erase(size_t pos, size_t len = std::string::npos) {
        if (pos >= chars.size()) {
            return;
        }
        size_t currentPos = 0;
        auto it = chars.begin();
        while (it != chars.end() && (len == std::string::npos || currentPos < pos + len)) {
            if (currentPos >= pos) {
                it = chars.erase(it);
            } else {
                ++it;
            }
            currentPos++;
        }
    }

    size_t size() const {
        return chars.size();
    }

    String operator[](size_t index) const {
        if (index >= chars.size()) {
            return "";
        }
        return chars[index].toUTF8();
    }

    std::string toUTF8() const {
        std::string result;
        for (const Char& ch : chars) {
            result += ch.toUTF8();
        }
        return result;
    }

    Iterator begin() const { return Iterator(this); }
    Iterator end() const { return Iterator(this, chars.size()); }

    bool operator==(const String& other) const {
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

    bool operator!=(const String& other) const {
        return !(*this == other);
    }


    friend std::ostream& operator<<(std::ostream& os, const String& str) {
        return os << str.toUTF8();
    }

private:
    std::vector<Char> chars;
};
