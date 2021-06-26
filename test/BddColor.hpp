#ifndef BDD_COLOR_HPP
#define BDD_COLOR_HPP

#include <iosfwd>

struct Color
{
public:
	enum Code
	{
		None = 0,

		White,
		Red,
		Green,
		Blue,
		Cyan,
		Yellow,
		Grey,

		Bright = 0x10,

		BrightRed    = Bright | Red,
		BrightGreen  = Bright | Green,
		LightGrey    = Bright | Grey,
		BrightWhite  = Bright | White,
		BrightYellow = Bright | Yellow,

		// By intention
		FileName              = LightGrey,
		Warning               = BrightYellow,
		ResultError           = BrightRed,
		ResultSuccess         = BrightGreen,
		ResultExpectedFailure = Warning,

		Error   = BrightRed,
		Success = Green,

		OriginalExpression      = Cyan,
		ReconstructedExpression = BrightYellow,

		SecondaryText = LightGrey,
		Headers       = White
	};

private:
	Code _code;

public:
	Color (Code code);
	Color (Color&& other) noexcept;
	Color& operator= (Color&& other) noexcept;
	
	~Color ();

private:
	friend std::ostream& operator << (std::ostream& os, Color const&);
};

#endif /* BDD_COLOR_HPP */