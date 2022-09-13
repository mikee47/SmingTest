/**
 * TestBase.cpp
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

#include "include/SmingTest/TestBase.h"
#include <HardwareSerial.h>

bool TestBase::testVerify(bool res, const TestParam& param)
{
	if(!res || param.verbose) {
		if(param.value1) {
			Serial << (res ? "OK: " : "FAIL: ") << param.expr << " (" << param.value1 << ", " << param.value2 << ')'
				   << endl;
		} else {
			Serial << (res ? "OK: " : "FAIL: ") << param.expr << endl;
		}
	}
	return res;
}
