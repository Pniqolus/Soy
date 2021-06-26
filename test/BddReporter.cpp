#include "BddReporter.hpp"

#include "BddColor.hpp"

#include <ostream>

//------------------------------------------------------------------------------

namespace {

class AssertionPrinter
{
private:
	std::ostream&                   stream            ;
    const Catch::AssertionStats&    stats             ;
    const Catch::AssertionResult&   result            ;
    Color::Code                     color             ;
    std::string                     passOrFail        ;
    std::string                     messageLabel      ;
    std::string                     message           ;
    std::vector<Catch::MessageInfo> messages          ;
    bool                            printInfoMessages ;

public:
	AssertionPrinter& operator= (AssertionPrinter const&) = delete;
    AssertionPrinter(AssertionPrinter const&) = delete;

    AssertionPrinter(
		std::ostream&                _stream,
		const Catch::AssertionStats& _stats,
		bool                         _printInfoMessages)
	: 	stream            ( _stream                ),
		stats             ( _stats                 ),
		result            ( _stats.assertionResult ),
		color             ( Color::None            ),
		message           ( result.getMessage()    ),
		messages          ( _stats.infoMessages    ),
		printInfoMessages ( _printInfoMessages     )
	{
		
	}
};

}

//------------------------------------------------------------------------------

BddReporter::~BddReporter()
{

}

//------------------------------------------------------------------------------

str BddReporter::getDescription()
{
	return "Reports test results in a behavior driven development style";
}

//------------------------------------------------------------------------------

void BddReporter::noMatchingTestCases(const str& spec)
{
	stream << "No test cases matched '" << spec << "'\n";
}

//------------------------------------------------------------------------------

void BddReporter::assertionStarting(const Catch::AssertionInfo& info)
{

}

//------------------------------------------------------------------------------

bool BddReporter::assertionEnded(const Catch::AssertionStats& stats)
{
	return true;
}

//------------------------------------------------------------------------------

void BddReporter::sectionEnded(const Catch::SectionStats& stats)
{

}

//------------------------------------------------------------------------------

void BddReporter::testRunEnded(const Catch::TestRunStats& stats)
{
	
}

//------------------------------------------------------------------------------