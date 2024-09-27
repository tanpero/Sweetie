#ifndef _MATCHER_HH_
#define _MATCHER_HH_

#include <tuple>
#include <vector>
#include <ranges>
#include <variant>
#include "string.hh"

bool canBeMatchedAsAnyCharacter(Char c) {
	return c != '\r' && c != '\n' &&
		c != fromCodepoint(0x2028)[0] && c != fromCodepoint(0x2029)[0];
}

struct Matcher {

    enum class Type
    {
        Literal,
        CharacterClass,
        AnyCharacter
    };

    Type type;

    Char literal;

	std::vector<int> enumerations;
	std::vector<std::pair<int, int>> codepointRanges;

    Matcher() {
        type = Type::AnyCharacter;
    }

    Matcher(const Char& c) {
        literal = c;
        type = Type::Literal;
    }

    // 构造函数，接受两个Char对象，表示一个范围
    Matcher(const Char& start, const Char& end) {
        type = Type::CharacterClass;
        codepointRanges.emplace_back(start.toCodepoint(), end.toCodepoint());
    }

    Matcher(const Matcher& other)
        : type(other.type) {
        if (type == Type::CharacterClass)
        {
            enumerations = other.enumerations;
            codepointRanges = other.codepointRanges;
        }
        else if (type == Type::Literal)
        {
            literal = other.literal;
        }
    }

    Matcher operator+(const Char& c) {
        Matcher result = *this;
        result.type = Type::CharacterClass;
        result.enumerations.emplace_back(c.toCodepoint());
        return result;
    }

    Matcher operator+(const std::pair<Char, Char>& range) {
        Matcher result = *this;
        result.type = Type::CharacterClass;
        std::pair<int, int> p{ range.first.toCodepoint(), range.second.toCodepoint() };
        result.codepointRanges.emplace_back(p);
        return result;
    }

    Matcher& operator+=(const Char& c) {
        if (type == Type::Literal)
        {
            type = Type::CharacterClass;
            enumerations.emplace_back(literal.toCodepoint());
            literal == "";
        }
        type = Type::CharacterClass;
        enumerations.emplace_back(c.toCodepoint());
        return *this;
    }

    Matcher& operator+=(const std::pair<Char, Char>& range) {
        if (type == Type::Literal)
        {
            type = Type::CharacterClass;
            enumerations.emplace_back(literal.toCodepoint());
            literal == "";
        }
        std::pair<int, int> p{ range.first.toCodepoint(), range.second.toCodepoint() };
        codepointRanges.emplace_back(p);
        return *this;
    }

    Matcher operator-(const Char& c) {
        Matcher result = *this;
        result.enumerations.erase(std::remove(result.enumerations.begin(),
            result.enumerations.end(), c.toCodepoint()), result.enumerations.end());
        return result;
    }

    Matcher operator-(const std::pair<Char, Char>& _range) {
        Matcher result = *this;
        std::pair<int, int> range{ _range.first.toCodepoint(), _range.second.toCodepoint() };
        auto newEnd = std::remove_if(result.codepointRanges.begin(),
            result.codepointRanges.end(), [&](const std::pair<int, int>& existingRange) {
                return existingRange.first <= range.second && range.first <= existingRange.second;
            });
        result.codepointRanges.erase(newEnd, result.codepointRanges.end());
        return result;
    }

    Matcher& operator-=(const Char& c) {
        enumerations.erase(std::remove(enumerations.begin(), enumerations.end(), c.toCodepoint()), enumerations.end());
        return *this;
    }

    Matcher& operator-=(const std::pair<Char, Char>& _range) {
        std::pair<int, int> range{ _range.first.toCodepoint(), _range.second.toCodepoint() };
        auto newEnd = std::remove_if(codepointRanges.begin(), codepointRanges.end(),
            [&](const std::pair<int, int>& existingRange) {
                return existingRange.first <= range.second && range.first <= existingRange.second;
            });
        codepointRanges.erase(newEnd, codepointRanges.end());
        return *this;
    }

    void normalize() {
        if (type != Type::CharacterClass)
        {
            return;
        }
        // 第一步：移除属于已有范围的枚举值
        for (auto it = enumerations.begin(); it != enumerations.end();) {
            int codepoint = *it;
            for (auto& range : codepointRanges) {
                if (range.first <= codepoint && codepoint <= range.second) {
                    it = enumerations.erase(it); // 如果枚举值在范围内，移除枚举值
                    break;
                }
            }
            if (it == enumerations.end()) {
                break;
            }
            ++it;
        }

        // 第二步：合并有交集或首尾相接的范围
        auto merge_ranges = [](std::pair<int, int> a, std::pair<int, int> b) {
                return std::make_pair(std::min(a.first, b.first), std::max(a.second, b.second));
            };

        for (auto it = codepointRanges.begin(); it != codepointRanges.end();) {
            auto& currentRange = *it;
            auto nextIt = it;
            ++nextIt;
            while (nextIt != codepointRanges.end()) {
                auto& nextRange = *nextIt;
                if (currentRange.second >= nextRange.first - 1) { // 检查是否有交集或首尾相接
                    currentRange = merge_ranges(currentRange, nextRange); // 合并范围
                    it = codepointRanges.erase(nextIt); // 移除下一个范围
                }
                else {
                    ++it;
                    ++nextIt;
                }
            }
        }
    }
	
	bool match(Char ch) const {
        if (type == Type::Literal)
        {
            return literal == ch;
        }
        if (type == Type::AnyCharacter)
        {
            return canBeMatchedAsAnyCharacter(ch);
        }
        int c = ch.toCodepoint();
		for (int e : enumerations) {
			if (e == c) return true;
		}
		for (auto p : codepointRanges) {
			int codepoint = c;
			if (p.first <= codepoint && codepoint <= p.second)
			{
				return true;
			}
		}
		return false;
	}
};

#endif // !_MATCHER_HH_
