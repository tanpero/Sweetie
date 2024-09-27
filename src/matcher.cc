#include "matcher.hh"


void remove(std::vector< std::pair<int, int>>& ranges, std::pair<int, int> to_remove) {
    std::vector<std::pair<int, int>> result;
    for (auto& range : ranges) {
        if (range.second < to_remove.first) {
            // 当前范围在待删除范围之前
            result.push_back(range);
        }
        else if (range.first > to_remove.second) {
            // 当前范围在待删除范围之后
            result.push_back(range);
        }
        else {
            // 有重叠
            if (range.first < to_remove.first) {
                // 前半部分保留
                result.push_back({ range.first, to_remove.first - 1 });
            }
            if (range.second > to_remove.second) {
                // 后半部分保留
                result.push_back({ to_remove.second + 1, range.second });
            }
        }
    }
    ranges = result;
}


bool canBeMatchedAsAnyCharacter(Char c) {
    return c != '\r' && c != '\n' &&
        c != fromCodepoint(0x2028)[0] && c != fromCodepoint(0x2029)[0];
}

Matcher::Matcher() : type(Type::AnyCharacter) {
}

Matcher::Matcher(const Char& c) : literal(c), type(Type::Literal) {}

Matcher::Matcher(const Char& start, const Char& end) : type(Type::CharacterClass) {
    codepointRanges.emplace_back(start.toCodepoint(), end.toCodepoint());
}

Matcher::Matcher(const Matcher& other)
    : type(other.type), literal(other.literal), enumerations(other.enumerations), codepointRanges(other.codepointRanges) {
}

Matcher& Matcher::operator+=(const Char& c) {
    switch (type)
    {
    case Matcher::Type::Literal:
        type = Type::CharacterClass;
        enumerations.emplace_back(literal.toCodepoint());
        literal = "";
        break;
    case Matcher::Type::AnyCharacter:
        type = Type::CharacterClass;
        break;
    default:
        break;
    }

    enumerations.emplace_back(c.toCodepoint());
    return *this;
}

Matcher& Matcher::operator+=(const std::pair<Char, Char>& range) {
    switch (type)
    {
    case Matcher::Type::Literal:
        type = Type::CharacterClass;
        enumerations.emplace_back(literal.toCodepoint());
        literal = "";
        break;
    case Matcher::Type::AnyCharacter:
        type = Type::CharacterClass;
        break;
    default:
        break;
    }
    std::pair<int, int> p{ range.first.toCodepoint(), range.second.toCodepoint() };
    codepointRanges.emplace_back(p);
    return *this;
}

Matcher Matcher::operator+(const Char& c) {
    Matcher result = *this;
    result.type = Type::CharacterClass;
    result.enumerations.emplace_back(c.toCodepoint());
    return result;
}

Matcher Matcher::operator+(const std::pair<Char, Char>& range) {
    Matcher result = *this;
    result.type = Type::CharacterClass;
    std::pair<int, int> p{ range.first.toCodepoint(), range.second.toCodepoint() };
    result.codepointRanges.emplace_back(p);
    return result;
}

Matcher Matcher::operator-(const Char& c) {
    Matcher result = *this;
    result -= c;
    return result;
}

Matcher Matcher::operator-(const std::pair<Char, Char>& _range) {
    Matcher result = *this;
    std::pair<int, int> range{ _range.first.toCodepoint(), _range.second.toCodepoint() };
    auto newEnd = std::remove_if(result.codepointRanges.begin(),
        result.codepointRanges.end(), [&](const std::pair<int, int>& existingRange) {
            return existingRange.first <= range.second && range.first <= existingRange.second;
        });
    result.codepointRanges.erase(newEnd, result.codepointRanges.end());
    return result;
}

Matcher& Matcher::operator-=(const Char& c) {
    if (type == Type::Literal) {
        // This situation shouldn't be happened.
        std::cerr << "Internal Error!\n";
        abort();
    }

    std::pair<int, int> p1{ 0, c.toCodepoint() - 1 };
    std::pair<int, int> p2{ c.toCodepoint() + 1, 0x10FFFF };
    if (type == Type::AnyCharacter) {
        type = Type::CharacterClass;
        codepointRanges.emplace_back(p1);
        codepointRanges.emplace_back(p2);
    }
    else {
        // 删除 enumerations 中的字符
        auto it = std::remove(enumerations.begin(), enumerations.end(), c.toCodepoint());
        if (it != enumerations.end()) {
            enumerations.erase(it, enumerations.end());
        }

        // 拆分 codepointRanges
        remove(codepointRanges, { c.toCodepoint(), c.toCodepoint() });
    }
    return *this;
}

Matcher& Matcher::operator-=(const std::pair<Char, Char>& _range) {
    if (type == Type::CharacterClass) {
        // 删除 codepointRanges 中包含 _range 的范围
        auto range = std::make_pair(_range.first.toCodepoint(), _range.second.toCodepoint());
        
        remove(codepointRanges, range);
    }
    else if (type == Type::AnyCharacter) {
        type = Type::CharacterClass;
        std::pair<int, int> p1{ 0, _range.first.toCodepoint() - 1};
        std::pair<int, int> p2{ _range.second.toCodepoint() + 1, 0x10FFFF };
        codepointRanges.emplace_back(p1);
        codepointRanges.emplace_back(p2);
    }
    else if (type == Type::Literal) {
        // This situation shouldn't be happened.
        std::cerr << "Internal Error!\n";
        abort();
    }
    return *this;
}

bool Matcher::match(Char ch) const {
    if (type == Type::Literal) {
        return literal == ch;
    }
    if (type == Type::AnyCharacter) {
        return canBeMatchedAsAnyCharacter(ch);
    }
    int c = ch.toCodepoint();
    for (int e : enumerations) {
        if (e == c) return true;
    }
    for (auto p : codepointRanges) {
        if (p.first <= c && c <= p.second) {
            return true;
        }
    }
    return false;
}
