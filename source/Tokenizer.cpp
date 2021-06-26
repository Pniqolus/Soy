#include "Tokenizer.hpp"

//------------------------------------------------------------------------------

template <>
size_t TokenizerBase::find_first_delimiter <vector<char>> (
	str_vw text, vector<char> delimiters, size_t start)
{
	for (size_t pos = start; pos < text.length(); pos++)
		for (auto delim : delimiters)
			if (text[pos] == delim)
				return pos;
	return text.length();
}

//------------------------------------------------------------------------------

template <>
size_t TokenizerBase::find_first_delimiter <char> (
	str_vw text, char delimiter, size_t start)
{
	for (size_t pos = start; pos < text.length(); pos++)
		if (text[pos] == delimiter)
			return pos;
	return text.length();
}

//------------------------------------------------------------------------------