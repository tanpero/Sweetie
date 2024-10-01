#ifndef _PARSER_HH_
#define _PARSER_HH_
#include "lexer.hh"
#include "ast.hh"


class Parser {
private:
    std::vector<Token> tokens;
    size_t current;

    // Is current token the last one?
    bool final() const;
    
    // Look the next token without moving position
    Token lookahead() const;

    // Move to the next token
    void advance();

    Token here() const;

    void error(const String& message) const;


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
    
    void parseExpressionPrime(std::unique_ptr<Expression>& expression);
    std::unique_ptr<AST> parseExpression();
    std::unique_ptr<AST> parseTerm();
    std::unique_ptr<AST> parseFactor();
    std::unique_ptr<AST> parseAtom();

    std::unique_ptr<AST> parseGroup();
    std::unique_ptr<AST> parseNamedCapturingGroup();
    std::unique_ptr<AST> parseNonCapturingGroup();
    std::unique_ptr<AST> parseUnicodeProperty();

    std::unique_ptr<AST> parseAssertion();

    std::unique_ptr<AST> convertSpecialSequenceToActualAST(SpecialSequenceType type);

public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

    std::unique_ptr<AST> parse();
};


#endif // !_PARSER_HH_

