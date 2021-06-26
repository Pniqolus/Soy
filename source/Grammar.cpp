#include "Grammar.hpp"

#include <iostream>

#include <cctype>
#include <fstream>
#include <regex>
#include <functional>
#include <algorithm>

using std::ifstream;
using std::getline;

using std::regex;
using std::regex_search;

using std::function;

//------------------------------------------------------------------------------
//
// GRAMMAR   -> RULE GRAMMAR | <epsilon>
// RULE      -> NTS '->' EXPR ';'
// EXPR      -> SYMBOL EXPR | ALTERNATE
// ALTERNATE -> '|' EXPR | <epsilon>
// SYMBOL    -> NTS | TS
// NTS       -> NAME
// TS        -> REGEX | TOKEN | STRING | KEYWORD
// 
// NAME      -> [a-zA-Z] NAME_ | '-' NAME_
// NAME_     -> [a-zA-Z] NAME_ | '-' NAME_ | [0-9] NAME_ | <epsilon>
// REGEX     -> '/' REGEX_ '/'
// REGEX_    -> [^\\\/] REGEX_ | '\\' . REGEX_ | <epsilon>
// TOKEN     -> '\'' TOKEN_ '\''
// TOKEN_    -> [^\\'\s] TOKEN_ | '\\' [^\s] TOKEN_ | <epsilon>
// STRING    -> '"' STRING_ '"'
// STRING_   -> [^\\"] STRING_ | '\\' . STRING_ | <epsilon>
// KEYWORD   -> '<' [^>] '>'
//
//------------------------------------------------------------------------------

enum MetaTerminalSymbols : size_t
{
	TS_WHITESPACE,
	TS_LETTER,
	TS_NUMBER,
	TS_MINUS,
	TS_UNDERSCORE,
	TS_LEFT_ANGLE_BRACKET,
	TS_RIGHT_ANGLE_BRACKET,
	TS_LEFT_SQUARE_BRACKET,
	TS_RIGHT_SQUARE_BRACKET,
	TS_SINGLE_QUOTE,
	TS_DOUBLE_QUOTE,
	TS_BACKSLASH,
	TS_FORWARD_SLASH,
	TS_CARET,
	TS_PIPE,
	TS_PERIOD,
	TS_SEMICOLON,
	TS_OTHER,
	
	TS_END_OF_STREAM,
	
	TS_SYMBOL_COUNT
};

enum MetaNonTerminalSymbols : size_t
{
	NTS_GRAMMAR,
	NTS_RULE,
	NTS_EXPRESSION,
	NTS_ALTERNATE,
	NTS_SYMBOL,
	NTS_NONTERMINAL,
	NTS_TERMINAL,
	
	NTS_NAME,
	NTS_NAME_CONT,
	NTS_REGEX,
	NTS_REGEX_CONT,
	NTS_TOKEN,
	NTS_TOKEN_CONT,
	NTS_STRING,
	NTS_STRING_CONT,
	NTS_KEYWORD,
	NTS_WHITESPACE,
	NTS_NON_WHITESPACE,
	
	NTS_ANY,
	
	NTS_SYMBOL_COUNT
};

static array<vector<str>, 2> MetaSymbolNames
{{
	{
		"whitespace", "letter", "number", "minus", "underscore",
		"left angle bracket", "right angle bracket",
		"left square bracket", "right square bracket",
		"single quote", "double quote", "backslash", "forward slash", "caret",
		"pipe", "period", "semicolon", "other", "end of stream"
	},
	{
		"grammar", "rule", "expression", "alternate", "symbol", "nonterminal",
		"terminal", "name", "name continued", "regex", "regex continued",
		"token", "token continued", "string", "string continued", "keyword",
		"whitespace", "any"
	}
}};

//------------------------------------------------------------------------------

struct MetaSymbol
{
	SymbolType type;
	union {
		size_t                 val ;
		MetaTerminalSymbols    ts  ;
		MetaNonTerminalSymbols nts ;
	};
	
	bool operator == (const MetaSymbol& s) const noexcept
	{
		return (type == s.type) && (val == s.val);
	}
};

//------------------------------------------------------------------------------

using MetaSymbolStack = stack<MetaSymbol>;
using MetaProduction  = function<void(MetaSymbolStack&, cstr, MetaTerminalSymbols)>;

//------------------------------------------------------------------------------

static const MetaProduction MetaRules[]
{
	/* Epsilon */
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 0 */ {
		ss.pop();
	},
	
	/* Grammar */
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 1 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_GRAMMAR });
		ss.push({ NON_TERMINAL, NTS_RULE    });
	},
	
	/* Rule */
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 2 */ {
		ss.pop();
		ss.push({ TERMINAL,     TS_SEMICOLON           });
		ss.push({ NON_TERMINAL, NTS_EXPRESSION         });
		ss.push({ TERMINAL,     TS_RIGHT_ANGLE_BRACKET });
		ss.push({ TERMINAL,     TS_MINUS               });
		ss.push({ NON_TERMINAL, NTS_NONTERMINAL        });
	},
	
	/* Expression */
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 3 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_EXPRESSION });
		ss.push({ NON_TERMINAL, NTS_SYMBOL     });
	},
	
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 4 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_ALTERNATE });
	},
	
	/* Alternate */
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 5 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_EXPRESSION });
		ss.push({ TERMINAL,     TS_PIPE        });
	},
	
	/* Symbol */
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 6 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_NONTERMINAL });
	},
	
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 7 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_TERMINAL });		
	},
	
	/* Non-Terminal */
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 8 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_NAME });
	},

	/* Terminal */
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 9 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_KEYWORD });
	},
	
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 10 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_TOKEN });
	},
	
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 11 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_STRING });
	},
	
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 12 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_REGEX });
	},
	
	/* Name */
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 13 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_NAME_CONT });
		ss.push({ TERMINAL,     TS_LETTER     });
	},
	
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 14 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_NAME_CONT });
		ss.push({ TERMINAL,     TS_UNDERSCORE });
	},
	
	/* Name (Continued) */
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 15 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_NAME_CONT });
		ss.push({ TERMINAL,     TS_LETTER     });
	},
	
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 16 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_NAME_CONT });
		ss.push({ TERMINAL,     TS_UNDERSCORE });
	},
	
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 17 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_NAME_CONT });
		ss.push({ TERMINAL,     TS_NUMBER     });
	},
	
	/* Regex */
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 18 */ {
		ss.pop();
		ss.push({ TERMINAL,     TS_FORWARD_SLASH });
		ss.push({ NON_TERMINAL, NTS_REGEX_CONT   });
		ss.push({ TERMINAL,     TS_FORWARD_SLASH });
	},
	
	/* Regex (Continued) */
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 19 */ {
		ss.push({ TERMINAL, ts });
	},
	
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 20 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_REGEX_CONT });
		ss.push({ NON_TERMINAL, NTS_ANY        });
		ss.push({ TERMINAL,     TS_BACKSLASH   });
	},
	
	/* Token */
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 21 */ {
		ss.pop();
		ss.push({ TERMINAL,     TS_SINGLE_QUOTE });
		ss.push({ NON_TERMINAL, NTS_TOKEN_CONT  });
		ss.push({ TERMINAL,     TS_SINGLE_QUOTE });
	},
	
	/* Token (Continued) */
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 22 */ {
		ss.push({ TERMINAL, ts });
	},
	
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 23 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_TOKEN_CONT     });
		ss.push({ NON_TERMINAL, NTS_NON_WHITESPACE });
		ss.push({ TERMINAL,     TS_BACKSLASH       });
	},
	
	/* String */
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 24 */ {
		ss.pop();
		ss.push({ TERMINAL,     TS_DOUBLE_QUOTE });
		ss.push({ NON_TERMINAL, NTS_STRING_CONT });
		ss.push({ TERMINAL,     TS_DOUBLE_QUOTE });
	},
	
	/* String (Continued) */
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 25 */ {
		ss.push({ TERMINAL, ts });
	},
	
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* 26 */ {
		ss.pop();
		ss.push({ NON_TERMINAL, NTS_STRING_CONT });
		ss.push({ NON_TERMINAL, NTS_ANY         });
		ss.push({ TERMINAL,     TS_BACKSLASH    });
	},
	
	/* Whitespace */
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* n - 2 */ {
		ss.push({ NON_TERMINAL, NTS_WHITESPACE });
		ss.push({ TERMINAL,     TS_WHITESPACE  });
	},
	
	/* Any */
	[](MetaSymbolStack& ss, cstr c, MetaTerminalSymbols ts) /* n - 1 */ {
		ss.pop();
		ss.push({ TERMINAL, ts });
	}
};

constexpr auto MetaRuleCount = sizeof(MetaRules) / sizeof(MetaProduction);

//------------------------------------------------------------------------------

using ParsingTable = array<array<size_t, TS_SYMBOL_COUNT>, NTS_SYMBOL_COUNT>;

constexpr auto NA  = size_max;
constexpr auto _WS = MetaRuleCount - 2;
constexpr auto ACC = MetaRuleCount - 1;

static ParsingTable MetaParsingTable
{{
//                      [\s]  [a-Z]  [0-9]  '-'  '_'  '<'  '>'  '['  ']'  '\''  '"'  '\\'  '/'  '^'  '|'  '.'  ';'  other
/* GRAMMER        */ {  _WS,     1,     0,   0,   1,   0,   0,   0,   0,    0,   0,    0,   0,   0,   0,   0,   0,     0 },
/* RULE           */ {  _WS,     2,     0,   0,   2,   0,   0,   0,   0,    0,   0,    0,   0,   0,   0,   0,   0,     0 },
/* EXPRESSION     */ {  _WS,     3,     0,   0,   3,   0,   0,   0,   0,    3,   3,    0,   3,   0,   4,   0,   0,     0 },
/* ALTERNATE      */ {  _WS,     0,     0,   0,   0,   0,   0,   0,   0,    0,   0,    0,   0,   0,   5,   0,   0,     0 },

/* SYMBOL         */ {   NA,     6,    NA,  NA,   6,   7,  NA,  NA,  NA,    7,   7,   NA,   7,  NA,  NA,  NA,  NA,    NA },
/* NONTERMINAL    */ {   NA,     8,    NA,  NA,   8,  NA,  NA,  NA,  NA,   NA,  NA,   NA,  NA,  NA,  NA,  NA,  NA,    NA },
/* TERMINAL       */ {   NA,    NA,    NA,  NA,  NA,   9,  NA,  NA,  NA,   10,  11,   NA,  12,  NA,  NA,  NA,  NA,    NA },

/* NAME           */ {   NA,    13,    NA,  NA,  14,  NA,  NA,  NA,  NA,   NA,  NA,   NA,  NA,  NA,  NA,  NA,  NA,    NA },
/* NAME_          */ {  _WS,    15,    17,   0,  16,   0,   0,   0,   0,    0,   0,    0,   0,   0,   0,   0,   0,     0 },
/* REGEX          */ {   NA,    NA,    NA,  NA,  NA,  NA,  NA,  NA,  NA,   NA,  NA,   NA,  18,  NA,  NA,  NA,  NA,    NA },
/* REGEX_         */ {   19,    19,    19,  19,  19,  19,  19,  19,  19,   19,  19,   20,   0,  19,  19,  19,  19,    19 },
/* TOKEN          */ {   NA,    NA,    NA,  NA,  NA,  NA,  NA,  NA,  NA,   21,  NA,   NA,  NA,  NA,  NA,  NA,  NA,    NA },
/* TOKEN_         */ {    0,    22,    22,  22,  22,  22,  22,  22,  22,    0,  22,   23,  22,  22,  22,  22,  22,    22 },
/* STRING         */ {   NA,    NA,    NA,  NA,  NA,  NA,  NA,  NA,  NA,   NA,  24,   NA,  NA,  NA,  NA,  NA,  NA,    NA },
/* STRING_        */ {   25,    25,    25,  25,  25,  25,  25,  25,  25,   25,   0,   26,  25,  25,  25,  25,  25,    25 },
/* KEYWORD        */ {   NA,    NA,    NA,  NA,  NA,  NA,  NA,  NA,  NA,   NA,  NA,   NA,  NA,  NA,  NA,  NA,  NA,    NA },

/* WHITESPACE     */ {  _WS,     0,     0,   0,   0,   0,   0,   0,   0,    0,   0,    0,   0,   0,   0,   0,   0,     0 },
/* NON-WHITESPACE */ {   NA,     0,     0,   0,   0,   0,   0,   0,   0,    0,   0,    0,   0,   0,   0,   0,   0,     0 },
/* ANY            */ {  ACC,   ACC,   ACC, ACC, ACC, ACC, ACC, ACC, ACC,  ACC, ACC,  ACC, ACC, ACC, ACC, ACC, ACC,   ACC },
}};

//------------------------------------------------------------------------------

MetaSymbol MetaLexer(char character)
{
	if (isspace(character)) return { TERMINAL, TS_WHITESPACE };
	if (isalpha(character)) return { TERMINAL, TS_LETTER     };
	if (isdigit(character)) return { TERMINAL, TS_NUMBER     };
	
	switch(character)
	{
		case '-':  return { TERMINAL, TS_MINUS                };
		case '_':  return { TERMINAL, TS_UNDERSCORE           };
		case '<':  return { TERMINAL, TS_LEFT_ANGLE_BRACKET   };
		case '>':  return { TERMINAL, TS_RIGHT_ANGLE_BRACKET  };
		case '[':  return { TERMINAL, TS_LEFT_SQUARE_BRACKET  };
		case ']':  return { TERMINAL, TS_RIGHT_SQUARE_BRACKET };
		case '\'': return { TERMINAL, TS_SINGLE_QUOTE         };
		case '"':  return { TERMINAL, TS_DOUBLE_QUOTE         };
		case '\\': return { TERMINAL, TS_BACKSLASH            };
		case '/':  return { TERMINAL, TS_FORWARD_SLASH        };
		case '^':  return { TERMINAL, TS_CARET                };
		case '|':  return { TERMINAL, TS_PIPE                 };
		case '.':  return { TERMINAL, TS_PERIOD               };
		case ';':  return { TERMINAL, TS_SEMICOLON            };
	}
	
	return { TERMINAL, TS_OTHER };
}

//------------------------------------------------------------------------------

Grammar::Grammar(const str& filename)
{
	ifstream file (filename);
	
	str firstLine;
	getline(file, firstLine);
	
	str fullText = firstLine;	
	for (str line; getline(file, line);)
		fullText += (" " + line);
	
	MetaSymbolStack symbolStack;

	symbolStack.push({ TERMINAL,     TS_END_OF_STREAM });
	symbolStack.push({ NON_TERMINAL, NTS_GRAMMAR      });
	
	auto grammar_is_valid = true;
	auto cursor           = &fullText[0];
	
	while (symbolStack.size() > 0 && grammar_is_valid)
	{
		auto context = symbolStack.top();
		auto symbol  = MetaLexer(*cursor);
		
		if (symbol == context)
		{
			cursor++;
			symbolStack.pop();
		}
		else
		{
			auto rule = MetaParsingTable[context.val][symbol.val];		
		
			grammar_is_valid = (rule < MetaRuleCount);
			if (grammar_is_valid)
				MetaRules[rule](symbolStack, cursor, symbol.ts);
		}
	}
	
	std::cout << (grammar_is_valid ? "V" : "Inv") << "alid grammar." << std::endl;
}

//------------------------------------------------------------------------------
