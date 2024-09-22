#include "string.hh"
#include <optional>
#include <functional>

using ParserInput = String;
template <typename T>
using ParserResult = std::optional<std::pair<T, ParserInput>>;
template <typename T>
using Parser = std::function<ParserResult<T>(ParserInput)>;

Parser<Char> charParser(Char c) {
    return [c](ParserInput s) -> ParserResult<Char> {
        if (s.size() > 0 && s.front() == c) {
            ParserInput remaining = s.substr(1);
            return {{c, remaining}};
        }
        return std::nullopt;
    };
}

Parser<String> stringParser(const String& str) {
    return [str](ParserInput s) -> ParserResult<String> {
        if (s.size() >= str.size() && s.substr(0, str.size()) == str) {
            ParserInput remaining = s.substr(str.size());
            return {{str, remaining}};
        }
        return std::nullopt;
    };
}

void testParsers() {
    String input = "糖水真好喝❤";
    auto charParserResult = charParser("糖")(input);
    std::cout << charParserResult.value().first;

    auto stringParserResult = stringParser("糖水")(input);
    std::cout << "\n\n\n" << stringParserResult.value().second;
}