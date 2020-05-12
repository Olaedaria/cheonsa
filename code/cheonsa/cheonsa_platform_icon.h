﻿#pragma once

#include "cheonsa_types.h"
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
		boolean_c load( string16_c const & relative_file_path );
		void_c un_load();

	};

}
