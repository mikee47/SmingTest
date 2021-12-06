/**
 * ci.h
 *
 * Copyright 2021 mikee47 <mike@sillyhouse.net>
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
 * @author: 2021 - Mikee47 <mike@sillyhouse.net>
 *
 ****/

#pragma once

#include "include/SmingTest/TestGroup.h"

namespace SmingTest
{
namespace CI
{
#ifdef ARCH_HOST
#define CI_API_DECLARE(func, ...) void func(__VA_ARGS__);
bool isEnabled();
#else
#define CI_API_DECLARE(func, ...)                                                                                      \
	static inline void func(__VA_ARGS__)                                                                               \
	{                                                                                                                  \
	}

static inline bool isEnabled()
{
	return false;
}
#endif

CI_API_DECLARE(initialise)
CI_API_DECLARE(updateState, TestGroup& group)
CI_API_DECLARE(markTestFailure)

} // namespace CI
} // namespace SmingTest
