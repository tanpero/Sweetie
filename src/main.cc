#include "parser.hh"


void testLexer(const String& input) {
    Lexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();
    for (const Token& token : tokens) {
        std::cout << token.toString();
    }
    std::cout << std::endl;
}

int main() {
    // 测试普通字符
    testLexer(R"(.hello world.)");

    // 测试特殊字符
    testLexer(R"(*+?(){}|^$)");

    // 测试转义序列
    testLexer(R"(\d\w\s\t\n\r\\\)");

    // 测试Unicode转义
    testLexer(R"(\u{1F600}\u0001F600)");

    // 测试量词
    testLexer(R"(a{1,5})");

    // 测试字符类
    testLexer(R"([abc])");
    testLexer(R"([a-z])");
    testLexer(R"([^abc])");
    testLexer(R"([[][)");

    // 测试分组
    testLexer(R"((abc))");
    testLexer(R"((?<name>abc))");
    testLexer(R"((?P<name>abc))");
    testLexer(R"((?:abc))");

    // 测试选择
    testLexer(R"(a|b)");

    // 测试锚点
    testLexer(R"(^abc$)");

    // 测试边界条件
    testLexer(R"(())");
    testLexer(R"(^$*+?(){}|[\\])");

    // 测试组合
    testLexer(R"(^abc+\\d{1,3}[a-z](?<name>def)|ghi$)");

    return 0;
}
