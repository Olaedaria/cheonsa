#include "cheonsa_platform_icon.h"
#include "cheonsa_engine.h"

#if defined( cheonsa_platform_windows )
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef LoadIcon
#endif

namespace cheonsa
{

	platform_icon_c::platform_icon_c()
		: icon_handle( nullptr )
	{
	}

	platform_icon_c::~platform_icon_c()
	{
		un_load();
	}

	boolean_c platform_icon_c::load( string16_c const & file_path_relative )
	{
		un_load();
#if defined( cheonsa_platform_windows )
		string16_c file_path_absolute;
		if ( global_engine_instance.interfaces.content_manager->resolve_file_path( file_path_relative, file_path_absolute ) )
		{
			icon_handle = static_cast< void_c * >( LoadImageW( NULL, file_path_absolute.character_list.get_internal_array(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE ) );
		}
#endif
		return icon_handle != nullptr;
	}

	void_c platform_icon_c::un_load()
	{
		if ( icon_handle != nullptr )
		{
			DestroyIcon( static_cast< HICON >( icon_handle ) );
			icon_handle = nullptr;
		}
	}

}
