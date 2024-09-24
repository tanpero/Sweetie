#include "parser.hh"


int main() {
    String input = R"(你好，{2,3}abc*^\\$)";
    Lexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();
    for (const Token& token : tokens) {
        std::cout << token.toString();
    }

    return 0;
}