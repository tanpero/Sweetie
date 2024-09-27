#ifndef _AST_HH_
#define _AST_HH_

#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <set>
#include "string.hh"

// 基类
class AST {
public:
    virtual ~AST() = default;
    virtual String toString() const = 0;
};

// 字面量节点
class Literal : public AST {
protected:
    Char value;
public:
    Literal(Char v) : value(v) {}
    virtual String toString() const override = 0;
};

// 字符类范围节点
class CharacterClass : public AST {
protected:
    std::vector<std::pair<Char, Char>> ranges;
    std::set<Char> chars;
public:
    CharacterClass() {}
    void addRange(const std::pair<Char, Char>& range);
    void addChar(const Char& ch);
    virtual String toString() const override = 0;
};

// 任意字符节点
class AnyCharacter : public AST {
public:
    virtual String toString() const override = 0;
};

// 锚点节点
class Anchor : public AST {
public:
    enum class Type { Begin, End };
protected:
    Type anchorType;
public:
    Anchor(Type type) : anchorType(type) {}
    virtual String toString() const override = 0;
};

// 量词节点
class Quantifier : public AST {
protected:
    std::pair<int, int> values;
    std::unique_ptr<AST> subject;
public:
    Quantifier(int min, int max, std::unique_ptr<AST> subj);
    virtual String toString() const override = 0;
};

// 捕获组节点
class CapturingGroup : public AST {
protected:
    int id;
    std::unique_ptr<AST> expression;
public:
    CapturingGroup(int id, std::unique_ptr<AST> expr);
    virtual String toString() const override = 0;
};

// 命名捕获组节点
class NamedCapturingGroup : public AST {
protected:
    String name;
    std::unique_ptr<AST> expression;
public:
    NamedCapturingGroup(const String& groupName, std::unique_ptr<AST> expr);
    virtual String toString() const override = 0;
};

// 非捕获组节点
class NonCapturingGroup : public AST {
protected:
    std::unique_ptr<AST> expression;
public:
    NonCapturingGroup(std::unique_ptr<AST> expr);
    virtual String toString() const override = 0;
};

// 先行断言节点
class LookaheadAssertion : public AST {
protected:
    std::unique_ptr<AST> expression;
    bool isNegative;
public:
    LookaheadAssertion(std::unique_ptr<AST> expr, bool negative);
    virtual String toString() const override = 0;
};

// 后行断言节点
class LookbehindAssertion : public AST {
protected:
    std::unique_ptr<AST> expression;
    bool isNegative;
public:
    LookbehindAssertion(std::unique_ptr<AST> expr, bool negative);
    virtual String toString() const override = 0;
};

// 反向引用节点
class Backreference : public AST {
protected:
    String reference;
public:
    Backreference(const String& ref);
    virtual String toString() const override = 0;
};

// 分支选择节点
class Alternation : public AST {
protected:
    std::unique_ptr<AST> left;
    std::unique_ptr<AST> right;
public:
    Alternation(std::unique_ptr<AST> l, std::unique_ptr<AST> r);
    virtual String toString() const override = 0;
};

// Unicode属性节点
class UnicodeProperty : public AST {
protected:
    String propertyName;
    String propertyValue;
public:
    UnicodeProperty(const String& name, const String& value);
    virtual String toString() const override = 0;
};

// 特殊序列节点
class SpecialSequence : public AST {
protected:
    String sequence;
public:
    SpecialSequence(const String& seq);
    virtual String toString() const override = 0;
};

#endif // !_AST_HH_
