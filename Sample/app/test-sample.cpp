#include <SmingTest.h>

class SampleTest : public TestGroup
{
public:
	SampleTest() : TestGroup(_F("Sample"))
	{
	}

	void execute() override
	{
		TEST_CASE("Check a String")
		{
			String s("This is a string");
			REQUIRE(s.length() == 16);
		}
	}
};

void REGISTER_TEST(sample)
{
	registerGroup<SampleTest>();
}
