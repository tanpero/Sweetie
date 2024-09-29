#include "parser.hh"
#include "matcher.hh"
#include "../tests/benchmark.hpp"


int main() {

    String regex = R"((?=name|abc)|^(你好){3,5}\d|[def\d])";

    Lexer lexer(regex);
    auto tokens = lexer.tokenize();
    for (auto& i : tokens) {
        std::cout << i.toString();
    }
    std::cout << "\n";
    Parser parser(tokens);
    std::cout << parser.parse()->toString();

    return 0;
}
