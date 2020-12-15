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
 * You should have received a copy of the GNU General Public License along with SmingTest.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 * @author: 2018 - Mikee47 <mike@sillyhouse.net>
 *
 ****/

#pragma once

#include <SmingCore.h>
#include <Platform/Timers.h>
#include <Services/Profiling/MinMaxTimes.h>
using namespace Profiling;

/**
 * @brief Class to simplify generation of begin/end messages for a test group
 */
class TestGroup
{
public:
	TestGroup(const String& name) : name(name)
	{
	}

	virtual ~TestGroup()
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
	void fail(const char* func);

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
	State state = State::running;
	OneShotFastUs groupTimer;
};

#define startTest(s) startItem(_F(s))

// Catch support
#define TEST_CASE(name, ...) startItem(_F(name));

/**
 * @brief Check an expression, on failure print it before assertion
 */
#define REQUIRE(expr) REQUIRE2(expr, expr)

#define REQUIRE2(res, expr)                                                                                            \
	do {                                                                                                               \
		PSTR_ARRAY(tmpExprStr, #expr);                                                                                 \
		if(res) {                                                                                                      \
			debug_i("OK: %s", tmpExprStr);                                                                             \
		} else {                                                                                                       \
			debug_e("FAIL: %s", tmpExprStr);                                                                           \
			TEST_ASSERT(false);                                                                                        \
		}                                                                                                              \
	} while(0)

#define REQUIRE_EQ(a, b)                                                                                               \
	do {                                                                                                               \
		PSTR_ARRAY(tmpExprStr, #a " == " #b);                                                                          \
		auto value_a = a;                                                                                              \
		auto value_b = b;                                                                                              \
		if(value_a == value_b) {                                                                                       \
			debug_i("OK: %s (%s)", tmpExprStr, String(value_a).c_str());                                               \
		} else {                                                                                                       \
			debug_e("FAIL: %s (%s, %s)", tmpExprStr, String(value_a).c_str(), String(value_b).c_str());                \
			TEST_ASSERT(false);                                                                                        \
		}                                                                                                              \
	} while(0)

#define REQUIRE_NEQ(a, b)                                                                                              \
	do {                                                                                                               \
		PSTR_ARRAY(tmpExprStr, #a " != " #b);                                                                          \
		auto value_a = a;                                                                                              \
		auto value_b = b;                                                                                              \
		String s;                                                                                                      \
		s += value_a;                                                                                                  \
		s += ", ";                                                                                                     \
		s += value_b;                                                                                                  \
		if(value_a != value_b) {                                                                                       \
			debug_e("OK: %s (%s)", tmpExprStr, s.c_str());                                                             \
		} else {                                                                                                       \
			debug_e("FAIL: %s (%s)", tmpExprStr, s.c_str());                                                           \
			TEST_ASSERT(false);                                                                                        \
		}                                                                                                              \
	} while(0)

/**
 * @brief Check a test result
 * @param result true if test was successful, false on failure
 * @note Failure generates an assertion so when run in the host emulator the process fails.
 */
#define TEST_ASSERT(result)                                                                                            \
	if(!(result)) {                                                                                                    \
		fail(__PRETTY_FUNCTION__);                                                                                     \
		assert(false);                                                                                                 \
	}
