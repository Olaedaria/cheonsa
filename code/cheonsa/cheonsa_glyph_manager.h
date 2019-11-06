#pragma once

#include "cheonsa__types.h"
#include "cheonsa_core_list.h"
#include "cheonsa_core_dictionary.h"
#include "cheonsa_memory_allocator_pool.h"
#include "cheonsa_video_types.h"

namespace cheonsa
{

	class resource_file_font_c;

#pragma pack( push, 1 )
	// used to index glyphs in the glyph dictionary.
	// needs to be packed a certain way to prevent the compiler from adding invisible bytes to the structure.
	// this is because the dictionary's generic hash function works on bytes, and those invisible bytes could break it.
	class glyph_key_c
	{
	public:
		uint32_c font_file_hash; // we use this to identify the font instead of a pointer, because it will be persistent across executions and it can be saved to and loaded from the disk cache. it is possible for "dangling" glyphs to accumulate in the disk cache, in the case that the fonts used change between executions.
		char16_c code_point; // code point is just a more technical term for character.
		uint8_c quantized_size; // the size in pixels that this glyph was rendered at (not exact). quantized with get_quantized_size().
		uint8_c unused_0;

	public:
		glyph_key_c();
		glyph_key_c( uint32_c font_file_hash, float32_c font_size, char16_c code_point ); // "code point" is just another name for "character". code_point is the character, size is the font size in IDK, pixels?

		boolean_c operator == ( glyph_key_c const & other ) const;

	};
	static_assert( sizeof( glyph_key_c ) == 8, "size of glyph_key_c is wrong." );

	// represents a glyph that has been rendered to the glyph atlas and has had its measurements calculated.
	// cheonsa 2d coordinate convention is used where x is right and y is down, which is in general different from font metrics where x is right and y is up.
	class glyph_c
	{
	public:
		//box32x2_c apparent_box; // xy coordinates of in pixels of glyph minimum bounding rectangle. needed to know where to position the blinking cursor and selection.
		box32x2_c box; // xy coordinates of textured quad in pixels (including extra margin to contain extent of SDF). relative to cursor origin of rendered glyph. this value is always going to be an integer, but we store it as a float so that we don't have to cast it to a float later when building the vertices.
		box32x2_c map; // uv coordinates of texture map in normalized texture coordinates (including extra margin to contain extent of SDF).
		glyph_key_c key;
		float32_c horizontal_advance; // for horizontal layout, this is how far the glyph advances the glyph run horizontally in pixels.
		uint8_c atlas_index; // the index of the texture array slice of the glyph atlas texture that this glyph's image lives in.
		uint8_c unused_0[ 3 ];
		sint32_c unused_1[ 4 ];

	public:
		glyph_c();

		glyph_c & operator = ( glyph_c const & ) = delete;

	};
	static_assert( sizeof( glyph_c ) == 64, "size of glyph_c is wrong." );
#pragma pack( pop )

	// manages rendering font glyph signed distance fields for all fonts to one or more glyph atlas textures in CPU memory, and then copies them to GPU textures as needed so that they can be accessed by font shaders to render text.
	// manages saving of glyph cash to disk when engine stops, and loading of glyph cache from disk when engine starts.
	// there is one global shared instance, declared in "cheonsa_engine.h", defined in "cheonsa_engine.cpp", it's fully qualified name is "global_engine_instance.interfaces.glyph_manager".
	class glyph_manager_c
	{
	public:
		// manages a CPU texture in which glyphs are rendered to, and a GPU texture which is a copy of the CPU texture, and it manages allocating spaces in the texture for glyph images.
		class glyph_atlas_c
		{
		public:
			class row_c
			{
			public:
				uint8_c quantized_size;
				sint32_c top;
				sint32_c width;
				sint32_c height;

			public:
				row_c();

			};

		public:
			boolean_c _needs_upload; // if true, then the CPU texture needs to be copied to the GPU.
			uint8_c * _texture_data; // texture data buffer. each byte is one pixel, interpret it like a signed distance field.
			sint32_c _texture_data_size; // texture data buffer size in bytes.
			core_list_c< row_c > _row_list; // glyphs are allocated in rows.
			boolean_c _was_modified_during_run_time; // will be set to true to indicate that this glyph atlas needs to be saved to the persistent disk glyph cache upon exit.

		public:
			glyph_atlas_c();
			~glyph_atlas_c();

			void_c reset();

			boolean_c find_space( sint32_c width, sint32_c height, sint32_c & x, sint32_c & y, uint8_c quantized_size );

			void_c set_pixel( sint32_c x, sint32_c y, uint8_c value );

			// copies a glyph from one atlas to another atlas.
			static void_c copy_sub_image( glyph_atlas_c * from, sint32_c from_left, sint32_c from_top, glyph_atlas_c * to, sint32_c to_left, sint32_c to_top, sint32_c width, sint32_c height );

		};

	public:
		memory_allocator_pool_c _glyph_pool; // memory allocator used to allocate the glyph structs that describe glyphs in the atlas, should be faster than the heap for this purpose.
		core_dictionary_c< glyph_key_c, glyph_c const * > _glyph_dictionary;
		glyph_atlas_c * _glyph_atlas_array; // there are as many glyph atlases allocated here as there are texture array slices in the GPU texture. all glyphs from all fonts are rendered by the CPU and stored in this glyph cache. when text needs to be rendered on the GPU, all of the glyph cache textures are bound, and then a texture index in each vertex is used to & the appropriate texture.
		video_texture_c * _glyph_atlas_texture; // a texture2darray of _glyph_atlas_array_slice_count number of slices. format of texture is video_texture_format_e_r8_unorm.
		video_texture_c * _glyph_atlas_staging_texture; // because we can't upload data to a texture2darray, we upload data to this texture then tell the GPU to copy it to a slice in the texture2darray _glyph_atlas_texture.
		void_c * _free_type_library_handle; // we use free type to load fonts and render glyphs.

	public:
		glyph_manager_c();
		~glyph_manager_c();

		boolean_c start();

		glyph_c const * load_quantized_glyph( resource_file_font_c * font, float32_c font_size, char16_c code_point ); // retreives a previously cached glyph or renders and caches a new glyph.

		boolean_c needs_reset; // will be set to true if the glyph cash runs out of space, this will force a reset at the next opportune moment.
		void_c reset(); // clears the glyph cache, and reflows all menu text element instances. this function is supposed to be called in the case that the glyph cache fills up, and works like garbage collection. this may cause the program to hang since the glyph cache will be rebuilt. ideally, this shouldn't need to ever be called, there should be enough glyph atlas texture room to hold all glyphs that would ever be needed concurrently.
		void_c update_glyph_map_texture(); // uploads CPU textures to GPU.

		boolean_c save_to_disk(); // saves the state of this glyph cache to the disk. this only saves if changes were made because it can be kind of slow.
		boolean_c load_from_disk(); // loads the state of this glyph cache from the disk.

	public:
		static sint32_c const glyph_atlas_array_slice_count = 8; // how many texture array slices to use. more slices means more glyphs can be cached at once, and reduces the probability and/or frequency that we will have to rebuild the cache.
		static sint32_c const glyph_atlas_width = 1024; // width of glyph atlas texture in pixels.
		static sint32_c const glyph_atlas_height = 1024; // height of glyph atlas texture in pixels.

		static sint32_c const quantized_count = 4;
		static uint8_c const quantized_sizes[ quantized_count ];
		static sint32_c get_quantized_index( float32_c size );
		static uint8_c get_quantized_size( float32_c size ); // quantizes an arbitrary font size in pixels to a descrete value, which the size that the glyph is rendered and cached at (baked). this is done to increase probability of reuse, such that if the game wants to render a glyph at many different arbitrary sizes, it will render from the cached descrete size and then scale it to the required arbitrary size.
		static uint8_c get_sdf_range( uint8_c quantized_size ); // gets range of signed distance field in pixels for the given quantized size. this contributes to extra glyph box size.

	};

}
