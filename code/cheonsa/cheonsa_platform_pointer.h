#pragma once

#include "cheonsa_types.h"
#include "cheonsa_string16.h"

namespace cheonsa
{

	// wrapper for loading custom mouse pointers/cursors.
	class platform_pointer_c
	{
	public:
		void_c * pointer_handle;

	public:
		platform_pointer_c();
		~platform_pointer_c();
		boolean_c load( string16_c const & file_path );
		void_c unload();

	};

}
