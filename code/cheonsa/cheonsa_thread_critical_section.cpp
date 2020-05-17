#include "cheonsa_thread_critical_section.h"

#if defined( cheonsa_platform_windows )
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace cheonsa
{

	class thread_critical_section_members_c
	{
	public:
#if defined( cheonsa_platform_windows )
		CRITICAL_SECTION critical_section;
#endif
	};

	thread_critical_section_c::thread_critical_section_c()
		: _members( nullptr )
	{
		_members = new thread_critical_section_members_c();
		thread_critical_section_members_c * members = reinterpret_cast< thread_critical_section_members_c * >( _members );
#if defined( cheonsa_platform_windows )
		InitializeCriticalSection( &members->critical_section );
#endif
	}

	thread_critical_section_c::~thread_critical_section_c()
	{
		thread_critical_section_members_c * members = reinterpret_cast< thread_critical_section_members_c * >( _members );
#if defined( cheonsa_platform_windows )
		DeleteCriticalSection( &members->critical_section );
#endif
		delete members;
		_members = nullptr;
	}

	void_c thread_critical_section_c::enter()
	{
		thread_critical_section_members_c * members = reinterpret_cast< thread_critical_section_members_c * >( _members );
#if defined( cheonsa_platform_windows )
		EnterCriticalSection( &members->critical_section );
#endif
	}

	void_c thread_critical_section_c::exit()
	{
		thread_critical_section_members_c * members = reinterpret_cast< thread_critical_section_members_c * >( _members );
#if defined( cheonsa_platform_windows )
		LeaveCriticalSection( &members->critical_section );
#endif
	}

}
