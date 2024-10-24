#include "parser.hh"
#include "matcher.hh"
#include "../tests/benchmark.hpp"
#include <regex>
#include "unicode.hh"

std::unique_ptr<AST> a;

void test(String regex) {    
    std::cout << "\n----------------\n - Regex: " << regex << "\n";
    BENCHMARK([&]() {
        std::regex r(regex.toUTF8(), std::regex::ECMAScript);
        r;
    }, 10000); 
}

//int main() {
    //// Test Case 1: 匹配以"hello"开头，后面跟着任意数量的字母或数字的字符串
    //test(R"(^hello\w*)");

    //// Test Case 2: 匹配包含至少一个空格的字符串
    ////test(R"((?<=\s).+(?=\s))");

    //// Test Case 3: 匹配电子邮件地址
    //test(R"([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,})");

    //// Test Case 4: 匹配由数字、字母和下划线组成的字符串，长度在8到16位之间
    //test(R"((?![0-9]+)(?![a-zA-Z]+)(?!_+)[0-9A-Za-z_]{8,16})");

    //// Test Case 5: 匹配HTML标签
    //test(R"(<(\w+)(?:\s+([^=>]+)(?:\s*=\s*(?:"[^"]*"|'[^']*'|[^>\s]+))?)*\s*(\/?)>)");

    //// Test Case 6: 匹配IPv4地址
    //test(R"(^(?:(?:25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(?:25[0-5]|2[0-4]\d|[01]?\d\d?)$)");

    //// Test Case 7: 匹配ISO 8601日期时间格式
    //test(R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(?:\.\d+)?(?:Z|[+-]\d{2}:\d{2})?$)");

    //// Test Case 8: 匹配十六进制颜色代码
    //test(R"(#(?:[0-9a-fA-F]{3}){1,2})");

    //// Test Case 9: 匹配CSS选择器
    //test(R"(^(?:\w+|\*)(?:#[\w-]+)?(?:\.[\w-]+)*(?:\[.+?\])?(?:::?[\w-]+)?(?:\(.+?\))?(?:\s*[+>~]\s*(?:\w+|\*)(?:#[\w-]+)?(?:\.[\w-]+)*(?:\[.+?\])?(?:::?[\w-]+)?(?:\(.+?\))?)*$)");

    //// Test Case 10: 匹配 ISBN
    //test(R"((?:ISBN(?:-1[03])?:?\ )?(?=\d{10}$|(?=(?:\d+[-\s]){3})[-\s])(?=\d[-\s](\d{7}x|[\d-]{10}x|[\d-]{13})[-\s]?)\d{1,5}[-\s]?\d{1,7}[-\s]?(\d{1,6}[\dXx])?)");

    //// Test Case 11: 匹配信用卡号
    //test(R"(^(?:4[0-9]{12}(?:[0-9]{3})?|5[1-5][0-9]{14}|3[47][0-9]{13}|6(?:011|5[0-9]{2})[0-9]{12})$)");

    //// Test Cases 12: 匹配 URL
    //test(R"(^(?:(?:25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(?:25[0-5]|2[0-4]\d|[01]?\d\d?)$)");

    //return 0;
//}

int main() {
    // 示例：创建一个谓词来检测大写字母
    CharPredicate isL = forProperty("L", true);

    std::cout << "L is letter? " << (isL(U'L') ? "Yes" : "No");

    return 0;
}
