#ifndef _MATCHER_HH_
#define _MATCHER_HH_

#include <tuple>
#include <vector>
#include <map>
#include <ranges>
#include <variant>
#include "string.hh"

bool canBeMatchedAsAnyCharacter(Char c);

class Matcher {
public:
    enum class Type {
        Literal,
        CharacterClass,
        AnyCharacter
    };

    Matcher();
    Matcher(const Char& c);
    Matcher(const Char& start, const Char& end);
    Matcher(const Matcher& other);
    Matcher& operator+=(const Char& c);
    Matcher& operator+=(const std::pair<Char, Char>& range);
    Matcher operator+(const Char& c);
    Matcher operator+(const std::pair<Char, Char>& range);
    Matcher operator-(const Char& c);
    Matcher operator-(const std::pair<Char, Char>& _range);
    Matcher& operator-=(const Char& c);
    Matcher& operator-=(const std::pair<Char, Char>& _range);
    bool match(Char ch) const;

private:
    Type type;
    Char literal;
    std::vector<int> enumerations;
    std::vector<std::pair<int, int>> codepointRanges;
};

#endif // !_MATCHER_HH_
