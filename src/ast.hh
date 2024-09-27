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
    virtual String toString() const;
};

// 字符类范围节点
class CharacterClass : public AST {
protected:
    std::vector<std::pair<Char, Char>> ranges;
    std::set<Char> chars;
    bool isNegative;
public:
    CharacterClass(bool isNegative) : isNegative(isNegative) {}
    void addRange(const std::pair<Char, Char>& range);
    void addChar(const Char& ch);
    virtual String toString() const;
};

// 任意字符节点
class AnyCharacter : public AST {
public:
    virtual String toString() const;
};

// 锚点节点
class Anchor : public AST {
public:
    enum class Type { Begin, End };
protected:
    Type anchorType;
public:
    Anchor(Type type) : anchorType(type) {}
    virtual String toString() const;
};

// 量词节点
class Quantifier : public AST {
protected:
    std::pair<int, int> values;
    std::unique_ptr<AST> subject;
    enum class Type
    {
        OneOrMore, ZeroOrMore, ZeroOrOne, Designated
    };
    Type type;
public:
    Quantifier(std::unique_ptr<AST> subj, int min, int max);
    Quantifier(std::unique_ptr<AST> subj, Type type);
    virtual String toString() const;
};

// 捕获组节点
class CapturingGroup : public AST {
protected:
    int id;
    std::unique_ptr<AST> expression;
public:
    CapturingGroup(std::unique_ptr<AST> expr);
    virtual String toString() const;
};

// 命名捕获组节点
class NamedCapturingGroup : public AST {
protected:
    String name;
    int id;
    std::unique_ptr<AST> expression;
public:
    NamedCapturingGroup(const String& groupName, std::unique_ptr<AST> expr);
    virtual String toString() const;
};

// 非捕获组节点
class NonCapturingGroup : public AST {
protected:
    std::unique_ptr<AST> expression;
public:
    NonCapturingGroup(std::unique_ptr<AST> expr);
    virtual String toString() const;
};

// 先行断言节点
class LookaheadAssertion : public AST {
protected:
    std::unique_ptr<AST> expression;
    bool isNegative;
public:
    LookaheadAssertion(std::unique_ptr<AST> expr, bool negative);
    virtual String toString() const;
};

// 后行断言节点
class LookbehindAssertion : public AST {
protected:
    std::unique_ptr<AST> expression;
    bool isNegative;
public:
    LookbehindAssertion(std::unique_ptr<AST> expr, bool negative);
    virtual String toString() const;
};

// 反向引用节点
class Backreference : public AST {
protected:
    String reference;
    int id;
public:
    Backreference(const String& ref);
    Backreference(int id);
    virtual String toString() const;
};

// 分支选择节点
class Alternation : public AST {
protected:
    std::unique_ptr<AST> left;
    std::unique_ptr<AST> right;
public:
    Alternation(std::unique_ptr<AST> l, std::unique_ptr<AST> r);
    virtual String toString() const;
};

// Unicode属性节点
class UnicodeProperty : public AST {
protected:
    String propertyName;
    String propertyValue;
public:
    UnicodeProperty(const String& name, const String& value);
    virtual String toString() const;
};

// 特殊序列节点
class SpecialSequence : public AST {
protected:
    enum class Type
    {
        r, n, f, v, t, s, S, w, W, d, D
    };
    Type type;
public:
    SpecialSequence(const String& seq);
    virtual String toString() const;
};

#endif // !_AST_HH_
