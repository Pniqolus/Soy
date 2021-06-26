#ifndef STRING_HPP
#define STRING_HPP

#include <algorithm> 
#include <cctype>
#include <locale>

static inline void ltrim(std::string &s)
{
	constexpr auto nonspace = [](unsigned char ch){ return !std::isspace(ch); };
    
	auto begin = s.begin();
	auto end   = std::find_if (s.begin(), s.end(), nonspace);
	s.erase (begin, end);
}

static inline void rtrim(std::string &s)
{
	constexpr auto nonspace = [](unsigned char ch){ return !std::isspace(ch); };
	
	auto begin = std::find_if (s.rbegin(), s.rend(), nonspace).base();
	auto end   = s.end();
    s.erase (begin, end);
}

static inline void trim(std::string &s) { ltrim(s); rtrim(s); }

static inline std::string ltrim_copy(std::string s) { ltrim(s); return s; }
static inline std::string rtrim_copy(std::string s) { rtrim(s); return s; }
static inline std::string trim_copy(std::string s)  { trim(s);  return s; }

#endif /* STRING_HPP */