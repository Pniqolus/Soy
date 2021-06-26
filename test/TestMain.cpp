#include <catch2/catch_session.hpp>
#include <catch2/internal/catch_compiler_capabilities.hpp>
#include <catch2/internal/catch_config_wchar.hpp>
#include <catch2/internal/catch_leak_detector.hpp>
#include <catch2/internal/catch_platform.hpp>

#include <catch2/interfaces/catch_interfaces_registry_hub.hpp>
#include <catch2/catch_reporter_registrars.hpp>

#include "BddReporter.hpp"

#include <iostream>

namespace Catch
{
    CATCH_INTERNAL_START_WARNINGS_SUPPRESSION
    CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS
    LeakDetector leakDetector;
    CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION
}

#if defined(CATCH_CONFIG_WCHAR) && defined(CATCH_PLATFORM_WINDOWS) && defined(_UNICODE) && !defined(DO_NOT_USE_WMAIN)
extern "C" int wmain (int argc, wchar_t * argv[], wchar_t * [])
{
#else
int main (int argc, char * argv[])
{
#endif
	Catch::getMutableRegistryHub().registerReporter(
		"bdd", Catch::Detail::make_unique<Catch::ReporterFactory<BddReporter>>()
	);

    (void)&Catch::leakDetector;

    return Catch::Session().run( argc, argv );
}