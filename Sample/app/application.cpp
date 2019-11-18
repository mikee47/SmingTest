#include <SmingTest.h>
#include "modules.h"

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
	SmingTest::runner.execute(testsComplete, RESTART_DELAY);
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

	SmingTest::runner.setGroupIntervalMs(TEST_GROUP_INTERVAL);

	// Start tests when system is ready
	System.onReady([]() { SmingTest::runner.execute(testsComplete); });
}
