#include <catch2/catch_test_macros.hpp>

#include "Tokenizer.hpp"

SCENARIO("A tokenizer can properly tokenize strings", "[Tokenizer]")
{
	GIVEN("a tokenizer that retains empty elements")
	{
		Tokenizer<KeepEmpty> tokenizer {};
		
		WHEN("passed an empty string")
		{
			str  text   = "";
			auto tokens = tokenizer.Tokenize(text);
			
			THEN("it returns a vector containing only an empty string")
			{
				REQUIRE(tokens.size() == 1);
				REQUIRE(tokens[0] == text);
			}
		}
		
		WHEN("passed a string with a single word and no delimiter matches")
		{
			str  text   = "test";
			auto tokens = tokenizer.Tokenize(text);
			
			THEN("it returns a vector containing only that word")
			{
				REQUIRE(tokens.size() == 1);
				REQUIRE(tokens[0] == text);
			}
		}
		
		WHEN("passed a string with a single word and a delimiter in the front")
		{
			str  text   = " test";
			auto tokens = tokenizer.Tokenize(text);
			
			THEN("it returns a vector containing an empty string and the word in that order")
			{
				REQUIRE(tokens.size() == 2);
				REQUIRE(tokens[0] == "");
				REQUIRE(tokens[1] == "test");
			}
		}
		
		WHEN("passed a string with a single word and a delimiter in the back")
		{
			str  text   = "test ";
			auto tokens = tokenizer.Tokenize(text);
			
			THEN("it returns a vector containing the word and an empty string in that order")
			{
				REQUIRE(tokens.size() == 2);
				REQUIRE(tokens[0] == "test");
				REQUIRE(tokens[1] == "");
			}
		}
	}
}