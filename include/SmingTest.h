/**
 * SmingTest.h - Extensible test framework for Sming
 *
 * Copyright 2019 mikee47 <mike@sillyhouse.net>
 *
 * This file is part of the FlashString Library
 *
 * This library is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, version 3 or later.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with SmingTest.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 * @author: 2018 - Mikee47 <mike@sillyhouse.net>
 *
 ****/

#pragma once

#include "TestGroup.h"
#include <WVector.h>
#include <Timer.h>

#define REGISTER_TEST(name) register_test_##name()

namespace SmingTest
{
/**
 * @brief Factory function to create a TestGroup class
 */
typedef TestGroup* (*Factory)();

using Callback = Delegate<void()>;

class Runner
{
public:
	Runner() : totalTestTime(NanoTime::Milliseconds, 0)
	{
	}

	void setGroupIntervalMs(unsigned ms)
	{
		groupIntervalMs = ms;
	}

	void addFactory(Factory factory)
	{
		groupFactories.add(factory);
	}

	void execute(Callback onComplete, unsigned initialDelayMs = 0);

	void groupComplete(TestGroup* group);

protected:
	void runNextGroup();

private:
	Vector<Factory> groupFactories; ///< List of registered class factories
	Timer taskTimer;
	unsigned taskIndex = 0;
	enum class State {
		stopped,
		waiting, ///< In between tests
		running,
	};
	State state = State::stopped;
	NanoTime::Time<uint32_t> totalTestTime;
	unsigned groupIntervalMs = 500;
	Callback onComplete;
};

extern Runner runner;

} // namespace SmingTest

/**
 * @brief Register a factory function (a lambda) to create a given TestGroup class
 * @tparam Class to be registered
 */
template <class GroupClass> void registerGroup()
{
	SmingTest::runner.addFactory([]() -> TestGroup* { return new GroupClass; });
}
