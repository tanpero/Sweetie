#include "lexer.hh"


static void expect(Char c, Char exp, size_t offset) {
    if (c != exp) {
        std::cerr << "Invalid Character: \"" << c << "\" at position " << offset
            << ", it should be \"" << exp << "\"." << std::endl;
        exit(-1);
    }
}

static void expect(Char c, bool ok, String exp, size_t offset) {
    if (!ok) {
        std::cerr << "Invalid Character: \"" << c << "\" at position " << offset
            << ", it should be " << exp << "." << std::endl;
        exit(-1);
    }
}

Lexer::Lexer(const String& input)
    : input(input), position(0),
    inCharacterClass(false) {
}

std::vector<Token> Lexer::tokenize()
{
    while (position < input.length()) {
        Char c = input[position];
        if (isLiteralCharacter()) {
            tokens.emplace_back(getLiteralCharacter());
            position++;
        }
        else if (c == '\\') {
            position++;
            if (canGetOrdinaryEscapedSequence())
            {
                continue;
            }
            c = input[position];
            switch (c.toStdChar())
            {
            case 'b': case 'B': {
                Token t = { TokenType::SpecialSequence, {{ String("\\") + c }, {} } };
                tokens.emplace_back(t);
                position++;
                break;
            }
            case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9':
                tokens.emplace_back(getBackreference());
                break;
            case 'k': {
                position++;
                tokens.emplace_back(getNamedBackreference());
                break;
            }
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
                if (input[position + 1] == '^')
                {
                    position++;
                    Token t1{ TokenType::CharacterClassNegative, { "^", {} } };
                    tokens.emplace_back(getCharacterClassOpen());
                    tokens.emplace_back(t1);
                }
                else
                {
                    tokens.emplace_back(getCharacterClassOpen());
                }

                // [\xab-\xef]
                // [a-z]
                // [abc]
                c = input[position];
                while (c != ']' && position < input.length()) {
                    getCharacterClassContent();
                    c = input[position];
                }
                expect(c, c == ']', "\"]\" to close the character class", position);
                tokens.emplace_back(getCharacterClassClose());
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
            tokens.emplace_back(getGroupOpen());
        }
        else if (c == ')') {
            tokens.emplace_back(getGroupClose());
        }
        else {
            position++;
        }
    }

    expect(input[position], inCharacterClass == false, "\"]\" to close the character class", position);

    expect(input[position], groupStack.size() == 0, "\")\" to end the group", position);


    return tokens;
}

bool Lexer::isLiteralCharacter()
{
    Char c = input[position];
    return c != '.' && c != '\\' &&
        c != '^' && c != '$' &&
        c != '*' && c != '+' &&
        c != '?' && c != '|' &&
        c != '[' && c != ']' &&
        c != '(' && c != ')' &&
        c != '{' && c != '}';
}

int Lexer::getInteger()
{
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

int Lexer::getHexInteger(bool check, int x)
{
    size_t start = position;
    int count = 0;
    while (isHexDigit(input[position]) && position < input.length() && (check && (count < x) || !check)) {
        position++;
        count++;
    }
    if (check && x == 4) // should be \uABCD
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

bool Lexer::isHexDigit(const Char& ch)
{
    char c = ch.toStdChar();
    return (c >= '0' && c <= '9') ||
        (c >= 'a' && c <= 'f') ||
        (c >= 'A' && c <= 'F');
}

Token Lexer::getLiteralCharacter()
{
    return { TokenType::LiteralCharacter, { { input[position] }, {}} };
}

Token Lexer::getAnyCharacter()
{
    position++;
    return { TokenType::AnyCharacter, { { "." }, {} } };
}

Token Lexer::getAnchorStart()
{
    position++;
    return { TokenType::AnchorStart, { { "^" }, {}} };
}

Token Lexer::getAnchorEnd()
{
    position++;
    return { TokenType::AnchorEnd, { { "$" }, {}} };
}

Token Lexer::getQuantifierStar()
{
    position++;
    return { TokenType::QuantifierStar, { { "*" }, {}} };
}

Token Lexer::getQuantifierPlus()
{
    position++;
    return { TokenType::QuantifierPlus, { { "+" }, {}} };
}

Token Lexer::getQuantifierQuestion()
{
    position++;
    return { TokenType::QuantifierQuestion, { { "?" }, {}} };
}

Token Lexer::getQuantifierBraces()
{
    // c = '{'
    position++;
    expect(input[position], input[position] != '}', "NOT a }", position);
    // c = m or c = ','(m is 0)
    int m = (input[position] == ',') ? 0 : getInteger();

    // "{3}"
    if (input[position] == '}')
    {
        return { TokenType::QuantifierBraces, { { m }, { m } } };
    }

    // c = ','
    position++;

    int n = (input[position] == '}') ? std::string::npos : getInteger();
    // c = '}'
    position++;
    return { TokenType::QuantifierBraces, { { m }, { n } } };
}

Token Lexer::getBranchAlternation()
{
    position++;
    return { TokenType::BranchAlternation, { { "|" }, {}} };
}

Token Lexer::getCharacterClassOpen()
{
    position++;
    inCharacterClass = true;
    return { TokenType::CharacterClassOpen, { { "[" }, {} } };
}

Token Lexer::getCharacterClassClose()
{
    position++;
    inCharacterClass = false;
    return { TokenType::CharacterClassClose, { { "]" }, {} } };
}

Token Lexer::getCharacterClassRange()
{

    // e.g. [a-z]
    // c = 'a'
    Char a = input[position];

    if (a == "\\")
    {
        position++;
        expect(input[position], canGetOrdinaryEscapedSequence(), "an correct escaped sequence", position);
        Token t1 = tokens.back();
    }

    position++;
    // c = '-'
    position++;
    // c = 'z'
    Char b = input[position];
    position++;
    return { TokenType::CharacterClassRange, { { a }, { b } } };
}

Token Lexer::getCharacterClassLiteral()
{

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
            Token t1{ TokenType::CharacterClassLiteral, { c, {} } };
            return t1;
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
        c = input[position];

        expect(c, canGetOrdinaryEscapedSequence(), "a valid escaped sequence", position);
    }

    return { TokenType::CharacterClassLiteral, { { c }, {} } };
}

Token Lexer::getGroupOpen()
{

    // c = '('
    position++;

    TokenType type = TokenType::GroupOpen; // 默认为普通捕获组
    Token t = { type, { "(", {} } };

    Char nextChar = input[position];

    if (nextChar == '?')
    {

        position++;
        nextChar = input[position];

        switch (nextChar.toStdChar())
        {
        case ':':
            position++;
            t = getNonCapturingGroupOpen();
            break;
        case '<': {
            // Named capturing group or negative lookahead assertion.
            Char nc = input[position + 1];
            if (nc == '=')
            {
                position += 2;
                t = getAssertionLookbehind();
            }
            else if (nc == '!')
            {
                position += 2;
                t = getAssertionNegativeLookbehind();
            }
            else
            {
                position++;
                t = getNamedCapturingGroupOpen("<");
            }
            break;
        }
        case '\'':
            position++;
            t = getNamedCapturingGroupOpen("'");
            break;
        case 'P':
            position++;
            nextChar = input[position];
            expect(nextChar, nextChar == '<',
                "\"<\" to start a named capturing group", position);
            position++;
            t = getNamedCapturingGroupOpen('<');
            break;
        case '=':
            position++;
            t = getAssertionLookahead();
            break;
        case '!':
            position++;
            t = getAssertionNegativeLookahead();
            break;
        case ')':
            position++;
            t = getGroupClose();
            break;
        default:

            // TODO...
            break;
        }

    }
    else
    {
        groupStack.push({ position, type });
    }

    return t;
}

Token Lexer::getGroupClose()
{
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

Token Lexer::getNonCapturingGroupOpen()
{
    return { TokenType::NonCapturingGroupOpen, { { "(?:" }, {} } };
}

Token Lexer::getNamedCapturingGroupOpen(Char mark)
{
    String name;

    // 捕获组的命名不能以数字开头
    expect(input[position], !input[position].isStdDigit(), "NOT start with a digit", position);

    Token t1{ TokenType::NamedCapturingGroupOpen, { "(?< ... >", {} } };
    tokens.emplace_back(t1);

    while (isLiteralCharacter() && input[position] != ">") {
        name += input[position];
        position++;
    }
    position++;
    return { TokenType::NamedCapturingGroupName, { { name }, {} } };
}

Token Lexer::getAssertionLookahead()
{
    return { TokenType::AssertionLookahead, { "(?=", {}} };
}

Token Lexer::getAssertionNegativeLookahead()
{
    return { TokenType::AssertionNegativeLookahead, { "(?!", {}} };
}

Token Lexer::getAssertionLookbehind()
{
    return { TokenType::AssertionLookbehind, { "(?<=", {}} };
}

Token Lexer::getAssertionNegativeLookbehind()
{
    return { TokenType::AssertionNegativeLookbehind, { "(?<!", {}} };
}

Token Lexer::getBackreference()
{
    return { TokenType::Backreference, { { input[position]}, {}} };
}

Token Lexer::getNamedBackreference()
{
    expect(input[position], input[position] == '<', "\"<\" to start a named backreference", position);
    position++;
    expect(input[position], isLiteralCharacter(), "a literal character to named a backreference", position);
    String name;
    while (isLiteralCharacter() && input[position] != '>')
    {
        name += input[position];
        position++;
    }
    expect(input[position], input[position] == '>', "\">\" to end a named backreference", position);
    Token t = { TokenType::NamedBackreference, { name, {} } };
    position++;
    return t;
}

Token Lexer::getModifier()
{
    return { TokenType::Modifier, { {}, {} } };
}

void Lexer::getCharacterClassContent()
{
    Char c = input[position];

    switch (c.toStdChar())
    {
    case '\\':
        position++;
        expect(input[position], canGetOrdinaryEscapedSequence(), "a correct escaped sequence", position);
        break;
    case '-':
        if (input[position + 1] == ']')
        {
            Token t{ TokenType::CharacterClassLiteral, { "-", {} } };
            tokens.emplace_back(t);
            position++;
            return;
        }
        else
        {
            if (tokens.back().type == TokenType::CharacterClassRange)
            {
                Token t{ TokenType::CharacterClassLiteral, { "-", {}} };
                tokens.emplace_back(t);
                position++;
            }
            else
            {
                position++;
                c = input[position];
                if (c == '\\')
                {
                    position++;
                    expect(c, canGetOrdinaryEscapedSequence(), "a correct escaped sequence", position);
                }
                else {
                    Token t{ TokenType::CharacterClassLiteral, { c, {} } };
                    tokens.emplace_back(t);
                    position++;
                }
                Token nextT = tokens.back();
                tokens.pop_back();
                Token lastT = tokens.back();
                tokens.pop_back();
                Token t{ TokenType::CharacterClassRange, { lastT.value.first, nextT.value.first } };
                tokens.emplace_back(t);
            }
        }
        break;
    default: {
        Token t{ TokenType::CharacterClassLiteral, { c, {} } };
        tokens.emplace_back(t);
        position++;
        break;
    }
    }
}

bool Lexer::canGetOrdinaryEscapedSequence()
{
    Char c = input[position];
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
    case 'x': {
        position++;
        char c1 = input[position].toStdChar();
        position++;
        char c2 = input[position].toStdChar();
        int codepoint = ((c1 >= '0' && c1 <= '9') ? c1 - '0' :
            ((c1 >= 'a' && c1 <= 'f') ? c1 - 'a' + 10 :
                ((c1 >= 'A' && c1 <= 'F') ? c1 - 'A' + 10 : 0))) * 16
            + ((c2 >= '0' && c2 <= '9') ? c2 - '0' :
                ((c2 >= 'a' && c2 <= 'f') ? c2 - 'a' + 10 :
                    ((c2 >= 'A' && c2 <= 'F') ? c2 - 'A' + 10 : 0)));

        Token t = { TokenType::UnicodeCodePoint, { { fromCodepoint(codepoint) }, { toHexString(codepoint) } } };
        tokens.emplace_back(t);
        position++;
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
        break;
    case 'd': case 'D': case 's': case 'S': case 'w': case 'W': {
        Token t = { TokenType::SpecialSequence, {{ String("\\") + c }, {} } };
        tokens.emplace_back(t);
        position++;
        break;
    }
    default:
        return false;
    }
    return true;
}

Token Lexer::getUnicodeCodePoint()
{

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
    return { TokenType::UnicodeCodePoint, { { fromCodepoint(codepoint) }, { toHexString(codepoint) }} };
}

Token Lexer::getUnicodeProperty(bool accept)
{
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
