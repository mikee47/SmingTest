/**
 * TestBase.h
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

#include <WString.h>
#include <debug_progmem.h>

class TestBase
{
public:
	struct TestParam {
		const char* expr;
		String value1;
		String value2;
		bool verbose;
	};

	virtual ~TestBase()
	{
	}

	virtual bool testVerify(bool res, const TestParam& param);

	bool test_verify(bool res, const char* expr, const String& value1, const String& value2, bool verbose)
	{
		return testVerify(res, TestParam{expr, value1, value2, verbose});
	}

	template <typename V>
	typename std::enable_if<std::is_integral<V>::value, bool>::type
	test_verify(bool res, const char* expr, const V& value1, const V& value2, bool verbose)
	{
		return testVerify(res, TestParam{expr, String(value1), String(value2), verbose});
	}

	template <typename V>
	typename std::enable_if<!std::is_same<V, String>::value && !std::is_integral<V>::value, bool>::type
	test_verify(bool res, const char* expr, const V& value1, const V& value2, bool verbose)
	{
		return testVerify(res, TestParam{expr, toString(value1), toString(value2), verbose});
	}

	virtual void fail(const char* func)
	{
		m_printf(_F("FAIL in `%s`\r\n"), func);
	}
};

/**
 * @brief Check an expression, on failure print it before assertion
 */
#define INTERNAL_CHECK(expr, verbose) INTERNAL_CHECK2(expr, expr, verbose)

#define INTERNAL_CHECK2(res, expr, verbose)                                                                            \
	do {                                                                                                               \
		PSTR_ARRAY(tmpExprStr, #expr);                                                                                 \
		if(!testVerify(bool(res), TestParam{tmpExprStr, nullptr, nullptr, verbose})) {                                 \
			TEST_ASSERT(false);                                                                                        \
		}                                                                                                              \
	} while(0)

#define INTERNAL_CHECK_EQ(a, b, verbose)                                                                               \
	do {                                                                                                               \
		PSTR_ARRAY(tmpExprStr, #a " == " #b);                                                                          \
		const auto value_a = a;                                                                                        \
		const auto value_b = b;                                                                                        \
		if(!test_verify(value_a == value_b, tmpExprStr, value_a, value_b, verbose)) {                                  \
			TEST_ASSERT(false);                                                                                        \
		}                                                                                                              \
	} while(0)

#define INTERNAL_CHECK_NEQ(a, b, verbose)                                                                              \
	do {                                                                                                               \
		PSTR_ARRAY(tmpExprStr, #a " != " #b);                                                                          \
		const auto value_a = a;                                                                                        \
		const auto value_b = b;                                                                                        \
		if(!test_verify(value_a != value_b, tmpExprStr, value_a, value_b, verbose)) {                                  \
			TEST_ASSERT(false);                                                                                        \
		}                                                                                                              \
	} while(0)

/**
 * @brief Check an expression, but only print message on failure
 */
#define CHECK(expr) INTERNAL_CHECK(expr, false)
#define CHECK2(res, expr) INTERNAL_CHECK2(res, expr, false)
#define CHECK_EQ(a, b) INTERNAL_CHECK_EQ(a, b, false)
#define CHECK_NEQ(a, b) INTERNAL_CHECK_NEQ(a, b, false)

/**
 * @brief Check an expression, on failure print it before assertion
 */
#define REQUIRE(expr) INTERNAL_CHECK(expr, true)
#define REQUIRE2(res, expr) INTERNAL_CHECK2(res, expr, true)
#define REQUIRE_EQ(a, b) INTERNAL_CHECK_EQ(a, b, true)
#define REQUIRE_NEQ(a, b) INTERNAL_CHECK_NEQ(a, b, true)

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
