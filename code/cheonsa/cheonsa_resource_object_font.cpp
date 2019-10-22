#include "cheonsa_resource_object_font.h"
#include "cheonsa_data_scribe_binary.h"
#include "cheonsa_menu_element_text.h"
#include "cheonsa_ops.h"
#include "cheonsa_engine.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_SIZES_H

namespace cheonsa
{

	uint8_c const resource_object_font_c::_quantized_sizes[ resource_object_font_c::_quantized_count ] = { 12, 24, 32, 48 };

	core_linked_list_c< resource_object_font_c * > resource_object_font_c::_global_list;

	glyph_key_c::glyph_key_c()
		: font_file_hash( 0 )
		, code_point( 0 )
		, quantized_size( 0 )
		, unused( 0 )
	{
	}

	glyph_key_c::glyph_key_c( uint32_c font_file_hash, float32_c font_size, char16_c code_point )
		: font_file_hash( font_file_hash )
		, code_point( code_point )
		, quantized_size( resource_object_font_c::get_quantized_size( font_size ) )
		, unused( 0 )
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
		, unused{}
	{
	}

	resource_object_font_c::size_metrics_c::size_metrics_c()
		: free_type_size_handle( nullptr )
		, quantized_size( 0 )
		, ascender( 0.0f )
		, descender( 0.0f )
		, space_horizontal_advance( 0.0f )
	{
	}

	boolean_c resource_object_font_c::size_metrics_c::_load( void_c * free_type_face_handle, uint8_c quantized_size )
	{
		assert( free_type_size_handle == nullptr );
		assert( quantized_size == get_quantized_size( quantized_size ) );

		this->quantized_size = quantized_size;

		FT_Error free_type_error = FT_New_Size( reinterpret_cast< FT_Face >( free_type_face_handle ), reinterpret_cast< FT_Size * >( &free_type_size_handle ) );
		if ( free_type_error != 0 )
		{
			goto clean_up;
		}
		free_type_error = FT_Activate_Size( reinterpret_cast< FT_Size >( free_type_size_handle ) );
		if ( free_type_error != 0 )
		{
			goto clean_up;
		}
		free_type_error = FT_Set_Pixel_Sizes( reinterpret_cast< FT_Face >( free_type_face_handle ), 0, quantized_size );
		if ( free_type_error != 0 )
		{
			goto clean_up;
		}
		ascender = static_cast< float32_c >( reinterpret_cast< FT_Face >( free_type_face_handle )->size->metrics.ascender ) / 64.0f;
		descender = static_cast< float32_c >( reinterpret_cast< FT_Face >( free_type_face_handle )->size->metrics.descender ) / 64.0f;

		free_type_error = FT_Load_Char( reinterpret_cast< FT_Face >( free_type_face_handle ), ' ', FT_LOAD_DEFAULT );
		if ( free_type_error != 0 )
		{
			goto clean_up;
		}
		space_horizontal_advance = static_cast< float32_c >( reinterpret_cast< FT_Face >( free_type_face_handle )->glyph->advance.x ) / 64.0f;

		return true;

	clean_up:
		_unload();
		return false;
	}

	void_c resource_object_font_c::size_metrics_c::_unload()
	{
		if ( free_type_size_handle != nullptr )
		{
			FT_Done_Size( reinterpret_cast< FT_Size >( free_type_size_handle ) );
			free_type_size_handle = nullptr;
		}
		quantized_size = 0;
		ascender = 0.0f;
		descender = 0.0f;
		space_horizontal_advance = 0.0f;
	}

	boolean_c resource_object_font_c::_load( data_stream_c * stream )
	{
		assert( stream->get_position() == 0 );

		_file_size = stream->get_size();
		_file = new uint8_c[ _file_size ];
		stream->load( _file, _file_size );

		FT_Error free_type_error = FT_New_Memory_Face( reinterpret_cast< FT_Library >( global_engine_instance.interfaces.glyph_manager->_free_type_library_handle ), reinterpret_cast< FT_Byte * >( _file ), static_cast< FT_Long >( _file_size ), 0, reinterpret_cast< FT_Face * >( &_free_type_face_handle ) );
		if ( free_type_error )
		{
			goto clean_up;
		}

		for ( sint32_c i = 0; i < _quantized_count; i++ )
		{
			if ( !_quantized_size_metrics[ i ]._load( _free_type_face_handle, static_cast< uint8_c >( _quantized_sizes[ i ] ) ) )
			{
				goto clean_up;
			}
		}

		_file_hash = ops::hash_jenkins_uint32( _file, _file_size );
		_is_loaded = true;

		menu_element_text_c::invalidate_glyph_layout_of_all_instances_with_font( this );

		return true;

	clean_up:
		if ( _free_type_face_handle != nullptr )
		{
			for ( sint32_c i = 0; i < _quantized_count; i++ )
			{
				_quantized_size_metrics[ i ]._unload();
			}
			FT_Done_Face( reinterpret_cast< FT_Face >( _free_type_face_handle ) );
			_free_type_face_handle = nullptr;
		}
		if ( _file != nullptr )
		{
			delete[] _file;
			_file = nullptr;
			_file_size = 0;
		}
		_file_hash = 0;
		return false;
	}

	void_c resource_object_font_c::_unload()
	{
		assert( _is_loaded == true );
		_is_loaded = false;
		on_unload.invoke( this );

		delete[] _file;
		_file = nullptr;
		_file_size = 0;

		for ( sint32_c i = 0; i < _quantized_count; i++ )
		{
			_quantized_size_metrics[ i ]._unload();
		}
		FT_Done_Face( reinterpret_cast< FT_Face >( _free_type_face_handle ) );
		_free_type_face_handle = nullptr;

		menu_element_text_c::invalidate_glyph_layout_of_all_instances_with_font( this );
	}

	resource_object_font_c::resource_object_font_c()
		: resource_object_c()
		, _global_list_node( this )
		, _file( nullptr )
		, _file_size( 0 )
		, _file_hash( 0 )
		, _free_type_face_handle( nullptr )
		, _quantized_size_metrics()
	{
		_global_list.insert_at_end( &_global_list_node );
	}

	resource_object_font_c::~resource_object_font_c()
	{
		assert( _is_loaded == false );
		_global_list.remove( &_global_list_node );
	}

	uint32_c resource_object_font_c::get_file_hash() const
	{
		return _file_hash;
	}

	void_c * resource_object_font_c::get_free_type_face_handle() const
	{
		return _free_type_face_handle;
	}

	float32_c resource_object_font_c::get_unquantized_ascender( float32_c size ) const
	{
		assert( _is_loaded );
		size_metrics_c const * size_metrics = get_quantized_size_metrics( size );
		return size_metrics->ascender * ( size / static_cast< float32_c >( size_metrics->quantized_size ) );
	}

	float32_c resource_object_font_c::get_unquantized_descender( float32_c size ) const
	{
		assert( _is_loaded );
		size_metrics_c const * size_metrics = get_quantized_size_metrics( size );
		return size_metrics->descender * ( size / static_cast< float32_c >( size_metrics->quantized_size ) );
	}

	float32_c resource_object_font_c::get_unquantized_horizontal_advance_for_space( float32_c size ) const
	{
		assert( _is_loaded );
		size_metrics_c const * size_metrics = get_quantized_size_metrics( size );
		return size_metrics->space_horizontal_advance * ( size / static_cast< float32_c >( size_metrics->quantized_size ) );
	}

	float32_c resource_object_font_c::find_kern_advance( float32_c left_font_size, char16_c left_code_point, float32_c right_font_size, char16_c right_code_point )
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

			//size_metrics_c const * right_size_metrics = _get_quantized_size_metrics( right_font_size );
			//FT_Activate_Size( reinterpret_cast< FT_Size >( right_size_metrics->free_type_size_handle ) );
			//FT_Vector right_kerning;
			//free_type_error = FT_Get_Kerning( reinterpret_cast< FT_Face >( _free_type_face_handle ), FT_Get_Char_Index( reinterpret_cast< FT_Face >( _free_type_face_handle ), left_code_point ), FT_Get_Char_Index( reinterpret_cast< FT_Face >( _free_type_face_handle ), right_code_point ), FT_KERNING_UNFITTED, &right_kerning );
			//if ( free_type_error )
			//{
			//	return 0.0f;
			//}
			//return ( static_cast< float32_c >( left_kerning.x ) / 64.0f ) * ( left_font_size / right_font_size );
		}
	}

	resource_object_font_c::size_metrics_c const * resource_object_font_c::get_quantized_size_metrics( float32_c size ) const
	{
		for ( sint32_c i = 0; i < _quantized_count - 1; i++ )
		{
			if ( size <= _quantized_sizes[ i ] )
			{
				return &_quantized_size_metrics[ i ];
			}
		}
		return &_quantized_size_metrics[ _quantized_count - 1 ];
	}

	uint8_c resource_object_font_c::get_quantized_size( float32_c size )
	{
		for ( sint32_c i = 0; i < _quantized_count - 1; i++ )
		{
			if ( size <= _quantized_sizes[ i ] )
			{
				return _quantized_sizes[ i ];
			}
		}
		return _quantized_sizes[ _quantized_count - 1 ];
	}

	float32_c resource_object_font_c::get_scale_to_unquantize_size( float32_c size )
	{
		return size / static_cast< float32_c >( get_quantized_size( size ) );
	}

}
