#include "parser.hh"

Token Parser::lookahead() const
{
	return tokens[current + 1];
}

void Parser::advance()
{
	current++;
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
	return parseExpression();
}

std::unique_ptr<AST> Parser::parseExpression()
{
	return std::unique_ptr<AST>();
}

std::unique_ptr<AST> Parser::parseTerm()
{
	return std::unique_ptr<AST>();
}

std::unique_ptr<AST> Parser::parseFactor()
{
	return std::unique_ptr<AST>();
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
