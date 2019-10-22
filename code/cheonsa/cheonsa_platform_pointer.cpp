#include "cheonsa_platform_pointer.h"
#include "cheonsa_engine.h"

#if defined( cheonsa_platform_windows )
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace cheonsa
{

	platform_pointer_c::platform_pointer_c()
		: pointer_handle( 0 )
	{
#if defined( cheonsa_platform_windows )
		pointer_handle = LoadCursor( NULL, IDC_ARROW );
#endif
	}

	platform_pointer_c::~platform_pointer_c()
	{
#if defined( cheonsa_platform_windows )
		DestroyCursor( reinterpret_cast< HCURSOR >( pointer_handle ) );
#endif
	}

	boolean_c platform_pointer_c::load( string16_c const & file_path )
	{
		un_load();
#if defined( cheonsa_platform_windows )
		if ( pointer_handle != nullptr )
		{
			DestroyCursor( static_cast< HCURSOR >( pointer_handle ) );
			pointer_handle = nullptr;
		}
		string16_c full_file_path;
		if ( global_engine_instance.interfaces.content_manager->resolve_file_path( file_path, full_file_path ) )
		{
			HCURSOR NewPointerHandle = LoadCursorFromFileW( full_file_path.character_list.get_internal_array() );
			if ( NewPointerHandle )
			{
				pointer_handle = NewPointerHandle;
				return true;
			}
		}
		if ( pointer_handle == nullptr )
		{
			pointer_handle = LoadCursor( NULL, IDC_ARROW );
		}
#endif
		return false;
	}

	void_c platform_pointer_c::un_load()
	{
#if defined( cheonsa_platform_windows )
		DestroyCursor( static_cast< HCURSOR >( pointer_handle ) );
		pointer_handle = LoadCursor( NULL, IDC_ARROW );
#endif
	}

}
