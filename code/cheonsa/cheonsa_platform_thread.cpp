#include "cheonsa_platform_thread.h"
#include <cassert>
#if defined( cheonsa_platform_windows )
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace cheonsa
{

	class platform_thread_members_c
	{
	public:
#if defined( cheonsa_platform_windows )
		HANDLE _handle; // thread handle.
		static DWORD WINAPI thread_function( LPVOID parameter )
		{
			platform_thread_c * platform_thread = reinterpret_cast< platform_thread_c * >( parameter );
			uint32_c result = (*platform_thread->_function)( platform_thread );
			return result;
		}
#endif
	};

	platform_thread_c::platform_thread_c()
		: _members( nullptr )
		, _function( nullptr )
		, _parameter( nullptr )
		, _keep_alive( false )
	{
		_members = new platform_thread_members_c();
	}

	platform_thread_c::~platform_thread_c()
	{
		platform_thread_members_c * members = reinterpret_cast< platform_thread_members_c * >( _members );
#if defined( cheonsa_platform_windows )
		if ( members->_handle != nullptr )
		{
			stop();
		}
#endif
		delete _members;
		_members = nullptr;
	}

	void_c * platform_thread_c::get_parameter() const
	{
		return _parameter;
	}

	boolean_c platform_thread_c::get_keep_alive() const
	{
		return _keep_alive;
	}

	void_c platform_thread_c::start( uint32_c (*function)( platform_thread_c * platform_thread ), void_c * parameter )
	{
		platform_thread_members_c * members = reinterpret_cast< platform_thread_members_c * >( _members );
#if defined( cheonsa_platform_windows )
		assert( members->_handle == nullptr );
		assert( function != nullptr );
		_keep_alive = true;
		_function = function;
		_parameter = parameter;
		members->_handle = CreateThread( NULL, 0, &platform_thread_members_c::thread_function, this, 0, NULL );
#endif
	}

	void_c platform_thread_c::stop()
	{
		platform_thread_members_c * members = reinterpret_cast< platform_thread_members_c * >( _members );
#if defined( cheonsa_platform_windows )
		assert( members->_handle != nullptr );
		_keep_alive = false;
		WaitForSingleObject( members->_handle, INFINITE );
		CloseHandle( members->_handle );
		members->_handle = nullptr;
		_function = nullptr;
		_parameter = nullptr;
#endif
	}

	void_c platform_thread_c::sleep( sint32_c milliseconds )
	{
#if defined( cheonsa_platform_windows )
		Sleep( milliseconds );
#endif
	}

}
