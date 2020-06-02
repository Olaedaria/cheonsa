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

	boolean_c platform_icon_c::load( string16_c const & relative_file_path )
	{
		un_load();
#if defined( cheonsa_platform_windows )
		string16_c absolute_file_path;
		if ( engine.get_content_manager()->resolve_absolute_file_path( relative_file_path, absolute_file_path, true, true ) )
		{
			icon_handle = static_cast< void_c * >( LoadImageW( NULL, absolute_file_path.character_list.get_internal_array(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE ) );
		}
#endif
		return icon_handle;
	}

	void_c platform_icon_c::un_load()
	{
		if ( icon_handle )
		{
			DestroyIcon( static_cast< HICON >( icon_handle ) );
			icon_handle = nullptr;
		}
	}

}
