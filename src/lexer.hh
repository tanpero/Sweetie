#include <string>
#include <cctype>
#include <iostream>
#include <vector>
#include <stack>
#include <optional>
#include "string.hh"
#include "token.hh"

class Lexer {

    String input;
    size_t position;
    std::vector<Token> tokens;
    bool inCharacterClass;
    std::stack<std::pair<size_t, TokenType>> groupStack;

public:
    Lexer(const String& input);

    std::vector<Token> tokenize();

private:


    bool isLiteralCharacter();

    int getInteger();

    // check: \uABCD or \u{A(BCDEF)} ?
    int getHexInteger(bool check, int x = 4);

    bool isHexDigit(const Char& ch);

    Token getLiteralCharacter();

    Token getAnyCharacter();

    Token getAnchorStart();

    Token getAnchorEnd();

    Token getQuantifierStar();

    Token getQuantifierPlus();

    Token getQuantifierQuestion();

    Token getQuantifierBraces();

    Token getBranchAlternation();

    Token getCharacterClassOpen();

    Token getCharacterClassClose();

    Token getCharacterClassRange();

    Token getCharacterClassLiteral();

    // 平衡组的语法
    // (?<name>expr)：创建一个命名捕获组，并将捕获的文本压入堆栈。
    // (?'name'expr)：同上，只是使用单引号。
    // (?<-name>expr)：从堆栈中弹出一个指定的命名捕获组。
    // (?'-name'expr)：同上，只是使用单引号。
    // (?(group)yes | no)：如果堆栈中存在名为group的捕获组，则匹配yes部分；否则匹配no部分。
    Token getGroupOpen();

    Token getGroupClose();

    Token getNonCapturingGroupOpen();

    Token getNamedCapturingGroupOpen(Char mark);


    Token getAssertionLookahead();

    Token getAssertionNegativeLookahead();

    Token getAssertionLookbehind();

    Token getAssertionNegativeLookbehind();

    Token getBackreference();

    Token getNamedBackreference();

    Token getModifier();

    void getCharacterClassContent();

    bool canGetOrdinaryEscapedSequence();

    Token getUnicodeCodePoint();

    Token getUnicodeProperty(bool accept);
};