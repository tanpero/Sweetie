#include "token.hh"
#include <string>
#include <cctype>
#include <iostream>
#include <vector>

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
            << ", it should be a " << exp << "." << std::endl;
        exit(-1);
    }
}


class Lexer {
public:
    Lexer(const String& input) : input(input), position(0) {}

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
                default:
                    break;
                }
            }
            else if (c == '{') {
                tokens.emplace_back(getQuantifierBraces());
            }
            else {
                position++;
            }
        }
        
        return tokens;
    }

private:
    String input;
    size_t position;


    bool isLiteralCharacter() {
        return input[position] != '\\' &&
               input[position] != '^' && input[position] != '$' &&
               input[position] != '*' && input[position] != '+' &&
               input[position] != '?' && input[position] != '|' &&
               input[position] != '[' && input[position] != ']' &&
               input[position] != '(' && input[position] != ')' &&
               input[position] != '{' && input[position] != '}';
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

    // check: \uABCD or \u{ABCD(EF)} ?
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
            expect(input[position], count >= 4 && count <= 6, "a hexadecimal digit or the length should <= 6", position);
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
        return { TokenType::LiteralCharacter, { { input[position]}, {}}};
    }

    Token getAnchorStart() {
        return { TokenType::AnchorStart, { { '^' }, {}}};
    }

    Token getAnchorEnd() {
        return { TokenType::AnchorEnd, { { '$' }, {}}};
    }

    Token getQuantifierStar() {
        return { TokenType::QuantifierStar, { { '*' }, {}}};
    }

    Token getQuantifierPlus() {
        return { TokenType::QuantifierPlus, { { '+' }, {}}};
    }

    Token getQuantifierQuestion() {
        return { TokenType::QuantifierQuestion, { { '?' }, {}}};
    }

    Token getQuantifierBraces() {
        // c = '{'
        position++;
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
        return { TokenType::BranchAlternation, { {}, {} } };
    }

    Token getCharacterClassOpen() {
        return { TokenType::CharacterClassOpen, { {}, {} } };
    }

    Token getCharacterClassClose() {
        return { TokenType::CharacterClassClose, { {}, {} } };
    }

    Token getCharacterClassRange() {
        return { TokenType::CharacterClassRange, { {}, {} } };
    }

    Token getCharacterClassLiteral() {
        return { TokenType::CharacterClassLiteral, { {}, {} } };
    }

    Token getGroupOpen() {
        return { TokenType::GroupOpen, { {}, {} } };
    }

    Token getGroupClose() {
        return { TokenType::GroupClose, { {}, {} } };
    }

    Token getNamedCapturingGroupOpen() {
        return { TokenType::NamedCapturingGroupOpen, { {}, {} } };
    }

    Token getNamedCapturingGroupName() {
        return { TokenType::NamedCapturingGroupName, { {}, {} } };
    }

    Token getNamedCapturingGroupClose() {
        return { TokenType::NamedCapturingGroupClose, { {}, {} } };
    }

    Token getNonCapturingGroupOpen() {
        return { TokenType::NonCapturingGroupOpen, { {}, {} } };
    }

    Token getNonCapturingGroupClose() {
        return { TokenType::NonCapturingGroupClose, { {}, {} } };
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
        return { TokenType::Backreference, { {}, {} } };
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
            // c = '}'
            position++;
        }
        else
        {
            codepoint = getHexInteger(true);
        }
        return { TokenType::UnicodeCodePoint, { { fromCodepoint(codepoint)}, { toHexString(codepoint) }}};
    }

    Token getUnicodeProperty(bool accept) {
        return { TokenType::UnicodeProperty, { {}, {} } };
    }
};
