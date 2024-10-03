#include "ast.hh"

static int capturingGroupId = 0;

CharacterClass::CharacterClass(bool isNegative) : isNegative(isNegative) {}

void CharacterClass::addRange(const std::pair<Char, Char>& range) {
    ranges.emplace_back(range);
}
void CharacterClass::addChar(const Char& ch) {
    chars.insert(ch);
}

void CharacterClass::concatRanges(const std::vector<std::pair<Char, Char>>& _ranges) {
    for (const auto& range : _ranges) {
        auto it = ranges.begin();
        while (it != ranges.end()) {
            if (it->second < range.first - 1) {
                if ((it->second + 1) >= (range.first - 1)) {
                    it->second = std::max(it->second, range.second);
                }
                ranges.emplace(it, range);
                break;
            }
            else if (range.second < it->first - 1) {
                if ((range.second + 1) >= (it->first - 1)) {
                    it->first = std::min(it->first, range.first);
                }
                ++it;
            }
            else {
                it->first = std::min(it->first, range.first);
                it->second = std::max(it->second, range.second);
                it = ranges.erase(it);
            }
        }
        if (it == ranges.end()) {
            ranges.push_back(range);
        }
    }
}

void CharacterClass::concatChars(const std::set<Char>& _chars) {
    chars.insert(_chars.begin(), _chars.end());
}

std::vector<std::pair<Char, Char>>&& CharacterClass::getRanges()
{
    return std::move(ranges);
}

std::set<Char>&& CharacterClass::getChars()
{
    return std::move(chars);
}

void CharacterClass::negative()
{
    isNegative = !isNegative;
}

String CharacterClass::toString() const
{
    String s1 = "Character Class:\n  <Single Characters:";
    for (const Char& c : chars) {
        s1 += " " + c;
    }
    s1 += ">\n  <Ranges:";
    for (auto& range : ranges) {
        s1 += " [" + toHexString(range.first.toCodepoint()) + " - "
            + toHexString(range.second.toCodepoint()) + "]";
    }
    s1 += ">\n";
    return s1;
}

String AnyCharacter::toString() const
{
    return "Any Character\n";
}

Literal::Literal(Char v) : value(v) {}

String Literal::toString() const
{
    return value + "\n";
}

String Anchor::toString() const
{
    return String(anchorType == Type::Begin ? "<At the head of one line>" : "<At the tail of one line>") + "\n";
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

LookaheadAssertion::LookaheadAssertion(std::unique_ptr<AST> expr, bool positive)
{
    expression = std::move(expr);
    isPositive = positive;
}

String LookaheadAssertion::toString() const
{
    return (isPositive ? "L" : "Negative l") + String{ "ookahead assertion:\n" }
        + expression->toString();
}

LookbehindAssertion::LookbehindAssertion(std::unique_ptr<AST> expr, bool positive)
{
    expression = std::move(expr);
    isPositive = positive;
}

String LookbehindAssertion::toString() const
{
    return (isPositive ? "L" : "Negative l") + String{ "ookbehind assertion\n"}
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


Quantifier::Quantifier() : type(Type::Once)
{
}

Quantifier::Quantifier(int min, int max) : values({ min, max }) {
    type = Type::Designated;
}

Quantifier::Quantifier(Type type) : type(type) {
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
}

String Quantifier::toString() const
{
    String s;
    switch (type)
    {
    case Quantifier::Type::Once:
        s = "No Quantifier";
        break;
    case Quantifier::Type::OneOrMore:
        s = "Quantifier: One or more";
        break;
    case Quantifier::Type::ZeroOrMore:
        s = "Quantifier: Zero or more";
        break;
    case Quantifier::Type::ZeroOrOne:
        s = "Quantifier: Zero or one";
        break;
    case Quantifier::Type::Designated:
        s = "Quantifier: At least " + String(values.first) + ", at most "
            + (values.second != -1 ? String(values.second) : "infinity");
        break;
    }
    s += "\n";
    return s;
}

Factor::Factor(std::unique_ptr<AST> assertion) : assertion(std::move(assertion))
{
    type = Type::Assertion;
}
 
Factor::Factor(std::unique_ptr<AST> atom, std::unique_ptr<AST> quantifier)
    : atom_quantifier({ std::move(atom), std::move(quantifier) })
{
    type = Type::Atom_Quantifier;
}

String Factor::toString() const
{
    return "[Begin Factor]\n"
        +String(type == Type::Assertion ? "<Assertion>\n" + assertion->toString()
        : "<Atom>\n" + atom_quantifier.first->toString()
            + "<Quantifier>\n" + atom_quantifier.second->toString()) + "[End Factor]\n";
}

Term::Term(bool _beginAnchor, std::unique_ptr<AST> factor0)
    : hasBeginAnchor(_beginAnchor), factor_(std::move(factor0))
{
    factors.emplace_back(std::move(factor_));
}

Term::Term()
{
    hasBeginAnchor = hasEndAnchor = false;
    factors.emplace_back(std::move(
        ast<Factor>(
            ast<Atom>(
                ast<Literal>("")
            ),
            ast<Quantifier>()
        )
    ));
}

void Term::addFactor(std::unique_ptr<AST> factor)
{
    factors.emplace_back(std::move(factor));
}

void Term::setEndAnchor()
{
    hasEndAnchor = true;
}

String Term::toString() const
{
    String s = "[Begin Term]\n";
    if (hasBeginAnchor)
    {
        s += "<Expect to see the beginning of the line>\n";
    }
    for (auto& fac : factors) {
        s += fac->toString();
    }
    if (hasEndAnchor)
    {
        s += "<Expect to see the ending of the line>\n";
    }
    s += "[End Term]\n";
    return s;
}

Expression::Expression(std::unique_ptr<AST> term0) : term_(std::move(term0))
{
    terms.emplace_back(std::move(term_));
}

void Expression::addTerm(std::unique_ptr<AST> term)
{
    terms.emplace_back(std::move(term));
}

String Expression::toString() const
{
    String s = "[Begin Expression]\n";
    for (auto& t : terms) {
        s += t->toString();
    }
    s += "[End Expression]\n";
    return s;
}

Regex::Regex(std::unique_ptr<AST> expr) : expression(std::move(expr))
{
}

String Regex::toString() const {
    return "[Begin Regex]\n" + expression->toString() + "[End Regex]\n";
}

Atom::Atom(std::unique_ptr<AST> atom_) : atom(std::move(atom_))
{
}

String Atom::toString() const
{
    return "[Begin Atom]\n" + atom->toString() + "[End Atom]\n";
}

SpecialSequenceType translateSpecialSequence(const String& seq)
{
    // seq: "\d", get 'd'
    switch (seq[1].toStdChar())
    {
    case 'r': return SpecialSequenceType::r;
    case 'n': return SpecialSequenceType::n;
    case 'f': return SpecialSequenceType::f;
    case 'v': return SpecialSequenceType::v;
    case 't': return SpecialSequenceType::t;
    case 's': return SpecialSequenceType::s;
    case 'S': return SpecialSequenceType::S;
    case 'w': return SpecialSequenceType::w;
    case 'W': return SpecialSequenceType::W;
    case 'd': return SpecialSequenceType::d;
    case 'D': return SpecialSequenceType::D;
    case 'b': return SpecialSequenceType::b;
    case 'B': return SpecialSequenceType::B;
    }
}
