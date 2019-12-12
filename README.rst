SmingTest
=========

An extensible test framework for Sming, with integrated profiling and scheduling support.

-  A test module contains all code related to testing a specific Component, Library or framework module
-  If a module contains more than one file then place all files in a sub-directory
-  Each module must have a registration function
-     Use the REGISTER_TEST macro to name the function
-     Add an entry to the `modules.h` file
-  Each module contains one or more test groups
-     Each group is a class inheriting from TestGroup
-     Add a call to `registerGroup<>` to the registration function for each test class
-  Keep each group brief. Use multiple, simpler groups if necessary.
-  The group `execute()` method is called to run the tests. On return the group is considered to
   have been successfully completed and destroyed.
-  Call `TEST_ASSERT()` at appropriate points; passing `false` will fail the test and abort the process,
   displaying the source location of the failure.
-  If a test fails then additional details may be shown before calling `TEST_ASSERT(false)`.
-  For asynchronous testing calling `pending()` before returning. When the tests have been completed
   call `complete()`. (See `test-timers` for an example.)

The following macros are added for ease of importing tests from other frameworks:

-  TEST_CASE()
-  REQUIRE()


What happens
------------

The `registerGroup<>` function creates a factory function which is added to the `groupFactories` list.

The test runner creates, executes and destroys each group in turn, and deals with scheduling.

Getting started
---------------

A `Sample` test skeleton project is provided which you can copy to provide a starting point.

See the Sming `HostTests` test application for how to create your own test applications.


Notes
-----

Tests are run with DEBUG_VERBOSE_LEVEL at WARNING level, so `debug_i` statements will not normally be shown.
Tests can use other `debug_X` functions as required, or `Serial` print methods.

Tests should compile and run for all architectures.
