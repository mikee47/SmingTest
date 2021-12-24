SmingTest
=========

.. highlight:: c++

An extensible test framework for Sming, with integrated profiling and scheduling support.

Getting started
---------------

A :source:`Sample` test skeleton project is provided which you can copy to provide a starting point.

See the Sming :source:`/tests/HostTests` test application for a more comprehensive example.

-  A test module contains all code related to testing a specific Component, Library or framework module
-  If a module contains more than one file then place all files in a sub-directory
-  Each module must have a registration function:

   - Use the :c:macro:`REGISTER_TEST` macro to name the function
   - Add an entry to the :source:`Sample/app/modules.h` file

-  Each module contains one or more test groups:

   - Each group is a class inheriting from TestGroup
   - Add a call to :cpp:func:`registerGroup` to the registration function for each test class

-  Keep each group brief. Use multiple, simpler groups if necessary.
-  The group :cpp:func:`TestGroup::execute` method is called to run the tests. On return the group is considered to
   have been successfully completed and destroyed.
-  Call :c:macro:`TEST_ASSERT` at appropriate points; passing ``false`` will fail the test and abort the process,
   displaying the source location of the failure.
-  If a test fails then additional details may be shown before calling :c:macro:`TEST_ASSERT(false) <TEST_ASSERT>`.
-  For asynchronous testing calling :cpp:func:`TestGroup::pending` before returning. When the tests have been completed
   call :cpp:func:`TestGroup::complete`. (See :source:`/tests/HostTests/modules/Timers.cpp` for an example.)

The following macros are added for ease of importing tests from other frameworks:

-  :c:macro:`TEST_CASE`
-  :c:macro:`REQUIRE`


What happens
------------

The :cpp:func:`registerGroup` function creates a factory function which is added to the :cpp:var:`SmingTest::Runner::groupFactories` list.

The test runner creates, executes and destroys each group in turn, and deals with scheduling.


Notes
-----

Tests are run with DEBUG_VERBOSE_LEVEL at WARNING level, so `debug_i` statements will not normally be shown.
Tests can use other `debug_X` functions as required, or ``Serial`` print methods.

Tests should compile and run for all architectures.


API Documentation
-----------------

.. doxygenfile:: SmingTest.h
.. doxygenfile:: SmingTest/TestBase.h
.. doxygenfile:: SmingTest/TestGroup.h
