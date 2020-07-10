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
		unload();
	}

	boolean_c platform_icon_c::load_from_exe( string8_c const & resource_name )
	{
		icon_handle = static_cast< void_c * >( LoadImageA( GetModuleHandleA( NULL ), resource_name.character_list.get_internal_array(), IMAGE_ICON, 0, 0, 0 ) );
		return icon_handle != 0;
	}

	boolean_c platform_icon_c::load_from_exe( string16_c const & resource_name )
	{
		icon_handle = static_cast< void_c * >( LoadImageW( GetModuleHandleW( NULL ), resource_name.character_list.get_internal_array(), IMAGE_ICON, 0, 0, 0 ) );
		return icon_handle != 0;
	}

	boolean_c platform_icon_c::load_from_absolute_file_path( string16_c const & file_path_absolute )
	{
		icon_handle = static_cast< void_c * >( LoadImageW( NULL, file_path_absolute.character_list.get_internal_array(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE ) );
		return icon_handle != 0;
	}

	boolean_c platform_icon_c::load_from_relative_file_path( string16_c const & file_path_relative )
	{
		unload();
#if defined( cheonsa_platform_windows )
		string16_c file_path_absolute;
		if ( engine.get_content_manager()->resolve_absolute_file_path( file_path_relative, file_path_absolute, true, true ) )
		{
			icon_handle = static_cast< void_c * >( LoadImageW( NULL, file_path_absolute.character_list.get_internal_array(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE ) );
		}
#endif
		return icon_handle != 0;
	}

	void_c platform_icon_c::unload()
	{
		if ( icon_handle )
		{
			DestroyIcon( static_cast< HICON >( icon_handle ) );
			icon_handle = nullptr;
		}
	}

}
