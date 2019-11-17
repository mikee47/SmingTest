/*
 * Test framework
 *
 * Related tests should be grouped into a single module.
 * The test code should be in a separate file called `test-XXX.cpp`
 * Each group must have a single entry function `void test_XXX()` - define using REGISTER_TEST macro
 * The name of the test must be added to the TEST_MAP definition below.
 *
 * See common.h for further details.
 */

#include <SmingTest.h>
#include "modules.h"

// Restart tests after a pause
#define RESTART_DELAY_MS 10000

#define XX(t) extern void REGISTER_TEST(t);
TEST_MAP(XX)
#undef XX

static void registerTests()
{
#define XX(t) REGISTER_TEST(t);
	TEST_MAP(XX)
#undef XX
}

static void testsComplete()
{
#ifdef ARCH_HOST
	// In the Host Emulator, this ends the session
	System.restart();
#else
	SmingTest::runner.execute(testsComplete, RESTART_DELAY_MS);
#endif
}

void init()
{
	Serial.setTxBufferSize(1024);
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.systemDebugOutput(true);

	// If you need to use the filesystem
	//	spiffs_mount();

	// Enable if you need network tests
	WifiStation.enable(false, false);
	WifiAccessPoint.enable(false, false);

	// Register tests with the framework
	registerTests();

	// Start tests when system is ready
	System.onReady([]() { SmingTest::runner.execute(testsComplete); });
}
