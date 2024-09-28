#include "parser.hh"
#include "matcher.hh"
#include "../tests/benchmark.hpp"

//void testLexer(const String& input) {
//    std::cout << "\n    ---------- Regex: " << input << "\n";
//    /*BENCHMARK([&]() {
//        Lexer lexer(input);
//        lexer.tokenize();
//        }, 100);*/
//    Lexer lexer(input);
//    std::vector<Token> tokens = lexer.tokenize();
//    int indentCount = 0;
//    for (const Token& token : tokens) {
//        if (token.type == TokenType::GroupClose)
//        {
//            indentCount--;
//        }
//        for (int i = 0; i < indentCount; i++)
//            std::cout << "    ";
//        std::cout << token.toString();
//        if (token.type == TokenType::GroupOpen
//            || token.type == TokenType::NamedCapturingGroupOpen
//            || token.type == TokenType::NonCapturingGroupOpen)
//        {
//            indentCount++;
//        }
//    }
//    std::cout << std::endl;
//}
//


//void testMatcher(const Matcher& matcher, Char ch) {
//    std::cout << "\n";
//    BENCHMARK([&]() {
//        for (int i = 0; i < pow(10, 6); i++ ) 
//            matcher.match(ch);
//        }, 100);
//}

//void testMatchers() {
//    Matcher m{ 'a', 'z' };
//    m += {0x1234, 0x2345};
//    m += {0x3456, 0x4567};
//    m += {0x5678, 0x6789};
//    m += {0x7890, 0x8901};
//    m += {0x9012, 0xFFFF};
//    testMatcher(m, 0xFFFE);
//
//    
//}

#define left_ast(C) std::move(std::make_unique<C>(
#define right_ast   ))

int main() {
    
    
    // [a-zA-Z]*(\d+)|(\d{1,3})

    // 创建一个字符类，匹配一个字母
    auto letterClass = std::make_unique<CharacterClass>(false);
    letterClass->addRange({ 'a', 'z' });
    letterClass->addRange({ 'A', 'Z' });

    auto atom1 = std::make_unique<Atom>(std::move(letterClass));
    auto quantifier1 = std::make_unique<Quantifier>(Quantifier::Type::ZeroOrMore);
    auto factor1 = std::make_unique<Factor>(std::move(atom1), std::move(quantifier1));

    auto factor2 = std::make_unique<Factor>(
        left_ast(Atom)
            
            left_ast(CapturingGroup)
                
                left_ast(Factor)

                    left_ast(Atom)
                        
                        left_ast(SpecialSequence)(R"(\d)")
                        right_ast

                    right_ast,

                    left_ast(Quantifier)(Quantifier::Type::OneOrMore)

                    right_ast

                right_ast

            right_ast
        right_ast,



        left_ast(Quantifier) right_ast
    
    );

    auto term1 = left_ast(Term) false, std::move(factor1) right_ast;
    term1->addFactor(std::move(factor2));

    std::cout << term1->toString();

    /*auto qualifier1 = std::make_unique<Quantifier>(std::move(letterClass), Quantifier::Type::ZeroOrMore);
    auto d = std::make_unique<SpecialSequence>(R"(\d)");
    auto qualifier2 = std::make_unique<Quantifier>(std::move(d), Quantifier::Type::OneOrMore);

    auto capturingGroup1 = std::make_unique<CapturingGroup>(std::move(qualifier2));*/



    return 0;
}
