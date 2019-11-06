#pragma once

#include "cheonsa__types.h"

namespace cheonsa
{

	// used to coordinate mutually exclusive data access between multiple threads.
	// any number of threads may request to enter (lock) the critical section.
	// once one thread enters (locks) the critical section, any other threads that also request to enter (lock) the critical section will be forced to block (wait).
	// this is one way to avoid race conditions (concurrent data access).
	class platform_critical_section_c
	{
	private:
		void_c * _members; // operating system specific members implementation. defined in the cpp file.

	public:
		platform_critical_section_c();
		~platform_critical_section_c();

		void_c enter(); // call this to secure mutually exclusive access to the critical section of data. any number of threads can call enter() at the same time, but only one thread will be allowed to proceed, and the other threads will block until exit() is called.
		void_c exit(); // call this to release mutually exclusive access to the critical section of data.

	};

}
