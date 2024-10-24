#include <functional>
#include <cstdint>
#include <iostream>
#include <string>
#include "ascii.hh"

// 定义 CharPredicate 类型为函数指针
using CharPredicate = std::function<bool(char32_t)>;


constexpr int UNASSIGNED = 0; // Cn normative
constexpr int UPPERCASE_LETTER = 1; // Lu normative
constexpr int LOWERCASE_LETTER = 2; // Ll normative
constexpr int TITLECASE_LETTER = 3; // Lt normative
constexpr int MODIFIER_LETTER = 4; // Lm normative
constexpr int OTHER_LETTER = 5; // Lo normative
constexpr int NON_SPACING_MARK = 6; // Mn informative
constexpr int ENCLOSING_MARK = 7; // Me informative
constexpr int COMBINING_SPACING_MARK = 8; // Mc normative
constexpr int DECIMAL_DIGIT_NUMBER = 9; // Nd normative
constexpr int LETTER_NUMBER = 10; // Nl normative
constexpr int OTHER_NUMBER = 11; // No normative
constexpr int SPACE_SEPARATOR = 12; // Zs normative
constexpr int LINE_SEPARATOR = 13; // Zl normative
constexpr int PARAGRAPH_SEPARATOR = 14; // Zp normative
constexpr int CONTROL = 15; // Cc normative
constexpr int FORMAT = 16; // Cf normative
constexpr int PRIVATE_USE = 18; // Co normative
constexpr int SURROGATE = 19; // Cs normative
constexpr int DASH_PUNCTUATION = 20; // Pd informative
constexpr int START_PUNCTUATION = 21; // Ps informative
constexpr int END_PUNCTUATION = 22; // Pe informative
constexpr int CONNECTOR_PUNCTUATION = 23; // Pc informative
constexpr int OTHER_PUNCTUATION = 24; // Po informative
constexpr int MATH_SYMBOL = 25; // Sm informative
constexpr int CURRENCY_SYMBOL = 26; // Sc informative
constexpr int MODIFIER_SYMBOL = 27; // Sk informative
constexpr int OTHER_SYMBOL = 28; // So informative
constexpr int INITIAL_QUOTE_PUNCTUATION = 29; // Pi informative
constexpr int FINAL_QUOTE_PUNCTUATION = 30; // Pf informative
constexpr int GENERAL_CATEGORY_COUNT = 31; // sentinel value

// category函数
CharPredicate category(int typeMask) {
    return [typeMask](char32_t ch) {
        return (typeMask & (1 << ASCII::getType(static_cast<int>(ch)))) != 0;
    };
}

// range函数
CharPredicate range(char32_t lower, char32_t upper) {
    return [lower, upper](char32_t ch) {
        return lower <= ch && ch <= upper;
    };
}

// 定义字符类型检测函数
CharPredicate forProperty(const std::string& name, bool caseIns) {
    if (name == "Cn") {
        return category(1 << UNASSIGNED);
    }
    else if (name == "Lu") {
        return caseIns ? category((1 << LOWERCASE_LETTER) |
            (1 << UPPERCASE_LETTER) |
            (1 << TITLECASE_LETTER))
            : category(1 << UPPERCASE_LETTER);
    }
    else if (name == "Ll") {
        return caseIns ? category((1 << LOWERCASE_LETTER) |
            (1 << UPPERCASE_LETTER) |
            (1 << TITLECASE_LETTER))
            : category(1 << LOWERCASE_LETTER);
    }
    else if (name == "Lt") {
        return caseIns ? category((1 << LOWERCASE_LETTER) |
            (1 << UPPERCASE_LETTER) |
            (1 << TITLECASE_LETTER))
            : category(1 << TITLECASE_LETTER);
    }
    else if (name == "Lm") {
        return category(1 << MODIFIER_LETTER);
    }
    else if (name == "Lo") {
        return category(1 << OTHER_LETTER);
    }
    else if (name == "Mn") {
        return category(1 << NON_SPACING_MARK);
    }
    else if (name == "Me") {
        return category(1 << ENCLOSING_MARK);
    }
    else if (name == "Mc") {
        return category(1 << COMBINING_SPACING_MARK);
    }
    else if (name == "Nd") {
        return category(1 << DECIMAL_DIGIT_NUMBER);
    }
    else if (name == "Nl") {
        return category(1 << LETTER_NUMBER);
    }
    else if (name == "No") {
        return category(1 << OTHER_NUMBER);
    }
    else if (name == "Zs") {
        return category(1 << SPACE_SEPARATOR);
    }
    else if (name == "Zl") {
        return category(1 << LINE_SEPARATOR);
    }
    else if (name == "Zp") {
        return category(1 << PARAGRAPH_SEPARATOR);
    }
    else if (name == "Cc") {
        return category(1 << CONTROL);
    }
    else if (name == "Cf") {
        return category(1 << FORMAT);
    }
    else if (name == "Co") {
        return category(1 << PRIVATE_USE);
    }
    else if (name == "Cs") {
        return category(1 << SURROGATE);
    }
    else if (name == "Pd") {
        return category(1 << DASH_PUNCTUATION);
    }
    else if (name == "Ps") {
        return category(1 << START_PUNCTUATION);
    }
    else if (name == "Pe") {
        return category(1 << END_PUNCTUATION);
    }
    else if (name == "Pc") {
        return category(1 << CONNECTOR_PUNCTUATION);
    }
    else if (name == "Po") {
        return category(1 << OTHER_PUNCTUATION);
    }
    else if (name == "Sm") {
        return category(1 << MATH_SYMBOL);
    }
    else if (name == "Sc") {
        return category(1 << CURRENCY_SYMBOL);
    }
    else if (name == "Sk") {
        return category(1 << MODIFIER_SYMBOL);
    }
    else if (name == "So") {
        return category(1 << OTHER_SYMBOL);
    }
    else if (name == "Pi") {
        return category(1 << INITIAL_QUOTE_PUNCTUATION);
    }
    else if (name == "Pf") {
        return category(1 << FINAL_QUOTE_PUNCTUATION);
    }
    else if (name == "L") {
        return category((1 << UPPERCASE_LETTER) |
            (1 << LOWERCASE_LETTER) |
            (1 << TITLECASE_LETTER) |
            (1 << MODIFIER_LETTER) |
            (1 << OTHER_LETTER));
    }
    else if (name == "M") {
        return category((1 << NON_SPACING_MARK) |
            (1 << ENCLOSING_MARK) |
            (1 << COMBINING_SPACING_MARK));
    }
    else if (name == "N") {
        return category((1 << DECIMAL_DIGIT_NUMBER) |
            (1 << LETTER_NUMBER) |
            (1 << OTHER_NUMBER));
    }
    else if (name == "Z") {
        return category((1 << SPACE_SEPARATOR) |
            (1 << LINE_SEPARATOR) |
            (1 << PARAGRAPH_SEPARATOR));
    }
    else if (name == "C") {
        return category((1 << CONTROL) |
            (1 << FORMAT) |
            (1 << PRIVATE_USE) |
            (1 << SURROGATE) |
            (1 << UNASSIGNED));
    }
    else if (name == "P") {
        return category((1 << DASH_PUNCTUATION) |
            (1 << START_PUNCTUATION) |
            (1 << END_PUNCTUATION) |
            (1 << CONNECTOR_PUNCTUATION) |
            (1 << OTHER_PUNCTUATION) |
            (1 << INITIAL_QUOTE_PUNCTUATION) |
            (1 << FINAL_QUOTE_PUNCTUATION));
    }
    else if (name == "S") {
        return category((1 << MATH_SYMBOL) |
            (1 << CURRENCY_SYMBOL) |
            (1 << MODIFIER_SYMBOL) |
            (1 << OTHER_SYMBOL));
    }
    else if (name == "LC") {
        return category((1 << UPPERCASE_LETTER) |
            (1 << LOWERCASE_LETTER) |
            (1 << TITLECASE_LETTER));
    }
    else if (name == "LD") {
        return category((1 << UPPERCASE_LETTER) |
            (1 << LOWERCASE_LETTER) |
            (1 << TITLECASE_LETTER) |
            (1 << MODIFIER_LETTER) |
            (1 << OTHER_LETTER) |
            (1 << DECIMAL_DIGIT_NUMBER));
    }
    else if (name == "L1") {
        return range(0x00, 0xFF); // Latin-1
    }
    else {
        throw std::invalid_argument("Unknown property name");
    }
}

