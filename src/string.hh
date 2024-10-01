#ifndef _STRING_HH_
#define _STRING_HH_

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

class String;

class Char {
public:
    Char();
    Char(const std::string& utf8);
    Char(char c);
    Char(int c);
    Char(const char* cstr);
    Char& operator=(const char* cstr);
    Char& operator=(char c);
    bool isASCII() const;
    char toStdChar() const;
    bool isStdDigit() const;
    bool isStdAlpha() const;
    bool isStdAlnum() const;
    bool isSingle() const;
    std::string toUTF8() const;
    bool operator==(const Char& other) const;
    bool operator!=(const Char& other) const;
    bool operator<(const Char& other) const;
    bool operator<=(const Char& other) const;
    bool operator>(const Char& other) const;
    bool operator>=(const Char& other) const;
    Char operator-(int n) const;
    Char operator+(int n) const;
    int toCodepoint() const;
    friend std::ostream& operator<<(std::ostream& os, const Char& ch);

private:
    std::string data;
    size_t size;
};

class String {
public:
    String();
    String(Char ch);
    String(std::vector<Char> vec);
    String(const std::string& utf8);
    String(int num);
    String(const char* cstr);
    String(char* cstr);
    String& operator=(const char* cstr);
    String& operator=(char* cstr);

    class Iterator {
    public:
        Iterator(const String* str, size_t index = 0);
        std::string operator*() const;
        Iterator& operator++();
        Iterator operator++(int);
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;

    private:
        const String* str_;
        size_t index_;
    };

    Char front() const;
    String substr(size_t pos, size_t len = std::string::npos) const;
    void erase(size_t pos, size_t len = std::string::npos);
    bool starts_with(const String& other) const;
    void remove_prefix(size_t n);
    void push_back(const Char& ch);
    void clear();
    std::vector<String> split(const String input, Char delimiter);
    bool empty() const;
    size_t length() const;
    Char operator[](size_t index) const;
    std::string toUTF8() const;
    Iterator begin() const;
    Iterator end() const;
    bool operator==(const String& other) const;
    bool operator!=(const String& other) const;
    friend std::ostream& operator<<(std::ostream& os, const String& str);
    friend String operator+(const String& lhs, const String& rhs);
    friend String& operator+=(String& lhs, const String& rhs);

private:
    std::vector<Char> chars;
};

String operator+(const String& lhs, const String& rhs);
String& operator+=(String& lhs, const String& rhs);

Char fromCodepoint(int unicode);
String toHexString(int number);
int toInteger(String s);

#endif // !_STRING_HH_
