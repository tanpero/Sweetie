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

std::unique_ptr<AST> Parser::parseExpression()
{
	auto term0 = parseTerm();
	auto expression = ast<Expression>(std::move(term0));

	while (lookahead().is(TokenType::BranchAlternation))
	{
		advance();

		// token = "|"

		if (final())
		{
			expression->addTerm(std::move(ast<Literal>("")));
			return expression;
		}

		advance();

		expression->addTerm(std::move(parseTerm()));
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

	while (!final() && !lookahead().is(TokenType::GroupClose))
	{
		// token = "|"

		if (here().is(TokenType::AnchorEnd))
		{
			term->setEndAnchor();
			return term;
		}

		term->addFactor(std::move(parseFactor()));
		advance();
	}

	return term;
}

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
	if (!final())
	{
		advance();
	}
	auto unitQuantifier = ast<Quantifier>(Quantifier::Type::Once);
	return ast<Factor>(std::move(atom), std::move(unitQuantifier));
}

std::unique_ptr<AST> Parser::parseAtom()
{
	return std::unique_ptr<AST>();
}

std::unique_ptr<AST> Parser::parseLiteral()
{
	return std::unique_ptr<AST>();
}

std::unique_ptr<AST> Parser::parseCharacterClass()
{
	return std::unique_ptr<AST>();
}

std::unique_ptr<AST> Parser::parseAnyCharacter()
{
	return std::unique_ptr<AST>();
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
	return std::unique_ptr<AST>();
}

std::unique_ptr<AST> Parser::parseNamedCapturingGroup()
{
	return std::unique_ptr<AST>();
}

std::unique_ptr<AST> Parser::parseNonapturingGroup()
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

std::unique_ptr<AST> Parser::parseQuantifier()
{
	return std::unique_ptr<AST>();
}
