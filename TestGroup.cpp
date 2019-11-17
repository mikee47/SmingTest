/**
 * TestGroup.cpp
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
 * You should have received a copy of the GNU General Public License along with FlashString.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 * @author: 2018 - Mikee47 <mike@sillyhouse.net>
 *
 ****/

#include "SmingTest.h"

void TestGroup::initialiseAndExecute()
{
	groupTimer.start();
	state = State::running;
	execute();
	if(state != State::pending) {
		complete();
	}
}

void TestGroup::startItem(const String& tag)
{
	m_printf(_F("\r\n>> %s\r\n"), tag.c_str());
}

void TestGroup::complete()
{
	SmingTest::runner.groupComplete(this);
}

void TestGroup::fail(const char* func)
{
	state = State::failed;
	m_printf(_F("FAIL in `%s`\r\n"), func);
}
