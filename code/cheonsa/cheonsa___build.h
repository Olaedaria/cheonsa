#pragma once

// auto detect which platform (operating system) we are targeting.
#if defined( _WIN32 )
#define cheonsa_platform_windows
#define cheonsa_video_d3d11
#else
#error i don't know where i am.
#endif

// auto detect which bit architecture we are targeting.
// this affects sint_native_c and uint_native_c types.
// this affects dictionary hashes which uses the uint_native_c type.
#if defined( _MSC_VER ) // visual studio c/c++ compiler.
	#if defined( _M_X64 )
		#define cheonsa_64_bit
	#elif defined( _M_IX86 )
		#define cheonsa_32_bit
	#else
		#error i don't know where i am.
	#endif
#elif defined( __GNUC__ ) // gcc.
	#if defined( __x86_64__ ) || defined( __ppc64__ )
		#define cheonsa_64_bit
	#else
		#define cheonsa_32_bit
	#endif
#elif
	#error i don't know where i am.
#endif
