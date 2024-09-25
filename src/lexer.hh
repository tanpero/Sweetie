#include "token.hh"
#include <string>
#include <cctype>
#include <iostream>
#include <vector>
#include <stack>

void expect(Char c, Char exp, int offset) {
    if (c != exp) {
        std::cerr << "Invalid Character: \"" << c << "\" at position " << offset
            << ", it should be \"" << exp << "\"." << std::endl;
        exit(-1);
    }
}

void expect(Char c, bool ok, String exp, int offset) {
    if (!ok) {
        std::cerr << "Invalid Character: \"" << c << "\" at position " << offset
            << ", it should be " << exp << "." << std::endl;
        exit(-1);
    }
}


class Lexer {

    String input;
    size_t position;
    bool inCharacterClass;
    std::stack<std::pair<size_t, TokenType>> groupStack;

public:
    Lexer(const String& input) : input(input), position(0),
        inCharacterClass(false) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;

        while (position < input.length()) {
            Char c = input[position];
            if (isLiteralCharacter()) {
                tokens.emplace_back(getLiteralCharacter());
                position++;
            }
            else if (c == '\\') {
                position++;
                c = input[position];
                switch (c.toStdChar())
                {
                case '\\': case '+': case '?': case '*':
                case '{': case '}': case '(': case ')':
                case '[': case ']': case '|': case '^': case '$': {
                    Token t = { TokenType::LiteralCharacter, { { c }, {} } };
                    tokens.emplace_back(t);
                    position++;
                    break;
                }
                case 'n': case 't': case 'r': case 'f': case 'v': {
                    Token t = { TokenType::EscapeSequence, {{ String("\\") + c }, {} } };
                    tokens.emplace_back(t);
                    position++;
                    break;
                }

                        // Represents the control character with value equal to the letter's character value modulo 32.
                        // 
                        // For example, \cJ represents line break (\n),
                        // because the code point of J is 74, and 74 modulo 32 is 10,
                        // which is the code point of line break.
                        // 
                        // Because an uppercase letter and its lowercase form differ by 32,
                        // \cJ and \cj are equivalent.
                case 'c': {
                    position++;
                    expect(input[position], input[position].isStdAlpha(), "a alpha character", position);
                    Token t = { TokenType::LiteralCharacter, { { Char{ input[position].toStdChar() % 32 } }, {} } };
                    tokens.emplace_back(t);
                    break;
                }
                case 'u':
                    position++;
                    tokens.emplace_back(getUnicodeCodePoint());
                    break;
                case 'p':
                    tokens.emplace_back(getUnicodeProperty(true));
                    break;
                case 'P':
                    tokens.emplace_back(getUnicodeProperty(false));
                case 'k': {
                    position++;
                    tokens.emplace_back(getNamedBackreference());
                    break;
                }
                case '1': case '2': case '3': case '4': case '5':
                case '6': case '7': case '8': case '9':
                    tokens.emplace_back(getBackreference());
                    break;
                default: {
                    Token t = { TokenType::LiteralCharacter, { { c }, {} } };
                    tokens.emplace_back(t);
                    position++;
                    break;
                }
                }
            }
            else if (c == '.') {
                tokens.emplace_back(getAnyCharacter());
            }
            // "{}" = "?"
            // a{} = a?
            else if (c == '{') {
                if (input[position + 1] == '}')
                {
                    position++;
                    tokens.emplace_back(getQuantifierQuestion());
                }
                else
                {
                    tokens.emplace_back(getQuantifierBraces());
                }
            }
            else if (c == '^') {
                tokens.emplace_back(getAnchorStart());
            }
            else if (c == '$') {
                tokens.emplace_back(getAnchorEnd());
            }
            else if (c == '?') {
                tokens.emplace_back(getQuantifierQuestion());
            }
            else if (c == '*') {
                tokens.emplace_back(getQuantifierStar());
            }
            else if (c == '+') {
                tokens.emplace_back(getQuantifierPlus());
            }
            else if (c == '|') {
                tokens.emplace_back(getBranchAlternation());
            }

            // The "[" following "[" are all literal values,
            // but once encountered "]", exit the Character Class.
            // Then the following "]" are all literal values.
            else if (c == '[') {
                if (inCharacterClass)
                {
                    Token t = { TokenType::LiteralCharacter, { { "[" }, {} } };
                    tokens.emplace_back(t);
                    position++;
                }
                else
                {
                    tokens.emplace_back(getCharacterClassOpen());
                    c = input[position];
                    while (c != ']') {
                        tokens.emplace_back(getCharacterClassLiteral());
                        c = input[position];
                    }
                    tokens.emplace_back(getCharacterClassClose());
                    position++;
                }
            }
            else if (c == ']')
            {
                // Nude "]" will always be literal.
                Token t = { TokenType::LiteralCharacter, { { "]" }, {} } };
                tokens.emplace_back(t);
                position++;
            }
            else if (c == '(') {
                tokens.push_back(getGroupOpen());
            }
            else if (c == ')') {
                tokens.push_back(getGroupClose());
            }
            else {
                position++;
            }
        }

        expect(input[position], inCharacterClass == false, " \"]\" to close the character class", position);

        expect(input[position], groupStack.size() == 0, " \")\" to end the group", position);


        return tokens;
    }

private:


    bool isLiteralCharacter() {
        Char c = input[position];
        return c != '.' && c != '\\' &&
            c != '^' && c != '$' &&
            c != '*' && c != '+' &&
            c != '?' && c != '|' &&
            c != '[' && c != ']' &&
            c != '(' && c != ')' &&
            c != '{' && c != '}';
    }

    int getInteger() {
        size_t start = position;

        while (input[position].isStdDigit() && position < input.length()) {
            position++;
        }

        size_t length = position - start;

        if (length == 0) {
            expect(input[position], '}', position);
        }

        std::string intStr = input.substr(start, length).toUTF8();

        int value = std::stoi(intStr);
        return value;
    }

    // check: \uABCD or \u{A(BCDEF)} ?
    int getHexInteger(bool check) {
        size_t start = position;
        int count = 0;
        while (isHexDigit(input[position]) && position < input.length() && (check && (count < 4) || !check)) {
            position++;
            count++;
        }
        if (check) // should be \uABCD
        {
            expect(input[position], count == 4, "a hexadecimal digit", position);
        }
        else       // should be \{ABCD{EF}
        {
            expect(input[position], count && count <= 6, "a hexadecimal digit with length <= 6", position);
        }

        size_t length = position - start;

        if (length == 0) {
            expect(input[position], false, "a hexadecimal digit", position);
        }

        std::string hexStr = input.substr(start, length).toUTF8();

        int value = std::stoi(hexStr, nullptr, 16);
        return value;
    }

    bool isHexDigit(const Char& ch) {
        char c = ch.toStdChar();
        return (c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'f') ||
            (c >= 'A' && c <= 'F');
    }


    Token getLiteralCharacter() {
        return { TokenType::LiteralCharacter, { { input[position] }, {}} };
    }

    Token getAnyCharacter() {
        position++;
        return { TokenType::AnyCharacter, { { "." }, {} } };
    }

    Token getAnchorStart() {
        position++;
        return { TokenType::AnchorStart, { { "^" }, {}} };
    }

    Token getAnchorEnd() {
        position++;
        return { TokenType::AnchorEnd, { { "$" }, {}} };
    }

    Token getQuantifierStar() {
        position++;
        return { TokenType::QuantifierStar, { { "*" }, {}} };
    }

    Token getQuantifierPlus() {
        position++;
        return { TokenType::QuantifierPlus, { { "+" }, {}} };
    }

    Token getQuantifierQuestion() {
        position++;
        return { TokenType::QuantifierQuestion, { { "?" }, {}} };
    }

    Token getQuantifierBraces() {
        // c = '{'
        position++;
        expect(input[position], input[position] != '}', "NOT a }", position);
        // c = m or c = ','(m is 0)
        int m = (input[position] == ',') ? 0 : getInteger();
        // c = ','
        position++;

        int n = (input[position] == '}') ? std::string::npos : getInteger();
        // c = '}'
        position++;
        return { TokenType::QuantifierBraces, { { m }, { n } } };
    }

    Token getBranchAlternation() {
        position++;
        return { TokenType::BranchAlternation, { { "|" }, {}} };
    }

    Token getCharacterClassOpen() {
        position++;
        inCharacterClass = true;
        return { TokenType::CharacterClassOpen, { { "[" }, {} } };
    }

    Token getCharacterClassClose() {
        position++;
        inCharacterClass = false;
        return { TokenType::CharacterClassClose, { { "]" }, {} } };
    }

    Token getCharacterClassRange() {

        // e.g. [a-z]
        // c = 'a'
        Char a = input[position];
        position++;
        // c = '-'
        position++;
        // c = 'z'
        Char b = input[position];
        position++;
        return { TokenType::CharacterClassRange, { { a }, { b } } };
    }

    Token getCharacterClassLiteral() {

        // e.g. [123a-z-]
        // c = '1' then push c
        // the next char should be looked.
        // next char = '-'?
        Char c = input[position];

        // if c = 'a' or 'z'
        position++;
        // c = '-'
        if (input[position] == "-")
        {
            // the next char = 'z' or ']'?
            if (input[position + 1] == "]") {
                // Now '-' is just a char
                return { TokenType::CharacterClassLiteral, { { "-" }, {}} };
            }
            else
            {
                // Now '-' means range
                position--;
                // c = 'a'
                return getCharacterClassRange();
            }
        }

        if (c == "\\")
        {
            position++;
            c = input[position];
            switch (c.toStdChar())
            {
            default:
                break;
            }
        }

        return { TokenType::CharacterClassLiteral, { { c }, {} } };
    }

    Token getGroupOpen() {

        // c = '('
        position++;
        TokenType type = TokenType::GroupOpen; // 默认为普通捕获组

        Char nextChar = input[position];
        if (nextChar == '?') {
            position++; // 跳过'?'
            nextChar = input[position];
            if (nextChar == ':') {
                type = TokenType::NonCapturingGroupOpen;
            }
            else if (nextChar.isStdAlpha()) {
                type = TokenType::NamedCapturingGroupOpen;
                position++; // 跳过字母，如 '<' 或 'P'
                String name;
                while (input[position].isStdAlnum() || input[position] == '_') {
                    name += input[position];
                    position++;
                }
                return { type, { { name }, {} } };
            }
        }
        else if (nextChar == ")")
        {
            return getGroupClose();
        }
        groupStack.push({ position, type });
        return { type, { { "(" }, {} } };
    }

    Token getGroupClose() {
        position++;
        if (!groupStack.empty()) {
            auto open = groupStack.top();
            size_t openPos = open.first;
            TokenType openType = open.second;
            groupStack.pop();
            return Token{ TokenType::GroupClose, { { ")" }, {} } };
        }
        return Token{ TokenType::GroupClose, { { ")" }, {} } };
    }

    Token getNonCapturingGroupOpen() {
        position += 2;
        return { TokenType::NonCapturingGroupOpen, { { "(?:" }, {} } };
    }

    Token getNamedCapturingGroupOpen() {
        position += 2;
        std::string name;
        while (input[position].isStdAlnum() || input[position] == '_') {
            name += input[position].toStdChar();
            position++;
        }
        position++;
        return { TokenType::NamedCapturingGroupName, { { name }, {} } };
    }


    Token getAssertionLookahead() {
        return { TokenType::AssertionLookahead, { {}, {} } };
    }

    Token getAssertionNegativeLookahead() {
        return { TokenType::AssertionNegativeLookahead, { {}, {} } };
    }

    Token getAssertionLookbehind() {
        return { TokenType::AssertionLookbehind, { {}, {} } };
    }

    Token getAssertionNegativeLookbehind() {
        return { TokenType::AssertionNegativeLookbehind, { {}, {} } };
    }

    Token getSpecialSequence() {
        return { TokenType::SpecialSequence, { {}, {} } };
    }

    Token getRecursiveMode() {
        return { TokenType::RecursiveMode, { {}, {} } };
    }

    Token getConditionalExpressionOpen() {
        return { TokenType::ConditionalExpressionOpen, { {}, {} } };
    }

    Token getConditionalExpressionClose() {
        return { TokenType::ConditionalExpressionClose, { {}, {} } };
    }

    Token getConditionalExpressionCondition() {
        return { TokenType::ConditionalExpressionCondition, { {}, {} } };
    }

    Token getConditionalExpressionBranch() {
        return { TokenType::ConditionalExpressionBranch, { {}, {} } };
    }

    Token getBalanceGroupOpen() {
        return { TokenType::BalanceGroupOpen, { {}, {} } };
    }

    Token getBalanceGroupName() {
        return { TokenType::BalanceGroupName, { {}, {} } };
    }

    Token getBalanceGroupClose() {
        return { TokenType::BalanceGroupClose, { {}, {} } };
    }

    Token getBalanceGroupBalancedOpen() {
        return { TokenType::BalanceGroupBalancedOpen, { {}, {} } };
    }

    Token getBalanceGroupBalancedClose() {
        return { TokenType::BalanceGroupBalancedClose, { {}, {} } };
    }

    Token getBackreference() {
        return { TokenType::Backreference, { { input[position]}, {}} };
    }

    Token getNamedBackreference() {
        expect(input[position], '<', "\"<\" to start a named backreference", position);
        position++;
        expect(input[position], isLiteralCharacter(), "a literal character to named a backreference", position);
        String name;
        while (isLiteralCharacter() && input[position] != '>')
        {
            name += input[position];
            position++;
        }
        expect(input[position], '>', "\">\" to end a named backreference", position);
        Token t = { TokenType::NamedBackreference, { name, {} } };
        position++;
        return t;
    }

    Token getModifier() {
        return { TokenType::Modifier, { {}, {} } };
    }

    Token getEscapeSequence() {
        return { TokenType::EscapeSequence, { {}, {} } };
    }

    Token getUnicodeCodePoint() {

        int codepoint;

        // c = '{' or hex digit
        if (input[position] == '{')
        {
            position++;
            codepoint = getHexInteger(false);
            expect(input[position], '}', position);
            expect(input[position - 1], codepoint <= 0x10FFFF, "under 0x10FFFF", position);
            // c = '}'
            position++;
        }
        else
        {
            codepoint = getHexInteger(true);
        }
        return { TokenType::UnicodeCodePoint, { { fromCodepoint(codepoint)}, { toHexString(codepoint) }} };
    }

    Token getUnicodeProperty(bool accept) {
        String propertyName;
        String propertyValue;

        position++;

        expect(input[position], "{", position);

        position++;
        while (position < input.length() && input[position] != '}') {
            Char c = input[position];
            if (c == '=') {
                position++;
                while (position < input.length() && input[position] != '}') {
                    propertyValue += input[position];
                    position++;
                }
                break;
            }
            else {
                propertyName += c;
                position++;
            }
        }

        expect(input[position], "}", position);

        position++;

        return { TokenType::UnicodeProperty, {propertyName, propertyValue} };
    }
};
