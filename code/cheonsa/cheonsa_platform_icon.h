#pragma once

#include "cheonsa__types.h"
#include "cheonsa_string16.h"

namespace cheonsa
{

	class platform_icon_c
	{
	public:
		void_c * icon_handle;

	public:
		platform_icon_c();
		~platform_icon_c();
		boolean_c load( string16_c const & file_path_relative );
		void_c un_load();

	};

}
