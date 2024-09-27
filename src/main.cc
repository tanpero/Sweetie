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


int main() {
    auto ast1 = std::make_unique<Literal>("a");
    auto ast2 = std::make_unique<NamedCapturingGroup>("name", std::move(ast1));
    std::cout << ast2->toString() << "\n";
    //testMatchers();
    // Email
    //testLexer(R"((?:[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*|"(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21\x23-\x5b\x5d-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])*")@(?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\[(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?|[a-z0-9-]*[a-z0-9]:(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21-\x5a\x53-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])+)\]))");
    //testLexer(R"(abc[a-z\p{Script=Greek}])");
    // Url
    //testLexer(R"(([a-z]([a-z]|\d|\+|-|\.)*):(\/\/(((([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(%[\da-f]{2})|[!\$&'\(\)\*\+,;=]|:)*@)?((\[(|(v[\da-f]{1,}\.(([a-z]|\d|-|\.|_|~)|[!\$&'\(\)\*\+,;=]|:)+))\])|((\d|[1-9]\d|1\d\d|2[0-4]\d|25[0-5])\.(\d|[1-9]\d|1\d\d|2[0-4]\d|25[0-5])\.(\d|[1-9]\d|1\d\d|2[0-4]\d|25[0-5])\.(\d|[1-9]\d|1\d\d|2[0-4]\d|25[0-5]))|(([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(%[\da-f]{2})|[!\$&'\(\)\*\+,;=])*)(:\d*)?)(\/(([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(%[\da-f]{2})|[!\$&'\(\)\*\+,;=]|:|@)*)*|(\/((([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(%[\da-f]{2})|[!\$&'\(\)\*\+,;=]|:|@)+(\/(([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(%[\da-f]{2})|[!\$&'\(\)\*\+,;=]|:|@)*)*)?)|((([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(%[\da-f]{2})|[!\$&'\(\)\*\+,;=]|:|@)+(\/(([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(%[\da-f]{2})|[!\$&'\(\)\*\+,;=]|:|@)*)*)|((([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(%[\da-f]{2})|[!\$&'\(\)\*\+,;=]|:|@)){0})(\?((([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(%[\da-f]{2})|[!\$&'\(\)\*\+,;=]|:|@)|[\uE000-\uF8FF]|\/|\?)*)?(\#((([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(%[\da-f]{2})|[!\$&'\(\)\*\+,;=]|:|@)|\/|\?)*)?)");
    return 0;
}
