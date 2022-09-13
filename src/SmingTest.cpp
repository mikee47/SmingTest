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
 * You should have received a copy of the GNU General Public License along with this library.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 * @author: 2018 - Mikee47 <mike@sillyhouse.net>
 *
 ****/

#include "include/SmingTest.h"
#include "ci.h"
#include <malloc_count.h>
#ifdef ARCH_HOST
#include <IFS/FileSystem.h>
#endif

namespace SmingTest
{
Runner runner;

Runner::Runner() : totalTestTime(NanoTime::Milliseconds, 0)
{
	CI::initialise();
}

void Runner::runNextGroup()
{
	assert(state != State::running);

	if(taskIndex < groupFactories.count()) {
		auto factory = groupFactories[taskIndex++];

		auto group = factory();
		assert(group != nullptr);

		Serial << endl
			   << endl
			   << "** Test Group: " << group->getName() << " (" << taskIndex << " of " << groupFactories.count()
			   << ")**" << endl
			   << endl;

		state = State::running;
		CI::updateState(*group);
		group->initialiseAndExecute();
		return;
	}

	state = State::stopped;

	Serial << endl << endl << _F("TESTS COMPLETE") << endl << endl;
	Serial << testCount << _F(" groups, ") << failureCount << _F(" failures") << endl;
	Serial << _F("Heap allocations: ") << MallocCount::getAllocCount() << _F(", total: ") << MallocCount::getTotal()
		   << _F(" bytes, peak: ") << MallocCount::getPeak() << _F(", current: ") << MallocCount::getCurrent() << endl;
	Serial << _F("Total test time: ") << totalTestTime.value().toString() << endl << endl;

	if(failureCount != 0) {
		CI::markTestFailure();
	}

	onComplete();
}

void Runner::groupComplete(TestGroup* group)
{
	auto elapsed = group->elapsedTime();
	totalTestTime += elapsed;
	++testCount;
	if(group->getState() == TestGroup::State::failed) {
		Serial << endl << _F("!!!! Test Group '") << group->getName() << _F("' FAILED !!!!") << endl << endl;
		++failureCount;
	} else {
		Serial << endl
			   << _F("** Test Group '") << group->getName() << _F("' OK ** Elapsed: ") << elapsed.toString() << endl;
	}

	CI::updateState(*group);
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
