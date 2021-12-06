/**
 * ci.cpp
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

#include "../../ci.h"
#include <IFS/File.h>

namespace SmingTest
{
namespace CI
{
enum Framework {
	none,
	appveyor,
};

namespace
{
Framework framework;
const char* moduleName;
} // namespace

void initialise()
{
	moduleName = getenv("MODULE") ?: "SmingTest";

	if(getenv("APPVEYOR") != nullptr) {
		framework = Framework::appveyor;
	} else {
		framework = Framework::none;
	}
}

bool isEnabled()
{
	return framework != Framework::none;
}

void updateState(TestGroup& group)
{
	using State = TestGroup::State;

	switch(framework) {
	case Framework::appveyor: {
		auto state = group.getState();
		String s = F("appveyor ");
		s += (state == State::running) ? F("AddTest") : F("UpdateTest");
		s += " \"";
		s += group.getName();
		s += F("\" -Framework Sming -Filename \"");
		s += moduleName;
		s += F("\" -Outcome ");
		switch(state) {
		case State::running:
			s += F("Running");
			break;
		case State::complete:
			s += F("Passed");
			break;
		case State::failed:
			s += F("Failed");
			break;
		default:
			s += F("Inconclusive");
		}

		if(state != State::running) {
			s += F(" -Duration ");
			s += group.elapsedTime().as<NanoTime::Milliseconds>();
		}

		assert(system(s.c_str()) == 0);
	}

	case Framework::none:
		break;
	}
}

void markTestFailure()
{
	auto filename = getenv("TEST_FAILURE_FILE");
	if(filename != nullptr) {
		IFS::File file(&IFS::Host::getFileSystem());
		file.open(filename, IFS::File::CreateNewAlways);
		assert(file);
	}
}

} // namespace CI
} // namespace SmingTest
