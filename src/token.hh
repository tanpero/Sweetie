#include "string.hh"

#include <tuple>
#include <map>

enum class TokenType {

    AnyCharacter, // "."

    // 字面量字符
    LiteralCharacter, // 实例："a", "b"

    // 锚点
    AnchorStart, // 实例："^"
    AnchorEnd,   // 实例："$"

    // 量词
    QuantifierStar,   // 实例："*"
    QuantifierPlus,   // 实例："+"
    QuantifierQuestion, // 实例："?"
    QuantifierBraces, // 实例："{n,m}"

    // 分支
    BranchAlternation, // 实例："|"

    // 字符类
    CharacterClassOpen, // 实例："["
    CharacterClassClose, // 实例："]"
    CharacterClassRange, // 实例："a-z"
    CharacterClassLiteral, // 实例："a", "b"
    CharacterClassNegative, // "^"

    // 捕获组
    GroupOpen, // 实例："("
    GroupClose, // 实例：")"
    NamedCapturingGroupOpen, // 实例："(?<name>"
    NamedCapturingGroupName, // 实例："name"
    NamedCapturingGroupClose, // 实例：")"

    // 非捕获组
    NonCapturingGroupOpen, // 实例："(?:"
    NonCapturingGroupClose, // 实例：")"

    // 断言
    AssertionLookahead, // 实例："(?="
    AssertionNegativeLookahead, // 实例："(?!"
    AssertionLookbehind, // 实例："(?<="
    AssertionNegativeLookbehind, // 实例："(?<!"

    // 特殊序列
    SpecialSequence, // 实例："\d", "\w"

    // 递归模式
    RecursiveMode, // 实例："(?R)"

    // 条件表达式
    ConditionalExpressionOpen, // 实例："(?("
    ConditionalExpressionClose, // 实例：")"
    ConditionalExpressionCondition, // 实例："n"
    ConditionalExpressionBranch, // 实例："|"

    // 平衡组
    BalanceGroupOpen, // 实例："(?<open>"
    BalanceGroupName, // 实例："open"
    BalanceGroupClose, // 实例：")"
    BalanceGroupBalancedOpen, // 实例："(?<close>"
    BalanceGroupBalancedClose, // 实例：")"

    // 后引用
    Backreference, // 实例："\1"，数值最大为 "\9"
    NamedBackreference, // 实例："\k<name>"

    // 修饰符
    Modifier, // 实例："?i"

    // 转义序列
    EscapeSequence, // 实例："\\"

    // Unicode 码点
    UnicodeCodePoint, // 实例："\u{1F600}"

    // Unicode 属性
    UnicodeProperty, // 实例："\p{L}"

};


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


struct Token {
    TokenType type;

    // 对于字符字面量、单字符标记等，只需要存储 first 成员；
    // 对于 {m, n}、\p{Script=Xxx} 等，将两个信息分别存入 first 和 second
    std::pair<String, String> value; 

    String toString() const {
        return String("[Type: ") + tokenTypeMap[type] +
            String(", Value: ") + value.first + (value.second.empty() ? "" : " " + value.second) + "]\n";
    }
};