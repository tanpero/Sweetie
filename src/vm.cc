#include "vm.hh"

VM::VM(Program _program) : program(_program), positionOfInstructions(0), positionOfText(0)
{
}

bool VM::test(String _text)
{
	text = _text;
    /*while (positionOfInstructions < program.size())
    {
        switch (program[positionOfInstructions].code)
        {
        case ByteCode::Match:
            if (match1(ch())) {
                return 
            }
            break;
        default:
            break;
        }

        positionOfInstructions++;
        advance();
    }*/
	return false;
}

bool VM::advance()
{
	return positionOfText < text.length() ? ++positionOfText : false;
}

Char VM::ch()
{
	return text[positionOfText];
}

void VM::keep()
{
	while (match1(ch()) && advance()) {}
}

bool VM::match1(Char c)
{
	return program[positionOfInstructions].matcher->match(c);
}

bool VM::match0or1()
{
	if (match1(ch()))
	{
		advance();
	}
	return true;
}

bool VM::match1orMore()
{
	if (!match1(ch()))
	{
		return false;
	}
	advance();
	keep();
	return true;
}

bool VM::match0orMore()
{
	keep();
	return true;
}

bool VM::match_or_(int m, int n)
{
    if (n == -1) {
        // n为-1时，相当于*原语，匹配0次或多次，至少m次
        int count = 0;
        size_t old_positionOfText = positionOfText;
        while (old_positionOfText < text.length() && match1(ch()) && count < m) {
            advance();
            count++;
        }
        if (count < m) {
            return false; // 没有达到最小匹配次数m
        }
        positionOfText = old_positionOfText;
        keep();
        return true;
    }
    else {
        // n为非负数时，匹配m到n次
        int count = 0;
        while (count < m && positionOfText < text.length() && match1(ch())) {
            advance();
            count++;
        }
        if (count < m) {
            return false; // 没有达到最小匹配次数m
        }
        while (count < n && positionOfText < text.length() && match1(ch())) {
            advance();
            count++;
        }
        // 回退到最后一个未满足n次但满足m次的位置
        if (count > m && count > n) {
            positionOfText -= count - n;
        }
        return true;
    }
}

