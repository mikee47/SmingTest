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

namespace SmingTest
{
Runner runner;

void ciUpdateState(TestGroup& group)
{
#ifdef ARCH_HOST

	using State = TestGroup::State;

	auto state = group.getState();
	String s = F("appveyor ");
	s += (state == State::running) ? F("AddTest") : F("UpdateTest");
	s += " \"";
	s += group.getName();
	s += F("\" -Framework Sming -Filename SmingTest -Outcome ");
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

	system(s.c_str());

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
		ciUpdateState(*group);
		group->initialiseAndExecute();
		return;
	}

	state = State::stopped;

	m_printf("\r\n\nTESTS COMPLETE\r\n\n");
	m_printf("Heap allocations: %u, total: %u bytes, peak: %u, current: %u\r\n", MallocCount::getAllocCount(),
			 MallocCount::getTotal(), MallocCount::getPeak(), MallocCount::getCurrent());
	m_printf("Total test time: %s\r\n\n", totalTestTime.value().toString().c_str());

	onComplete();
}

void Runner::groupComplete(TestGroup* group)
{
	auto elapsed = group->elapsedTime();
	totalTestTime += elapsed;
	if(group->getState() == TestGroup::State::failed) {
		m_printf(_F("\r\n!!!! Test Group '%s' FAILED !!!!\r\n\r\n"), group->getName().c_str());
	} else {
		m_printf(_F("\r\n** Test Group '%s' OK ** Elapsed: %s\r\n"), group->getName().c_str(),
				 elapsed.toString().c_str());
	}

	ciUpdateState(*group);

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
