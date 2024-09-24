#include "parser.hh"


int main() {
    String input = R"(你好，糖糖{1,5}abc*\u1234^\\$)";
    Lexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();
    for (const Token& token : tokens) {
        std::cout << token.toString();
    }

    return 0;
}