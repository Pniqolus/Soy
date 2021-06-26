#ifndef REPORTER_HPP
#define REPORTER_HPP

#include "Types.hpp"

#include <catch2/reporters/catch_reporter_streaming_base.hpp>

struct BddReporter : Catch::StreamingReporterBase
{
	using StreamingReporterBase::StreamingReporterBase;

	~BddReporter() override;

	static str getDescription();

	void noMatchingTestCases(const str& spec);
	void assertionStarting(const Catch::AssertionInfo& info);
	bool assertionEnded(const Catch::AssertionStats& stats);
	void sectionEnded(const Catch::SectionStats& stats);
	void testRunEnded(const Catch::TestRunStats& stats);
};

#endif /* REPORTER_HPP */