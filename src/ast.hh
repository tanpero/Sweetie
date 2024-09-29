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
    void negative();
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
    bool isPositive;
public:
    LookaheadAssertion(std::unique_ptr<AST> expr, bool positive);
    virtual String toString() const;
};

// 后行断言节点
class LookbehindAssertion : public AST {
protected:
    std::unique_ptr<AST> expression;
    bool isPositive;
public:
    LookbehindAssertion(std::unique_ptr<AST> expr, bool positive);
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

// Unicode属性节点
class UnicodeProperty : public AST {
protected:
    String propertyName;
    String propertyValue;
public:
    UnicodeProperty(const String& name, const String& value);
    virtual String toString() const;
};

enum class SpecialSequenceType
{
    r, n, f, v, t, s, S, w, W, d, D, b, B
};

SpecialSequenceType translateSpecialSequence(const String& seq);

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
    Term();
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

template<typename T, typename... Args>
std::unique_ptr<T> ast(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}


//const auto emptyTerm = ast<Term>(
//    false,
//    ast<Factor>(
//        ast<Atom>(
//            ast<Literal>("")
//        ),
//        ast<Quantifier>()
//    )
//);


#endif // !_AST_HH_
