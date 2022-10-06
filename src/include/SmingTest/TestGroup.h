/****
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
	void startItem(const String& tag, const String& description = nullptr);

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
	/**
	 * @brief Call to mark test as pending so it will be executed asynchronously
	 * Call `complete()` when test is finished.
	 */
	void pending()
	{
		state = State::pending;
	}

	/**
	 * @brief Call to complete pending (asynchronous) test
	 */
	void complete();

private:
	String name;
	State state{State::running};
	OneShotFastUs groupTimer;
	jmp_buf exception;
};

#define startTest(s) startItem(_F(s))

/**
 * @brief Start a test item
 *
 * Use like this:
 * 
 * ```
 * TEST_CASE("My Test", "description") {
 * 	  ...
 * }
 * ```
 *
 * Note: Description is optional.
 */
// https://stackoverflow.com/questions/3046889/optional-parameters-with-c-macros
#define TEST_CASE_1_ARG(name) startItem(_F(name));
#define TEST_CASE_2_ARGS(name, desc) startItem(_F(name), _F(desc));

#define GET_3RD_ARG(arg1, arg2, arg3, ...) arg3
#define TEST_CASE_ARG_CHOOSER(...) GET_3RD_ARG(__VA_ARGS__, TEST_CASE_2_ARGS, TEST_CASE_1_ARG, )

#define TEST_CASE(...) TEST_CASE_ARG_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
