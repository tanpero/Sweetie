#ifndef _PARSER_HH_
#define _PARSER_HH_
#include "lexer.hh"
#include "ast.hh"


// 解析器类
class Parser {
private:
    std::vector<Token> tokens; // Token流
    size_t current;            // 当前Token的位置

    // 确认当前 token 是否是最后一个
    bool final() const;

    // 确认是否已经结束所有 token
    bool end() const;

    // 向前查找下一个Token
    Token lookahead() const;

    // 向前移动到下一个Token
    void advance();

    void advanceWhenNonFinal();

    Token here() const;

    // 报告错误信息并退出
    void error(const String& message) const;

    // 解析函数

    /*
     * Regex → Expression
     * Expression → Term { "|" Term }
     * Term → { BeginAnchor } Factor { Factor } { EndAnchor }
     * Factor → Atom { Qualifier } | Assertion
     * Atom → LiteralCharacter
     *      → CharacterClass
     *      → AnyCharacter
     *      → Group
     *      → NamedCapturingGroup
     *      → NonCapturingGroup
     *      → Backreference
     *      → UnicodeProperty
     *      → UnicodeCodepoint
     * Quantifier → "?" | "*" | "+" | "{"[Number][","[Number]] "}"
     * Assertion → "(?=" Expression ")" (正向先行断言)
     *           → "(?!" Expression ")" (负向先行断言)
     *           → "(?<=" Expression ")" (正向后行断言)
     *           → "(?<!" Expression ")" (负向后行断言)
     *           → "\b" (单词边界)
     * Group → "(" Expression ")"
     * NamedCapturingGroup → "(?P<" Name ">" Expression ")"
     * NonCapturingGroup → "(?:" Expression ")"
     * Backreference → "\" Number
     *               → "\" Name
     */

    std::unique_ptr<AST> parseExpression();
    std::unique_ptr<AST> parseTerm();
    std::unique_ptr<AST> parseFactor();
    std::unique_ptr<AST> parseAtom();

    std::unique_ptr<AST> parseGroup();
    std::unique_ptr<AST> parseNamedCapturingGroup();
    std::unique_ptr<AST> parseNonCapturingGroup();
    std::unique_ptr<AST> parseBackreference();
    std::unique_ptr<AST> parseUnicodeProperty();

    std::unique_ptr<AST> parseAssertion();

    std::unique_ptr<AST> convertSpecialSequenceToActualAST(SpecialSequenceType type);

public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

    // 开始解析并生成AST
    std::unique_ptr<AST> parse();
};


#endif // !_PARSER_HH_

