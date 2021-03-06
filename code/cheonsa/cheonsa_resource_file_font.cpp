#include "cheonsa_resource_file_font.h"
#include "cheonsa_data_scribe_binary.h"
#include "cheonsa_menu_element_text.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_SIZES_H

namespace cheonsa
{

	glyph_key_c::glyph_key_c()
		: font_file_hash( 0 )
		, code_point( 0 )
		, quantized_size( 0 )
		, unused_0{}
	{
	}

	glyph_key_c::glyph_key_c( uint32_c font_file_hash, float32_c font_size, char16_c code_point )
		: font_file_hash( font_file_hash )
		, code_point( code_point )
		, quantized_size( glyph_manager_c::get_quantized_size( font_size ) )
		, unused_0{}
	{
	}

	boolean_c glyph_key_c::operator == ( glyph_key_c const & other ) const
	{
		return ( font_file_hash == other.font_file_hash ) && ( quantized_size == other.quantized_size ) && ( code_point == other.code_point );
	}

	glyph_c::glyph_c()
		: key()
		, box()
		, map()
		, horizontal_advance( 0.0f )
		, atlas_index( 0 )
		, unused_0{}
		, unused_1{}
	{
	}

	resource_file_font_c::size_metrics_c::size_metrics_c()
		: free_type_size_handle( nullptr )
		, quantized_size( 0 )
		, ascender( 0.0f )
		, descender( 0.0f )
		, space_horizontal_advance( 0.0f )
	{
	}

	boolean_c resource_file_font_c::size_metrics_c::_load( void_c * free_type_face_handle, uint8_c quantized_size )
	{
		assert( free_type_size_handle == nullptr );
		assert( quantized_size == glyph_manager_c::get_quantized_size( quantized_size ) );

		this->quantized_size = quantized_size;

		FT_Error free_type_error = FT_New_Size( reinterpret_cast< FT_Face >( free_type_face_handle ), reinterpret_cast< FT_Size * >( &free_type_size_handle ) );
		if ( free_type_error != 0 )
		{
			goto cancel;
		}
		free_type_error = FT_Activate_Size( reinterpret_cast< FT_Size >( free_type_size_handle ) );
		if ( free_type_error != 0 )
		{
			goto cancel;
		}
		free_type_error = FT_Set_Pixel_Sizes( reinterpret_cast< FT_Face >( free_type_face_handle ), 0, quantized_size );
		if ( free_type_error != 0 )
		{
			goto cancel;
		}
		ascender = static_cast< float32_c >( reinterpret_cast< FT_Face >( free_type_face_handle )->size->metrics.ascender ) / 64.0f;
		descender = static_cast< float32_c >( reinterpret_cast< FT_Face >( free_type_face_handle )->size->metrics.descender ) / 64.0f;

		free_type_error = FT_Load_Char( reinterpret_cast< FT_Face >( free_type_face_handle ), ' ', FT_LOAD_DEFAULT );
		if ( free_type_error != 0 )
		{
			goto cancel;
		}
		space_horizontal_advance = static_cast< float32_c >( reinterpret_cast< FT_Face >( free_type_face_handle )->glyph->advance.x ) / 64.0f;

		return true;

	cancel:
		_unload();
		return false;
	}

	void_c resource_file_font_c::size_metrics_c::_unload()
	{
		if ( free_type_size_handle )
		{
			FT_Done_Size( reinterpret_cast< FT_Size >( free_type_size_handle ) );
			free_type_size_handle = nullptr;
		}
		quantized_size = 0;
		ascender = 0.0f;
		descender = 0.0f;
		space_horizontal_advance = 0.0f;
	}

	void_c resource_file_font_c::_load( data_stream_c * stream )
	{
		assert( stream );
		assert( _is_loaded == false );

		_file_size = stream->get_size();
		_file = new uint8_c[ _file_size ];
		stream->load( _file, _file_size );

		FT_Error free_type_error = FT_New_Memory_Face( reinterpret_cast< FT_Library >( engine.get_glyph_manager()->_free_type_library_handle ), reinterpret_cast< FT_Byte * >( _file ), static_cast< FT_Long >( _file_size ), 0, reinterpret_cast< FT_Face * >( &_free_type_face_handle ) );
		if ( free_type_error )
		{
			goto cancel;
		}

		for ( sint32_c i = 0; i < glyph_manager_c::quantized_count; i++ )
		{
			if ( !_quantized_size_metrics[ i ]._load( _free_type_face_handle, static_cast< uint8_c >( glyph_manager_c::quantized_sizes[ i ] ) ) )
			{
				goto cancel;
			}
		}

		_file_hash = ops::xxhash32( _file, _file_size );

		_is_loaded = true;

		menu_element_text_c::invalidate_glyph_layout_of_all_instances_with_font( this );

		on_loaded.invoke( this );

		return;

	cancel:
		if ( _free_type_face_handle )
		{
			for ( sint32_c i = 0; i < glyph_manager_c::quantized_count; i++ )
			{
				_quantized_size_metrics[ i ]._unload();
			}
			FT_Done_Face( reinterpret_cast< FT_Face >( _free_type_face_handle ) );
			_free_type_face_handle = nullptr;
		}
		if ( _file )
		{
			delete[] _file;
			_file = nullptr;
			_file_size = 0;
		}
		_file_hash = 0;
	}

	void_c resource_file_font_c::_unload()
	{
		assert( _is_loaded == true );

		on_unloaded.invoke( this );

		_is_loaded = false;

		delete[] _file;
		_file = nullptr;
		_file_size = 0;

		for ( sint32_c i = 0; i < glyph_manager_c::quantized_count; i++ )
		{
			_quantized_size_metrics[ i ]._unload();
		}
		FT_Done_Face( reinterpret_cast< FT_Face >( _free_type_face_handle ) );
		_free_type_face_handle = nullptr;

		menu_element_text_c::invalidate_glyph_layout_of_all_instances_with_font( this );
	}

	resource_file_font_c::resource_file_font_c( string16_c const & file_path )
		: resource_file_c( file_path )
		, _file( nullptr )
		, _file_size( 0 )
		, _file_hash( 0 )
		, _free_type_face_handle( nullptr )
		, _quantized_size_metrics()
	{
	}

	resource_file_font_c::~resource_file_font_c()
	{
		assert( _is_loaded == false );
	}

	uint32_c resource_file_font_c::get_file_hash() const
	{
		return _file_hash;
	}

	void_c * resource_file_font_c::get_free_type_face_handle() const
	{
		return _free_type_face_handle;
	}

	float32_c resource_file_font_c::get_unquantized_ascender( float32_c size ) const
	{
		assert( _is_loaded );
		size_metrics_c const * size_metrics = get_quantized_size_metrics( size );
		return size_metrics->ascender * ( size / static_cast< float32_c >( size_metrics->quantized_size ) );
	}

	float32_c resource_file_font_c::get_unquantized_descender( float32_c size ) const
	{
		assert( _is_loaded );
		size_metrics_c const * size_metrics = get_quantized_size_metrics( size );
		return size_metrics->descender * ( size / static_cast< float32_c >( size_metrics->quantized_size ) );
	}

	float32_c resource_file_font_c::get_unquantized_space_horizontal_advance( float32_c size ) const
	{
		assert( _is_loaded );
		size_metrics_c const * size_metrics = get_quantized_size_metrics( size );
		return size_metrics->space_horizontal_advance * ( size / static_cast< float32_c >( size_metrics->quantized_size ) );
	}

	float32_c resource_file_font_c::find_kern_advance( float32_c left_font_size, char16_c left_code_point, float32_c right_font_size, char16_c right_code_point )
	{
		assert( _is_loaded );
		if ( left_font_size == right_font_size )
		{
			size_metrics_c const * size_metrics = get_quantized_size_metrics( left_font_size );
			FT_Activate_Size( reinterpret_cast< FT_Size >( size_metrics->free_type_size_handle ) );
			FT_Vector kerning;
			FT_Error free_type_error = FT_Get_Kerning( reinterpret_cast< FT_Face >( _free_type_face_handle ), FT_Get_Char_Index( reinterpret_cast< FT_Face >( _free_type_face_handle ), left_code_point ), FT_Get_Char_Index( reinterpret_cast< FT_Face >( _free_type_face_handle ), right_code_point ), FT_KERNING_UNFITTED, &kerning );
			if ( free_type_error )
			{
				return 0.0f;
			}
			return ( static_cast< float32_c >( kerning.x ) / 64.0f ) * ( left_font_size / static_cast< float32_c >( size_metrics->quantized_size ) );
		}
		else
		{
			size_metrics_c const * left_size_metrics = get_quantized_size_metrics( left_font_size );
			FT_Activate_Size( reinterpret_cast< FT_Size >( left_size_metrics->free_type_size_handle ) );
			FT_Vector left_kerning;
			FT_Error free_type_error = FT_Get_Kerning( reinterpret_cast< FT_Face >( _free_type_face_handle ), FT_Get_Char_Index( reinterpret_cast< FT_Face >( _free_type_face_handle ), left_code_point ), FT_Get_Char_Index( reinterpret_cast< FT_Face >( _free_type_face_handle ), right_code_point ), FT_KERNING_UNFITTED, &left_kerning );
			if ( free_type_error )
			{
				return 0.0f;
			}
			size_metrics_c const * right_size_metrics = get_quantized_size_metrics( right_font_size );
			return ( static_cast< float32_c >( left_kerning.x ) / 64.0f ) * ( ( left_font_size / static_cast< float32_c >( left_size_metrics->quantized_size ) ) / ( right_font_size / static_cast< float32_c >( right_size_metrics->quantized_size ) ) );
		}
	}

	resource_file_font_c::size_metrics_c const * resource_file_font_c::get_quantized_size_metrics( float32_c size ) const
	{
		for ( sint32_c i = 0; i < glyph_manager_c::quantized_count - 1; i++ )
		{
			if ( size <= glyph_manager_c::quantized_sizes[ i ] )
			{
				return &_quantized_size_metrics[ i ];
			}
		}
		return &_quantized_size_metrics[ glyph_manager_c::quantized_count - 1 ];
	}

}
