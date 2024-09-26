#ifndef _AST_HH_
#define _AST_HH_


#include <string>
#include <memory>
#include <vector>
#include <utility>
#include "string.hh"

// 基类
class AST {
public:
    virtual ~AST() = default;
    virtual String toString() const = 0;
};

// 字面量节点
class Literal : public AST {
    Char value;
public:
    Literal(Char v) : value(v) {}
    String toString() const override {
        return String(value);
    }
};

// 字符类范围节点
class CharacterClassRange : public AST {
    Char start;
    Char end;
public:
    CharacterClassRange(Char s, Char e) : start(s), end(e) {}
    String toString() const override {
        return "[" + String(start) + "-" + String(end) + "]";
    }
};

// 任意字符节点
class AnyCharacter : public AST {
public:
    String toString() const override {
        return ".";
    }
};

// 锚点节点
class Anchor : public AST {
    String anchorType;
public:
    Anchor(const String& type) : anchorType(type) {}
    String toString() const override {
        return "^" + anchorType;
    }
};

// 量词节点
class Quantifier : public AST {
    std::pair<int, int> values;
    std::unique_ptr<AST> subject;
public:
    Quantifier(int min, int max, std::unique_ptr<AST> subj) : values(min, max), subject(std::move(subj)) {}
    String toString() const override {
        return subject->toString() + "{" + std::to_string(values.first) + "," + std::to_string(values.second) + "}";
    }
};

// 捕获组节点
class CapturingGroup : public AST {
    String name;
    std::unique_ptr<AST> expression;
public:
    CapturingGroup(const String& groupName, std::unique_ptr<AST> expr) : name(groupName), expression(std::move(expr)) {}
    String toString() const override {
        return "(" + name + ")" + expression->toString() + ")";
    }
};

// 非捕获组节点
class NonCapturingGroup : public AST {
    std::unique_ptr<AST> expression;
public:
    NonCapturingGroup(std::unique_ptr<AST> expr) : expression(std::move(expr)) {}
    String toString() const override {
        return "(?:" + expression->toString() + ")";
    }
};

// 先行断言节点
class LookaheadAssertion : public AST {
    std::unique_ptr<AST> expression;
    bool isNegative;
public:
    LookaheadAssertion(std::unique_ptr<AST> expr, bool negative) : expression(std::move(expr)), isNegative(negative) {}
    String toString() const override {
        return (isNegative ? "(?!" : "(?=") + expression->toString() + ")";
    }
};

// 后行断言节点
class LookbehindAssertion : public AST {
    std::unique_ptr<AST> expression;
    bool isNegative;
public:
    LookbehindAssertion(std::unique_ptr<AST> expr, bool negative) : expression(std::move(expr)), isNegative(negative) {}
    String toString() const override {
        return (isNegative ? "(?<!" : "(?<=") + expression->toString() + ")";
    }
};

// 反向引用节点
class Backreference : public AST {
    String reference;
public:
    Backreference(const String& ref) : reference(ref) {}
    String toString() const override {
        return "\\" + reference;
    }
};

// 分支选择节点
class Alternation : public AST {
    std::unique_ptr<AST> left;
    std::unique_ptr<AST> right;
public:
    Alternation(std::unique_ptr<AST> l, std::unique_ptr<AST> r) : left(std::move(l)), right(std::move(r)) {}
    String toString() const override {
        return left->toString() + "|" + right->toString();
    }
};

// Unicode属性节点
class UnicodeProperty : public AST {
    String propertyName;
    String propertyValue;
public:
    UnicodeProperty(const String& name, const String& value) : propertyName(name), propertyValue(value) {}
    String toString() const override {
        return "\\p{" + propertyName + "=" + propertyValue + "}";
    }
};

// 特殊序列节点
class SpecialSequence : public AST {
    String sequence;
public:
    SpecialSequence(const String& seq) : sequence(seq) {}
    String toString() const override {
        return "\\" + sequence;
    }
};

// 字符类节点
class CharacterClass : public AST {
    std::vector<std::unique_ptr<AST>> elements;
public:
    void addElement(std::unique_ptr<AST> element) {
        elements.push_back(std::move(element));
    }
    String toString() const override {
        String result = "[";
        for (const auto& element : elements) {
            result += element->toString();
        }
        result += "]";
        return result;
    }
};



#endif // !_AST_HH_
