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
    CharacterClass(bool isNegative);
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
public:
    enum class Type
    {
        OneOrMore, ZeroOrMore, ZeroOrOne, Designated, Once
    };
    Type type;
    Quantifier();
    Quantifier(int min, int max);
    Quantifier(Type type);
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

class Atom : public AST {
protected:
    std::unique_ptr<AST> atom;
public:
    Atom(std::unique_ptr<AST> atom_);
    virtual String toString() const;
};

// 匹配因子：原子（及其量词）或断言
class Factor : public AST {
protected:
    std::unique_ptr<AST> assertion;
    std::pair<std::unique_ptr<AST>, std::unique_ptr<AST>> atom_quantifier;
public:
    enum class Type
    {
        Atom_Quantifier, Assertion
    };
    Type type;
    Factor(std::unique_ptr<AST> assertion);
    Factor(std::unique_ptr<AST> atom, std::unique_ptr<AST> quantifier);
    virtual String toString() const;
};


// 匹配项：匹配因子的序列
class Term : public AST {
protected:
    bool hasBeginAnchor;
    bool hasEndAnchor;
    std::unique_ptr<AST> factor_;
    std::vector<std::unique_ptr<AST>> factors;
public:
    Term(bool _beginAnchor, std::unique_ptr<AST> factor0);
    void addFactor(std::unique_ptr<AST> factor);
    void setEndAnchor();
    virtual String toString() const;
};

// 匹配式：各可选匹配项的并集
class Expression : public AST {
protected:
    std::vector<std::unique_ptr<AST>> terms;
    std::unique_ptr<AST> term_;
public:
    Expression(std::unique_ptr<AST> term0);
    void addTerm(std::unique_ptr<AST> term);
    virtual String toString() const;
};

// 正则表达式：最外层的匹配式
class Regex : public AST {
protected:
    std::unique_ptr<AST> expression;
public:
    Regex(std::unique_ptr<AST> expr);
    virtual String toString() const;
};


#endif // !_AST_HH_
