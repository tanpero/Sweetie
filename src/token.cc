#include "token.hh"



std::map<TokenType, std::string> tokenTypeMap = {
    {{ TokenType::AnyCharacter}, { "AnyCharacter"}},
    {{ TokenType::LiteralCharacter}, { "LiteralCharacter" }},
    {{ TokenType::AnchorStart}, { "AnchorStart" }},
    {{ TokenType::AnchorEnd}, { "AnchorEnd" }},
    {{ TokenType::QuantifierStar}, { "QuantifierStar" }},
    {{ TokenType::QuantifierPlus}, { "QuantifierPlus" }},
    {{ TokenType::QuantifierQuestion}, { "QuantifierQuestion" }},
    {{ TokenType::QuantifierBraces}, { "QuantifierBraces" }},
    {{ TokenType::BranchAlternation}, { "BranchAlternation" }},
    {{ TokenType::CharacterClassOpen}, { "CharacterClassOpen" }},
    {{ TokenType::CharacterClassNegative }, { "CharacterClassNegative" }},
    {{ TokenType::CharacterClassClose}, { "CharacterClassClose" }},
    {{ TokenType::CharacterClassRange}, { "CharacterClassRange" }},
    {{ TokenType::CharacterClassLiteral}, { "CharacterClassLiteral" }},
    {{ TokenType::GroupOpen}, { "GroupOpen" }},
    {{ TokenType::GroupClose}, { "GroupClose" }},
    {{ TokenType::NamedCapturingGroupOpen}, { "NamedCapturingGroupOpen" }},
    {{ TokenType::NamedCapturingGroupName}, { "NamedCapturingGroupName" }},
    {{ TokenType::NamedCapturingGroupClose}, { "NamedCapturingGroupClose" }},
    {{ TokenType::NonCapturingGroupOpen}, { "NonCapturingGroupOpen" }},
    {{ TokenType::NonCapturingGroupClose}, { "NonCapturingGroupClose" }},
    {{ TokenType::AssertionLookahead}, { "AssertionLookahead" }},
    {{ TokenType::AssertionNegativeLookahead}, { "AssertionNegativeLookahead" }},
    {{ TokenType::AssertionLookbehind}, { "AssertionLookbehind" }},
    {{ TokenType::AssertionNegativeLookbehind}, { "AssertionNegativeLookbehind" }},
    {{ TokenType::SpecialSequence}, { "SpecialSequence" }},
    {{ TokenType::RecursiveMode}, { "RecursiveMode" }},
    {{ TokenType::ConditionalExpressionOpen}, { "ConditionalExpressionOpen" }},
    {{ TokenType::ConditionalExpressionClose}, { "ConditionalExpressionClose" }},
    {{ TokenType::ConditionalExpressionCondition}, { "ConditionalExpressionCondition" }},
    {{ TokenType::ConditionalExpressionBranch}, { "ConditionalExpressionBranch" }},
    {{ TokenType::BalanceGroupOpen}, { "BalanceGroupOpen" }},
    {{ TokenType::BalanceGroupName}, { "BalanceGroupName" }},
    {{ TokenType::BalanceGroupClose}, { "BalanceGroupClose" }},
    {{ TokenType::BalanceGroupBalancedOpen}, { "BalanceGroupBalancedOpen" }},
    {{ TokenType::BalanceGroupBalancedClose}, { "BalanceGroupBalancedClose" }},
    {{ TokenType::Backreference}, { "Backreference" }},
    {{ TokenType::NamedBackreference }, { "NamedBackreference" }},
    {{ TokenType::Modifier}, { "Modifier" }},
    {{ TokenType::EscapeSequence}, { "EscapeSequence" }},
    {{ TokenType::UnicodeCodePoint}, { "UnicodeCodePoint" }},
    {{ TokenType::UnicodeProperty}, { "UnicodeProperty" }}
};

String Token::toString() const
{
    return String("[Type: ") + tokenTypeMap[type] +
        String(", Value: ") + value.first + (value.second.empty() ? "" : " " + value.second) + "]\n";
}

bool Token::is(TokenType t) const
{
    return type == t;
    return false;
}
