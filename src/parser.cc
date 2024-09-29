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

std::unique_ptr<AST> Parser::parse()
{
    return ast<Regex>(parseExpression());
}
/*
std::unique_ptr<AST> Parser::parseExpression()
{
    auto term0 = parseTerm();
    auto expression = ast<Expression>(std::move(term0));

    while (!final() && !lookahead().is(TokenType::BranchAlternation))
    {
        advance();

        auto term = parseTerm();
        expression->addTerm(std::move(term));
    }

    return expression;
}

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

    // [a-z](abc)def
    // token = '('

    while (!here().is(TokenType::GroupClose))
    {

        if (here().is(TokenType::AnchorEnd))
        {
            term->setEndAnchor();
            return term;
        }

        auto factor = parseFactor();

        term->addFactor(std::move(factor));

        if (final())
        {
            break;
        }

        if (here().is(TokenType::GroupClose))
        {
            advance();
        }

    }

    return term;
}

// 进入此函数前，token 为 Factor 的第一个 token
// 退出此函数后，token 为 下一个 Factor 的第一个 token
std::unique_ptr<AST> Parser::parseFactor()
{

    bool isAssertion =
        here().is(TokenType::AssertionLookahead) ||
        here().is(TokenType::AssertionNegativeLookahead) ||
        here().is(TokenType::AssertionLookbehind) ||
        here().is(TokenType::AssertionNegativeLookbehind);
    if (isAssertion)
    {
        auto factor = ast<Factor>(std::move(parseAssertion()));
        return factor;
    }
    auto atom = ast<Atom>(std::move(parseAtom()));
    if (final())
    {
        auto unitQuantifier = ast<Quantifier>(Quantifier::Type::Once);
        return ast<Factor>(std::move(atom), std::move(unitQuantifier));
    }
    if (here().is(TokenType::CharacterClassClose))
    {
        advance();
    }
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
        if (!final())
        {
            advance();
        }
        return ast<Factor>(std::move(atom), std::move(quantifier));
    }
    if (lookahead().is(TokenType::QuantifierStar))
    {
        advance();
        auto quantifier = ast<Quantifier>(Quantifier::Type::ZeroOrMore);
        if (!final())
        {
            advance();
        }
        return ast<Factor>(std::move(atom), std::move(quantifier));
    }
    if (lookahead().is(TokenType::QuantifierPlus))
    {
        advance();
        auto quantifier = ast<Quantifier>(Quantifier::Type::OneOrMore);
        if (!final())
        {
            advance();
        }
        return ast<Factor>(std::move(atom), std::move(quantifier));
    }
    if (lookahead().is(TokenType::QuantifierQuestion))
    {
        advance();
        auto quantifier = ast<Quantifier>(Quantifier::Type::ZeroOrOne);
        if (!final())
        {
            advance();
        }
        return ast<Factor>(std::move(atom), std::move(quantifier));
    }
    
    advance();

    auto unitQuantifier = ast<Quantifier>(Quantifier::Type::Once);
    return ast<Factor>(std::move(atom), std::move(unitQuantifier));
}

// 解析原子后 token 索引不会自动向前，需要在调用该解析器之后显式步进索引
std::unique_ptr<AST> Parser::parseAtom()
{
    Token t = here();
    if (t.is(TokenType::LiteralCharacter) || t.is(TokenType::UnicodeCodePoint))
    {
        return ast<Literal>(t.value.first[0]);
    }
    else if (t.is(TokenType::AnyCharacter))
    {
        return ast<AnyCharacter>();
    }
    else if (t.is(TokenType::CharacterClassOpen))
    {
        bool isNegativeClass = lookahead().is(TokenType::CharacterClassNegative);
        if (isNegativeClass)
        {
            advance();
            advance();
        }
        else
        {
            advance();
        }
        if (t.is(TokenType::CharacterClassClose))
        {
            error("Character classes without actual directionality");
            return nullptr;
        }
        auto cc = ast<CharacterClass>(isNegativeClass);
        while (current < tokens.size() && !here().is(TokenType::CharacterClassClose))
        {
            t = here();
            if (t.is(TokenType::CharacterClassLiteral) || t.is(TokenType::UnicodeCodePoint))
            {
                cc->addChar(t.value.first[0]);
            }
            else
            {
                cc->addRange({ t.value.first[0], t.value.second[0] });
            }
            advance();


        }
        return cc;
    }
    else if (t.is(TokenType::GroupOpen))
    {
        advance();
        auto expr = ast<Expression>(std::move(parseExpression()));
        return expr;
    }
    
    
    return nullptr;
}

std::unique_ptr<AST> Parser::parseAssertion()
{
    return std::unique_ptr<AST>();
}

std::unique_ptr<AST> Parser::parseLookaheadAssertion()
{
    return std::unique_ptr<AST>();
}

std::unique_ptr<AST> Parser::parseNegativeLookaheadAssertion()
{
    return std::unique_ptr<AST>();
}

std::unique_ptr<AST> Parser::parseLookbehindAssertion()
{
    return std::unique_ptr<AST>();
}

std::unique_ptr<AST> Parser::parseNegativeLookbehindAssertion()
{
    return std::unique_ptr<AST>();
}

std::unique_ptr<AST> Parser::parseWordBound()
{
    return std::unique_ptr<AST>();
}

std::unique_ptr<AST> Parser::parseGroup()
{
    // token = '('
    advance();
    if (lookahead().is(TokenType::GroupClose))
    {
        error("Capturing group without actual directionality");
        return nullptr;
    }
    auto group = ast<CapturingGroup>(parseExpression());
    return std::unique_ptr<AST>();
}

*/
std::unique_ptr<AST> Parser::parseExpression()
{
    auto term0 = parseTerm();
    auto expression = ast<Expression>(std::move(term0));
    return expression;
}

// parseFactor 将不会在退出前自动移动索引！！！

std::unique_ptr<AST> Parser::parseTerm()
{
    auto factor0 = parseFactor();

    auto term = ast<Term>(false, std::move(factor0));


    // 这时 token 还是该 Factor 所对应的最后一个 token
    // 若它也是整个正则式的最后一个 token，就结束

    if (final())
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

    } while (!final());
    return term;
}
std::unique_ptr<AST> Parser::parseFactor()
{
    // 一个 Factor 要么是一个 Atom 及其量词，要么是一个断言

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
        return ast<Atom>(std::move(l));
    }
    else if (t.is(TokenType::AnyCharacter))
    {
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

        return characterClass;
    }

    return std::unique_ptr<AST>();
}
std::unique_ptr<AST> Parser::parseGroup()
{
    return std::unique_ptr<AST>();
}
std::unique_ptr<AST> Parser::parseNamedCapturingGroup()
{
    return std::unique_ptr<AST>();
}

std::unique_ptr<AST> Parser::parseNonCapturingGroup()
{
    return std::unique_ptr<AST>();
}

std::unique_ptr<AST> Parser::parseBackreference()
{
    return std::unique_ptr<AST>();
}

std::unique_ptr<AST> Parser::parseUnicodeProperty()
{
    return std::unique_ptr<AST>();
}

