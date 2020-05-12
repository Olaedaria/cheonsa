#pragma once

#include "cheonsa_types.h"
#include "cheonsa_core_event.h"

namespace cheonsa
{

	// runs a function on another thread.
	class platform_thread_c
	{
	private:
		friend class platform_thread_members_c;
		void_c * _members; // operating system specific members implementation. defined in the cpp file.
		uint32_c (*_function)( platform_thread_c * platform_thread ); // the thread function that can loop forever until _keep_alive is set to false, or can return whenever it finishes what it's supposed to be doing.
		void_c * _parameter; // arbitrary thing to pass to the function.
		boolean_c _keep_alive; // set to false to tell the thread function to return when it gets the chance.

	public:
		platform_thread_c();
		~platform_thread_c();

		void_c * get_parameter() const;

		boolean_c get_keep_alive() const;

		// the thread function should be a function that loops forever and cleans up when platform_thread->get_keep_alive() returns false, or it can just return whenever.
		// it's not important what the thread function returns.
		void_c start( uint32_c (*function)( platform_thread_c * platform_thread ), void_c * parameter );
		// this will set _keep_alive to false and it will block until the thread exits.
		void_c stop();

		// blocks the calling thread for the specified time.
		static void_c sleep( sint32_c milliseconds );

	};

}
