#include "ast.hh"

static int capturingGroupId = 0;

// 字符类范围节点
void CharacterClass::addRange(const std::pair<Char, Char>& range) {
    ranges.emplace_back(range);
}
void CharacterClass::addChar(const Char& ch) {
    chars.insert(ch);
}

String CharacterClass::toString() const
{
    String s1 = "Character Class:\n  <Single Characters:";
    for (const Char& c : chars) {
        s1 += " " + c;
    }
    s1 += ">\n  <Ranges:";
    for (auto& range : ranges) {
        s1 += " [" + toHexString(range.first.toCodepoint())
            + toHexString(range.second.toCodepoint()) + "]";
    }
    s1 += ">\n";
    return s1;
}

String AnyCharacter::toString() const
{
    return "Any Character";
}

String Literal::toString() const
{
    return value;
}

String Anchor::toString() const
{
    return anchorType == Type::Begin ? "^" : "$";
}

Quantifier::Quantifier(std::unique_ptr<AST> subj, int min, int max) :
    values({ min, max }), subject(std::move(subj)) {
    type = Type::Designated;
}

Quantifier::Quantifier(std::unique_ptr<AST> subj, Type type) :
    subject(std::move(subj)), type(type) {
    switch (type)
    {
    case Quantifier::Type::OneOrMore:
        values = { 1, -1 };
        break;
    case Quantifier::Type::ZeroOrMore:
        values = { 0, -1 };
        break;
    case Quantifier::Type::ZeroOrOne:
        values = { 0, 1 };
        break;
    }
};

String Quantifier::toString() const
{
    String s;
    switch (type)
    {
    case Quantifier::Type::OneOrMore:
        s = "Qualifier: One or more";
        break;
    case Quantifier::Type::ZeroOrMore:
        s = "Qualifier: Zero or more";
        break;
    case Quantifier::Type::ZeroOrOne:
        s = "Qualifier: Zero or one";
        break;
    case Quantifier::Type::Designated:
        s = "Qualifier: At least " + String(values.first) + ", at most "
            + (values.second != -1 ? String(values.second) : "infinity") + "\n";
        break;
    }
    return s + subject->toString();
}

CapturingGroup::CapturingGroup(std::unique_ptr<AST> _expr)
{
    capturingGroupId += 1;
    id = capturingGroupId;
    expression = std::move(_expr);
}

String CapturingGroup::toString() const
{
    return String("Capturing group with ID: ") + id + "\n" + expression->toString();
}

NamedCapturingGroup::NamedCapturingGroup(const String& groupName, std::unique_ptr<AST> expr)
{
    capturingGroupId += 1;
    id = capturingGroupId;
    name = groupName;
    expression = std::move(expr);
}

String NamedCapturingGroup::toString() const
{
    return String("Capturing group with ID: ") + id + 
        ", name: " + name + "\n" + expression->toString();
}

NonCapturingGroup::NonCapturingGroup(std::unique_ptr<AST> expr)
{
    expression = std::move(expr);
}

String NonCapturingGroup::toString() const
{
    return "Non-capturing group:\n" + expression->toString();
}

LookaheadAssertion::LookaheadAssertion(std::unique_ptr<AST> expr, bool negative)
{
    expression = std::move(expr);
    isNegative = negative;
}

String LookaheadAssertion::toString() const
{
    return (isNegative ? "Negative l" : "L") + String{ "ookahead assertion:\n" }
        + expression->toString();
}

LookbehindAssertion::LookbehindAssertion(std::unique_ptr<AST> expr, bool negative)
{
    expression = std::move(expr);
    isNegative = negative;
}

String LookbehindAssertion::toString() const
{
    return (isNegative ? "Negative l" : "L") + String{ "ookbehind assertion\n"}
        + expression->toString();
}

Backreference::Backreference(const String& ref)
{
    id = 0;
    reference = ref;
}

Backreference::Backreference(int _id)
{
    id = _id;
    reference = "";
}

String Backreference::toString() const
{
    return String{ "Backreference for " } +
        (id ? "ID: " + String{ id } : String{ "name: " } + reference) + "\n";
}

Alternation::Alternation(std::unique_ptr<AST> l, std::unique_ptr<AST> r)
{
    left = std::move(l);
    right = std::move(r);
}

String Alternation::toString() const
{
    return String{ "Alternation:\n" } + " - Left: " + left->toString()
        + "\n - Right: " + right->toString() + "\n";
}

UnicodeProperty::UnicodeProperty(const String& name, const String& value)
{
    propertyName = name;
    propertyValue = value;
}

String UnicodeProperty::toString() const
{
    return "Unicode Property: <Name: " + propertyName +
        (propertyValue.length() ? " Value: " + propertyValue : "") + ">\n";
}

SpecialSequence::SpecialSequence(const String& seq)
{
    // seq: "\d", get 'd'
    switch (seq[1].toStdChar())
    {
    case 'r': type = Type::r; break;
    case 'n': type = Type::n; break;
    case 'f': type = Type::f; break;
    case 'v': type = Type::v; break;
    case 't': type = Type::t; break;
    case 's': type = Type::s; break;
    case 'S': type = Type::S; break;
    case 'w': type = Type::w; break;
    case 'W': type = Type::W; break;
    case 'd': type = Type::d; break;
    case 'D': type = Type::D; break;
    }
}

String SpecialSequence::toString() const
{
    return "<Special Sequence>\n";
}
