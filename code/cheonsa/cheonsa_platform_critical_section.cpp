#include "cheonsa_platform_critical_section.h"

#if defined( cheonsa_platform_windows )
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace cheonsa
{

	class platform_critical_section_members_c
	{
	public:
#if defined( cheonsa_platform_windows )
		CRITICAL_SECTION critical_section;
#endif
	};

	platform_critical_section_c::platform_critical_section_c()
		: _members( nullptr )
	{
		_members = new platform_critical_section_members_c();
		platform_critical_section_members_c * members = reinterpret_cast< platform_critical_section_members_c * >( _members );
#if defined( cheonsa_platform_windows )
		InitializeCriticalSection( &members->critical_section );
#endif
	}

	platform_critical_section_c::~platform_critical_section_c()
	{
		platform_critical_section_members_c * members = reinterpret_cast< platform_critical_section_members_c * >( _members );
#if defined( cheonsa_platform_windows )
		DeleteCriticalSection( &members->critical_section );
#endif
		delete members;
		_members = nullptr;
	}

	void_c platform_critical_section_c::enter()
	{
		platform_critical_section_members_c * members = reinterpret_cast< platform_critical_section_members_c * >( _members );
#if defined( cheonsa_platform_windows )
		EnterCriticalSection( &members->critical_section );
#endif
	}

	void_c platform_critical_section_c::exit()
	{
		platform_critical_section_members_c * members = reinterpret_cast< platform_critical_section_members_c * >( _members );
#if defined( cheonsa_platform_windows )
		LeaveCriticalSection( &members->critical_section );
#endif
	}

}
