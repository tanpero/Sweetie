#include "parser.hh"

bool Parser::final() const
{
    return current == tokens.size() - 1;
}

bool Parser::end() const
{
    return current >= tokens.size();
}

Token Parser::lookahead() const
{
    return tokens[current + 1];
}

void Parser::advance()
{
    current++;
}

void Parser::advanceWhenNonFinal()
{
    if (!final()) advance();
}

Token Parser::here() const
{
    return tokens[current];
}

void Parser::error(const String& message) const
{
    std::cerr << message << std::endl;
    exit(-1);
}

std::unique_ptr<AST> Parser::convertSpecialSequenceToActualAST(SpecialSequenceType type)
{

    // The \s equivalents to
    //     [\f\n\r\t\v\u0020\u00a0\u1680\u2000-\u200a\u2028\u2029\u202f\u205f\u3000\ufeff]
    auto spaceCharacterClass = ast<CharacterClass>(type == SpecialSequenceType::S);
    spaceCharacterClass->addChar("\f");
    spaceCharacterClass->addChar("\n");
    spaceCharacterClass->addChar("\r");
    spaceCharacterClass->addChar("\t");
    spaceCharacterClass->addChar("\v");
    spaceCharacterClass->addChar(fromCodepoint(0x20));
    spaceCharacterClass->addChar(fromCodepoint(0xa0));
    spaceCharacterClass->addChar(fromCodepoint(0x1680));
    spaceCharacterClass->addRange({ fromCodepoint(0x2000), fromCodepoint(0x200a) });
    spaceCharacterClass->addChar(fromCodepoint(0x2028));
    spaceCharacterClass->addChar(fromCodepoint(0x2029));
    spaceCharacterClass->addChar(fromCodepoint(0x202f));
    spaceCharacterClass->addChar(fromCodepoint(0x205f));
    spaceCharacterClass->addChar(fromCodepoint(0x3000));
    spaceCharacterClass->addChar(fromCodepoint(0xfeff));

    // The \w equivalents to
    //     [0-9A-Za-z_]
    auto wordCharacterClass = ast<CharacterClass>(type == SpecialSequenceType::W);
    wordCharacterClass->addRange({ "0", "9" });
    wordCharacterClass->addRange({ "A", "Z" });
    wordCharacterClass->addRange({ "a", "z" });
    wordCharacterClass->addChar("_");

    // The \d equivalents to
    //     [0-9]
    auto digitCharacterClass = ast<CharacterClass>(type == SpecialSequenceType::D);
    digitCharacterClass->addRange({ "0", "9" });

    // The \b equivalents to
    //     (?<=\w)(?=\W)|(?<=\W)(?=\w)
    // The \B equivalents to
    //     (?<=\w)(?=\w)|(?<=\W)(?=\W)
    // Note:
    //     At the appropriate time, a simpler and faster implementation method will be used
    // TODO...

    switch (type)
    {
    case SpecialSequenceType::r:
        return ast<Atom>(std::move(ast<Literal>("\r")));
    case SpecialSequenceType::n:
        return ast<Atom>(std::move(ast<Literal>("\n")));
    case SpecialSequenceType::f:
        return ast<Atom>(std::move(ast<Literal>("\f")));
    case SpecialSequenceType::v:
        return ast<Atom>(std::move(ast<Literal>("\v")));
    case SpecialSequenceType::t:
        return ast<Atom>(std::move(ast<Literal>("\t")));
    case SpecialSequenceType::s: case SpecialSequenceType::S:
        return ast<Atom>(std::move(spaceCharacterClass));
    case SpecialSequenceType::w: case SpecialSequenceType::W:
        return ast<Atom>(std::move(wordCharacterClass));
    case SpecialSequenceType::d: case SpecialSequenceType::D:
        return ast<Atom>(std::move(digitCharacterClass));
    case SpecialSequenceType::b: case SpecialSequenceType::B:
        error("Internal Error: \\b and \\B is not implemented");
        return nullptr;
    default:
        return nullptr;
    }
}

std::unique_ptr<AST> Parser::parse()
{
    return ast<Regex>(parseExpression());
}

std::unique_ptr<AST> Parser::parseExpression()
{
    if (here().is(TokenType::GroupClose))
    {
        error("Capturing group without actual directionality");
        return nullptr;
    }
    auto term0 = parseTerm();
    auto expression = ast<Expression>(std::move(term0));

    while (!final() && !lookahead().is(TokenType::GroupClose))
    {
        advance();

        if (here().is(TokenType::BranchAlternation))
        {
            if (final())
            {
                expression->addTerm(std::move(ast<Term>()));
                break;
            }
            advance();

            expression->addTerm(std::move(parseTerm()));
        }
        else if (lookahead().is(TokenType::GroupClose)) {
            advance();
            break;
        }
        else break;
    }

    return expression;
}

// parseFactor 将不会在退出前自动移动索引！！！

std::unique_ptr<AST> Parser::parseTerm()
{
    bool hasAnchorStart = false;
    if (here().is(TokenType::AnchorStart))
    {
        hasAnchorStart = true;
        advance();
    }

    auto factor0 = parseFactor();

    auto term = ast<Term>(hasAnchorStart, std::move(factor0));

    // 这时 token 还是该 Factor 所对应的最后一个 token
    // 若它也是整个正则式的最后一个 token，就结束

    if (final() || lookahead().is(TokenType::GroupClose))
    {
        return term;
    }

    // 接下来 token 应该是下一个 Factor 的第一个 token

    do
    {
        advance();

        auto factor = std::move(parseFactor());

        // token 是该 Factor 的最后一个 token

        term->addFactor(std::move(factor));

    } while (!final()
        && !lookahead().is(TokenType::AnchorEnd)
        && !lookahead().is(TokenType::BranchAlternation));

    if (final()) return term;

    if (lookahead().is(TokenType::AnchorEnd))
    {
        term->setEndAnchor();
        advance();
        return term;
    }
    if (lookahead().is(TokenType::BranchAlternation))
    {
        return term;
    }

    error("Internal Error from Parser::parseTerm()");
    return nullptr;
}
std::unique_ptr<AST> Parser::parseFactor()
{
    // 一个 Factor 要么是一个 Atom 及其量词，要么是一个断言

    bool isAssertion =
        here().is(TokenType::AssertionLookahead) ||
        here().is(TokenType::AssertionNegativeLookahead) ||
        here().is(TokenType::AssertionLookbehind) ||
        here().is(TokenType::AssertionNegativeLookbehind);
    if (isAssertion)
    {
        Token next = lookahead();
        switch (next.type)
        {
        case TokenType::QuantifierBraces:
        case TokenType::QuantifierStar:
        case TokenType::QuantifierPlus:
        case TokenType::QuantifierQuestion:
            error("Unexpected quantifier after an assertion");
            return nullptr;
        }
        auto assertion = ast<Factor>(std::move(parseAssertion()));
        return assertion;
    }

    // 现在 token 是该 Atom 的第一个 token

    auto atom = parseAtom();

    // 现在 token 是该 Atom 的最后一个 token
        
    // 若 Atom 是一个字符，则显然索引没有变化

    if (final())
    {
        auto factor = ast<Factor>(std::move(atom), std::move(ast<Quantifier>(Quantifier::Type::Once)));
        return factor;
    }

    // 下一个 token 会是量词吗？
    if (lookahead().is(TokenType::QuantifierBraces))
    {
        advance();
        int min = toInteger(here().value.first);
        int max = toInteger(here().value.second);
        if (min > max)
        {
            error("numbers out of order in {} quantifier");
        }
        auto quantifier = ast<Quantifier>(min, max);
        return ast<Factor>(std::move(atom), std::move(quantifier));
    }
    if (lookahead().is(TokenType::QuantifierStar))
    {
        advance();
        auto quantifier = ast<Quantifier>(Quantifier::Type::ZeroOrMore);
        return ast<Factor>(std::move(atom), std::move(quantifier));
    }
    if (lookahead().is(TokenType::QuantifierPlus))
    {
        advance();
        auto quantifier = ast<Quantifier>(Quantifier::Type::OneOrMore);
        return ast<Factor>(std::move(atom), std::move(quantifier));
    }
    if (lookahead().is(TokenType::QuantifierQuestion))
    {
        advance();
        auto quantifier = ast<Quantifier>(Quantifier::Type::ZeroOrOne);
        return ast<Factor>(std::move(atom), std::move(quantifier));
    }

    // 下一个 token 不是量词，那么不移动索引，索引依然指向当前 Factor 的最后一个 token

    auto quantifier = ast<Quantifier>(Quantifier::Type::Once);
    auto factor = ast<Factor>(std::move(atom), std::move(quantifier));

    return factor;
}
std::unique_ptr<AST> Parser::parseAtom()
{
    // 一个 Atom 可能是一个单字符、字符类、组
    // 当前 token 为 Atom 的第一个 token

    Token t = here();

    if (t.is(TokenType::LiteralCharacter) || t.is(TokenType::UnicodeCodePoint))
    {
        auto l = ast<Literal>(t.value.first[0]);
        if (!final() && lookahead().is(TokenType::GroupClose))
        {
            advance();
        }
        return ast<Atom>(std::move(l));
    }
    else if (t.is(TokenType::SpecialSequence))
    {
        SpecialSequenceType type = translateSpecialSequence(t.value.first);
        return convertSpecialSequenceToActualAST(type);
    }
    else if (t.is(TokenType::AnyCharacter))
    {
        if (!final() && lookahead().is(TokenType::GroupClose))
        {
            advance();
        }
        return ast<Atom>(std::move(ast<AnyCharacter>()));
    }
    else if (t.is(TokenType::CharacterClassOpen))
    {
        advance();
        bool isNegative = here().is(TokenType::CharacterClassNegative);

        auto characterClass = ast<CharacterClass>(isNegative);

        if ((isNegative && lookahead().is(TokenType::CharacterClassClose))
            || (!isNegative && here().is(TokenType::CharacterClassClose))) {
            error("Character classes without actual directionality");
            return nullptr;
        }
        
        if (isNegative)
        {
            advance();
        }

        while (!here().is(TokenType::CharacterClassClose))
        {
            t = here();
            if (t.is(TokenType::CharacterClassLiteral) || t.is(TokenType::UnicodeCodePoint))
            {
                characterClass->addChar(t.value.first[0]);
            }
            else if (t.is(TokenType::CharacterClassRange))
            {
                characterClass->addRange({ t.value.first[0],
                                           t.value.second[0] });
            }
            else
            {
                error("Internal Error: from Parser::parseAtom()");
            }
            advance();
        }
        if (!final() && lookahead().is(TokenType::GroupClose))
        {
            advance();
        }

        return characterClass;
    }
    else if (t.is(TokenType::GroupOpen))
    {
        return ast<Atom>(std::move(parseGroup()));
    }
    else if (t.is(TokenType::NamedCapturingGroupOpen))
    {
        return ast<Atom>(std::move(parseNamedCapturingGroup()));
    }
    else if (t.is(TokenType::NonCapturingGroupOpen))
    {
        return ast<Atom>(std::move(parseNonCapturingGroup()));
    }
    else if (t.is(TokenType::Backreference))
    {
        return ast<Atom>(std::move(ast<Backreference>(toInteger(t.value.first))));
    }
    else if (t.is(TokenType::NamedBackreference))
    {
        return ast<Atom>(std::move(ast<Backreference>(t.value.first)));
    }
    else if (  t.is(TokenType::AssertionLookahead)
        || t.is(TokenType::AssertionNegativeLookahead)
        || t.is(TokenType::AssertionLookbehind)
        || t.is(TokenType::AssertionNegativeLookbehind))
    {
        return ast<Atom>(std::move(parseAssertion()));
    }
    error("Internal Error from Parser::parseAtom()");
    return std::unique_ptr<AST>();
}
std::unique_ptr<AST> Parser::parseGroup()
{
    if (lookahead().is(TokenType::GroupClose))
    {
        error("Capturing group without actual directionality");
        return nullptr;
    }
    advance();
    auto group = ast<CapturingGroup>(std::move(parseExpression()));
    return group;
}
std::unique_ptr<AST> Parser::parseNamedCapturingGroup()
{
    if (!lookahead().is(TokenType::NamedCapturingGroupName))
    {
        error("Named capturing group without actual directionality");
        return nullptr;
    }

    advance();

    if (lookahead().is(TokenType::GroupClose))
    {
        error("Named capturing group without actual directionality");
        return nullptr;
    }

    String name = here().value.first;

    advance();

    auto group = ast<NamedCapturingGroup>(name, std::move(parseExpression()));
    return group;
}

std::unique_ptr<AST> Parser::parseNonCapturingGroup()
{
    if (!lookahead().is(TokenType::NamedCapturingGroupName))
    {
        error("Non-capturing group without actual directionality");
        return nullptr;
    }

    advance();

    if (lookahead().is(TokenType::GroupClose))
    {
        error("Non-capturing group without actual directionality");
        return nullptr;
    }

    advance();
    auto group = ast<NonCapturingGroup>(std::move(parseExpression()));
    return group;
}

std::unique_ptr<AST> Parser::parseUnicodeProperty()
{
    return std::unique_ptr<AST>();
}

std::unique_ptr<AST> Parser::parseAssertion()
{
    switch (here().type)
    {
    case TokenType::AssertionLookahead:
        if (lookahead().is(TokenType::GroupClose))
        {
            error("Lookahead assertion without actual directionality");
            return nullptr;
        }
        advance();
        return ast<LookaheadAssertion>(std::move(parseExpression()), true);
    case TokenType::AssertionNegativeLookahead:
        if (lookahead().is(TokenType::GroupClose))
        {
            error("Negative lookahead assertion without actual directionality");
            return nullptr;
        }
        advance();
        return ast<LookaheadAssertion>(std::move(parseExpression()), false);
    case TokenType::AssertionLookbehind:
        if (lookahead().is(TokenType::GroupClose))
        {
            error("Lookbehind assertion without actual directionality");
            return nullptr;
        }
        advance();
        return ast<LookbehindAssertion>(std::move(parseExpression()), true);
    case TokenType::AssertionNegativeLookbehind:
        if (lookahead().is(TokenType::GroupClose))
        {
            error("Negative lookbehind assertion without actual directionality");
            return nullptr;
        }
        advance();
        return ast<LookbehindAssertion>(std::move(parseExpression()), false);
    }
}


