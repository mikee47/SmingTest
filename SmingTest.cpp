/**
 * SmingTest.cpp
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

#include "include/SmingTest.h"
#include <malloc_count.h>
#ifdef ARCH_HOST
#include <IFS/FileSystem.h>
#endif

namespace SmingTest
{
Runner runner;

Framework framework;

#ifdef ARCH_HOST

const char* moduleName;

static void ciUpdateState(TestGroup& group)
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

#endif

Runner::Runner() : totalTestTime(NanoTime::Milliseconds, 0)
{
#ifdef ARCH_HOST
	moduleName = getenv("MODULE") ?: "SmingTest";

	if(getenv("APPVEYOR") != nullptr) {
		framework = Framework::appveyor;
	} else {
		framework = Framework::none;
	}
#endif
}

void Runner::runNextGroup()
{
	assert(state != State::running);

	if(taskIndex < groupFactories.count()) {
		auto factory = groupFactories[taskIndex++];

		auto group = factory();
		assert(group != nullptr);

		m_printf(_F("\r\n"
					"\r\n"
					"** Test Group: %s (%u of %u)**\r\n"
					"\r\n"),
				 group->getName().c_str(), taskIndex, groupFactories.count());

		state = State::running;
#ifdef ARCH_HOST
		ciUpdateState(*group);
#endif
		group->initialiseAndExecute();
		return;
	}

	state = State::stopped;

	m_printf("\r\n\nTESTS COMPLETE\r\n\n");
	m_printf("%u groups, %u failures\r\n", testCount, failureCount);
	m_printf("Heap allocations: %u, total: %u bytes, peak: %u, current: %u\r\n", MallocCount::getAllocCount(),
			 MallocCount::getTotal(), MallocCount::getPeak(), MallocCount::getCurrent());
	m_printf("Total test time: %s\r\n\n", totalTestTime.value().toString().c_str());

#ifdef ARCH_HOST
	if(failureCount != 0) {
		auto filename = getenv("TEST_FAILURE_FILE");
		if(filename != nullptr) {
			IFS::File file(&IFS::Host::getFileSystem());
			file.open(filename, IFS::File::CreateNewAlways);
			assert(file);
		}
	}
#endif

	onComplete();
}

void Runner::groupComplete(TestGroup* group)
{
	auto elapsed = group->elapsedTime();
	totalTestTime += elapsed;
	++testCount;
	if(group->getState() == TestGroup::State::failed) {
		m_printf(_F("\r\n!!!! Test Group '%s' FAILED !!!!\r\n\r\n"), group->getName().c_str());
		++failureCount;
	} else {
		m_printf(_F("\r\n** Test Group '%s' OK ** Elapsed: %s\r\n"), group->getName().c_str(),
				 elapsed.toString().c_str());
	}

#ifdef ARCH_HOST
	ciUpdateState(*group);
#endif
	delete group;
	taskTimer.setIntervalMs(groupIntervalMs);
	taskTimer.startOnce();
	state = State::waiting;
}

void Runner::execute(Callback onComplete, unsigned initialDelayMs)
{
	assert(state == State::stopped);

	this->onComplete = onComplete;

	totalTestTime.time = 0;
	taskIndex = 0;
	testCount = failureCount = 0;

	taskTimer.setCallback([this]() { runNextGroup(); });

	if(initialDelayMs == 0) {
		runNextGroup();
	} else {
		taskTimer.setIntervalMs(initialDelayMs);
		taskTimer.startOnce();
		state = State::waiting;
	}
}

} // namespace SmingTest
