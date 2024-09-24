#include "parser.hh"


int main() {
    String input = R"(^埃及法老糖糖{1,5}说：𓁏\u{1304f}^\\$)";
    Lexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();
    for (const Token& token : tokens) {
        std::cout << token.toString();
    }

    return 0;
}