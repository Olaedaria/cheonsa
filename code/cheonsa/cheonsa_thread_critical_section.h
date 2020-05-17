#pragma once

#include "cheonsa_types.h"

namespace cheonsa
{

	// used to coordinate mutually exclusive (mutex) access of data between multiple threads.
	// any number of threads may simultaneously request to enter (lock) the critical section.
	// at most only one thread is granted access to enter (lock) the critical section, and all the other threads that simultaneiously requested to enter (lock) the critical section will be made to block (wait, spin) until the one thread can exit (unlock) the critical section.
	class thread_critical_section_c
	{
	private:
		void_c * _members; // operating system specific members implementation. defined in the cpp file.

	public:
		thread_critical_section_c();
		~thread_critical_section_c();

		void_c enter(); // call this to secure mutually exclusive access to the critical section of data. any number of threads can call enter() at the same time, but only one thread will be allowed to proceed, and the other threads will block until exit() is called.
		void_c exit(); // call this to release mutually exclusive access to the critical section of data.

	};

}
