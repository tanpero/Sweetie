#ifndef _PARSER_HH_
#define _PARSER_HH_
#include "lexer.hh"
#include "ast.hh"


// 解析器类
class RegexParser {
private:
    std::vector<Token> tokens; // Token流
    size_t current;           // 当前Token的位置

    // 向前查找下一个Token
    Token lookahead() const;

    // 向前移动到下一个Token
    void advance();

    // 报告错误信息并退出
    void error(const std::string& message) const;

    // 解析器函数
    std::unique_ptr<AST> parseExpression();
    std::unique_ptr<AST> parseTerm();
    std::unique_ptr<AST> parseFactor();
    std::unique_ptr<AST> parseAtom();
    std::unique_ptr<AST> parseAssertion();
    std::unique_ptr<AST> parseQuantifier();
    std::unique_ptr<AST> parseGroup();
    std::unique_ptr<AST> parseCharacterClass();
    std::unique_ptr<AST> parseUnicodeProperty();

public:
    RegexParser(const std::vector<Token>& tokens);

    // 开始解析并生成AST
    std::unique_ptr<AST> parse();
};


#endif // !_PARSER_HH_

