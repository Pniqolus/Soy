#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "Parser.hpp"

class Compiler
{
private:
	Parser parser;
	
public:
	void Parse();
};

#endif /* COMPILER_HPP */