#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include "Types.hpp"
#include "String.hpp"
#include "Container.hpp"

//------------------------------------------------------------------------------

enum SymbolType { TERMINAL, NON_TERMINAL };

struct Symbol
{
	u32 id;
	str label;
};

struct TerminalSymbol : Symbol
{
	str value;
};

struct NonTerminalSymbol : Symbol
{
	using Rule = vector<Symbol>;
	vector<Rule> rules;
};

const Symbol epsilon = { .id = 0, .label = "epsilon" };

//------------------------------------------------------------------------------

class Grammar
{
private:
	vector<NonTerminalSymbol> nonTerminalSymbols;

public:
	Grammar(const str& filename);
	
private:
};

//------------------------------------------------------------------------------

#endif /* GRAMMAR_HPP */