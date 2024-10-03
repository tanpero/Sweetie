#ifndef _VM_HH_
#define _VM_HH_

#include <variant>
#include "parser.hh"
#include "matcher.hh"

enum class ByteCode
{
	Jump, Split, Match, Repeat, Anchor, Capture, Assert, Ref, Accept, Halt
};

struct Instruction
{
	ByteCode code;

	size_t operand_1;
	size_t operand_2;

	std::unique_ptr<Matcher> matcher;
	bool isNegative;
	Instruction(ByteCode _code, bool _isNegative = false)
		: code(_code), isNegative(_isNegative) {
		matcher = nullptr;
	}
	Instruction(std::unique_ptr<Matcher> _m) {
		code = ByteCode::Match;
		matcher = std::move(_m);
		isNegative = matcher->isNegative;
	}
	Instruction(const Instruction& i)
		: code(i.code), isNegative(i.isNegative) {
		if (i.matcher) {
			matcher = std::make_unique<Matcher>(*i.matcher);
		}
		else {
			matcher = nullptr;
		}
	}
};


using Program = std::vector<Instruction>;

class VM
{
	Program program;
	size_t positionOfText;
	size_t positionOfInstructions;
	String text;
public:
	VM(Program _program);
	bool test(String _text);

private:
	bool advance();
	Char ch();
	void keep();
	bool match1(Char ch);
	bool match0or1();
	bool match1orMore();
	bool match0orMore();
	bool match_or_(int m, int n);
};




#endif // !_VM_HH_
