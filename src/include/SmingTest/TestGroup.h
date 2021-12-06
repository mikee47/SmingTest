/**
 * TestGroup.h - Extensible test framework for Sming
 *
 * Copyright 2019 mikee47 <mike@sillyhouse.net>
 *
 * This file is part of the SmingTest Library
 *
 * This library is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, version 3 or later.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this library.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 * @author: 2018 - Mikee47 <mike@sillyhouse.net>
 *
 ****/

#pragma once

#include "TestBase.h"
#include <Platform/Timers.h>
#include <Services/Profiling/MinMaxTimes.h>
#include <csetjmp>

using namespace Profiling;

/**
 * @brief Class to simplify generation of begin/end messages for a test group
 */
class TestGroup : public TestBase
{
public:
	TestGroup(const String& name) : name(name)
	{
	}

	void commenceTest();

	/**
	 * @brief Implement this method to define the test
	 * @note If tests are asynchronous, call `pending()` before returning and call `complete()`
	 * when the group has completed execution (e.g. via timer callback, etc.)
	 */
	virtual void execute() = 0;

	/**
	 * @brief Note the start of a test item within a group
	 */
	void startItem(const String& tag);

	/**
	 * @brief Called when test fails to identify location
	 */
	void fail(const char* func) override;

	const String& getName()
	{
		return name;
	}

	NanoTime::Time<uint32_t> elapsedTime() const
	{
		return groupTimer.elapsedTime();
	}

	enum class State {
		running,
		pending,
		complete,
		failed,
	};

	State getState() const
	{
		return state;
	}

	/**
	 * @brief Called by test runner
	 */
	void initialiseAndExecute();

protected:
	void pending()
	{
		state = State::pending;
	}

	void complete();

private:
	String name;
	State state{State::running};
	OneShotFastUs groupTimer;
	jmp_buf exception;
};

#define startTest(s) startItem(_F(s))

// Catch support
#define TEST_CASE(name, ...) startItem(_F(name));
