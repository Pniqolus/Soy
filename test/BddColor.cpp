#include "BddColor.hpp"

#include <ostream>

//------------------------------------------------------------------------------

namespace {

struct TerminalColor
{
	virtual ~TerminalColor() = default;
};

struct NoColor : TerminalColor
{

};

}

//------------------------------------------------------------------------------ Constructor(s)

Color::Color(Code code) : _code(code) {}

//------------------------------------------------------------------------------ Move Constructor

Color::Color(Color&& other) : _code(other._code) {}

//------------------------------------------------------------------------------ Copy Constructor

Color& Color::operator= (Color&& other) noexcept
{
	_code = other._code;
	return *this;
}

//------------------------------------------------------------------------------ Destructor

Color::~Color() {}

//------------------------------------------------------------------------------

std::ostream& operator<< (std::ostream& out, const Color&) { return out; }

//------------------------------------------------------------------------------