#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include "Types.hpp"
#include "Container.hpp"

//------------------------------------------------------------------------------

enum class TrimEmpty;
enum class KeepEmpty;

//------------------------------------------------------------------------------

struct TokenizerBase
{
protected:

	static constexpr char Default_Delim = ' ';

	template <typename Delimiters>
	size_t find_first_delimiter(
		str_vw text, Delimiters delimiters, size_t start = 0);
};

//------------------------------------------------------------------------------

template <typename EmptyCondition = TrimEmpty>
struct Tokenizer;

template <>
struct Tokenizer<TrimEmpty> : TokenizerBase
{
	template <typename Delimiters = decltype(Default_Delim)>
	vector<str_vw> Tokenize(str_vw text, Delimiters delimiters = Default_Delim)
	{
		size_t         pos    ;
		vector<str_vw> tokens ;

		for (size_t lastPos = 0; lastPos < text.length() + 1; lastPos = pos + 1)
		{
			pos = find_first_delimiter(text, delimiters, lastPos);
			if (pos != lastPos)
				tokens.push_back(text.substr(lastPos, pos - lastPos));
		}

		return tokens;
	}
};

//------------------------------------------------------------------------------

template <>
struct Tokenizer<KeepEmpty> : TokenizerBase
{
	template <typename Delimiters = decltype(Default_Delim)>
	vector<str_vw> Tokenize(str_vw text, Delimiters delimiters = Default_Delim)
	{
		size_t         pos    ;
		vector<str_vw> tokens ;

		for (size_t lastPos = 0; lastPos < text.length() + 1; lastPos = pos + 1)
		{
			pos = find_first_delimiter(text, delimiters, lastPos);
			tokens.push_back(text.substr(lastPos, pos - lastPos));
		}

		return tokens;
	}
};

//------------------------------------------------------------------------------

#endif /* TOKENIZER_HPP */