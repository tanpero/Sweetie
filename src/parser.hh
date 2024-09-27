#ifndef _PARSER_HH_
#define _PARSER_HH_
#include "lexer.hh"
#include "ast.hh"


// 解析器类
class Parser {
private:
    std::vector<Token> tokens; // Token流
    size_t current;           // 当前Token的位置

    // 向前查找下一个Token
    Token lookahead() const;

    // 向前移动到下一个Token
    void advance();

    Token here() const;

    // 报告错误信息并退出
    void error(const String& message) const;

    // 解析函数

    /*
     * Regex → Expression
     * Expression → Term { "|" Term }
     * Term → { BeginAnchor } Factor { Factor } { EndAnchor }
     * Factor → Atom { Qualifier }
     * Atom → LiteralCharacter
     *      → CharacterClass
     *      → AnyCharacter
     *      → "(?=" Expression ")" (正向先行断言)
     *      → "(?!" Expression ")" (负向先行断言)
     *      → "(?<=" Expression ")" (正向后行断言)
     *      → "(?<!" Expression ")" (负向后行断言)
     *      → "\b" (单词边界)
     *      → Group
     *      → NamedCapturingGroup
     *      → NonCapturingGroup
     *      → Backreference
     *      → UnicodeProperty
     * Quantifier → "?" | "*" | "+" | "{"[Number][","[Number]] "}"
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

    std::unique_ptr<AST> parseLiteral();
    std::unique_ptr<AST> parseCharacterClass();
    std::unique_ptr<AST> parseAnyCharacter();
    std::unique_ptr<AST> parseLookaheadAssertion();
    std::unique_ptr<AST> parseNegativeLookaheadAssertion();
    std::unique_ptr<AST> parseLookbehindAssertion();
    std::unique_ptr<AST> parseNegativeLookbehindAssertion();
    std::unique_ptr<AST> parseWordBound();
    std::unique_ptr<AST> parseGroup();
    std::unique_ptr<AST> parseNamedCapturingGroup();
    std::unique_ptr<AST> parseNonapturingGroup();
    std::unique_ptr<AST> parseBackreference();
    std::unique_ptr<AST> parseUnicodeProperty();

    std::unique_ptr<AST> parseQuantifier();

public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

    // 开始解析并生成AST
    std::unique_ptr<AST> parse();
};


#endif // !_PARSER_HH_

