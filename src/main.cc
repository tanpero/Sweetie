#include "parser.hh"
#include "matcher.hh"
#include "../tests/benchmark.hpp"


int main() {

    // Test Case 1: 匹配以"hello"开头，后面跟着任意数量的字母或数字的字符串
    String regex1 = R"(^hello\w*)";

    // Test Case 2: 匹配包含至少一个空格的字符串
    String regex2 = R"((?<=\s).+?(?=\s))";

    // Test Case 3: 匹配电子邮件地址
    String regex3 = R"([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,})";

    // Test Case 4: 匹配由数字、字母和下划线组成的字符串，长度在8到16位之间
    String regex4 = R"((?![0-9]+)(?![a-zA-Z]+)(?!_+)[0-9A-Za-z_]{8,16})";

    // Test Case 5: 匹配HTML标签
    String regex5 = R"(<([a-zA-Z0-9]+)([^>]*)>)";

    // Test Case 6: 匹配IPv4地址
    String regex6 = R"((?:\d{1,3}\.){3}\d{1,3})";

    // Test Case 7: 匹配日期格式（YYYY-MM-DD）
    String regex7 = R"(\d{4}-\d{2}-\d{2})";

    // Test Case 8: 匹配十六进制颜色代码
    String regex8 = R"(#(?:[0-9a-fA-F]{3}){1,2})";

    BENCHMARK([&]() {
        Lexer lexer(regex1);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        parser.parse();
        }, 100);

    BENCHMARK([&]() {
        Lexer lexer(regex2);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        parser.parse();
        }, 100);


    BENCHMARK([&]() {
        Lexer lexer(regex3);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        parser.parse();
        }, 100);


    BENCHMARK([&]() {
        Lexer lexer(regex4);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        parser.parse();
        }, 100);

    BENCHMARK([&]() {
        Lexer lexer(regex5);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        parser.parse();
        }, 100);

    BENCHMARK([&]() {
        Lexer lexer(regex6);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        parser.parse();
        }, 100);

    BENCHMARK([&]() {
        Lexer lexer(regex7);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        parser.parse();
        }, 100);

    BENCHMARK([&]() {
        Lexer lexer(regex8);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        parser.parse();
        }, 100);
        
    return 0;
}
