#include "cheonsa_resource_file_model.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"
#include "cheonsa_debug_manager.h"

namespace cheonsa
{

	void_c resource_file_model_c::data_c::reset()
	{
		mesh_bone_name_list.remove_all();
		mesh_list.remove_all();
		mesh_draw_list.remove_all();
		mesh_vertex_list_base.remove_all();
		mesh_vertex_list_bone_weight.remove_all();
		mesh_index_list.remove_all();
		bone_list.remove_all();
		bone_logic_list.remove_all();
		bone_logic_property_list.remove_all();
		attachment_point_list.remove_all();
		animation_list.remove_all();
		animation_object_list.remove_all();
		animation_property_list.remove_all();
		animation_key_list.remove_all();
		animation_event_list.remove_all();
		physics_body_list.remove_all();
		physics_shape_list.remove_all();
		physics_vertex_list.remove_all();
		physics_index_list.remove_all();
		physics_constraint_list.remove_all();
		physics_parameter_list.remove_all();
		light_list.remove_all();
		property_list.remove_all();
		string_table.remove_all();
		bone_extras_list.remove_all();

		_cpu_vertex_buffer_mesh_base.remove_all();
		if ( _gpu_vertex_buffer_mesh_base != nullptr )
		{
			delete _gpu_vertex_buffer_mesh_base;
			_gpu_vertex_buffer_mesh_base = nullptr;
		}
		_cpu_vertex_buffer_mesh_bone_weight.remove_all();
		if ( _gpu_vertex_buffer_mesh_bone_weight != nullptr )
		{
			delete _gpu_vertex_buffer_mesh_bone_weight;
			_gpu_vertex_buffer_mesh_bone_weight = nullptr;
		}
		if ( _gpu_mesh_index_buffer != nullptr )
		{
			delete _gpu_mesh_index_buffer;
			_gpu_mesh_index_buffer = nullptr;
		}
	}

	sint32_c resource_file_model_c::find_bone_index_with_bone_name( uint16_c bone_name )
	{
		if ( bone_name > 0 )
		{
			for ( sint32_c i = 0; i < _data.bone_list.get_length(); i++ )
			{
				if ( bone_name == _data.bone_list[ i ].name )
				{
					return i;
				}
			}
		}
		return -1;
	}

	sint32_c resource_file_model_c::find_bone_index_with_bone_name( string8_c const & bone_name )
	{
		if ( bone_name.get_length() > 0 )
		{
			for ( sint32_c i = 0; i < _data.bone_list.get_length(); i++ )
			{
				if ( bone_name == &_data.string_table[ _data.bone_list[ i ].name ] )
				{
					return i;
				}
			}
		}
		return -1;
	}

	boolean_c resource_file_model_c::find_bone_logic_property_as_sint32( bone_logic_c const * logic, string8_c const & name, sint32_c & value )
	{
		assert( _is_loaded );
		assert( logic != nullptr );
		value = 0;
		for ( uint32_c i = logic->property_start; i < logic->property_end; i++ )
		{
			bone_logic_property_c const * bone_logic_property = &_data.bone_logic_property_list[ i ];
			if ( name == &_data.string_table[ bone_logic_property->name ] )
			{
				string8_c value_string( core_list_mode_e_static_volatile, &_data.string_table[ bone_logic_property->value ] );
				return ops::convert_string8_to_sint32( value_string, value );
			}
		}
		return false;
	}

	boolean_c resource_file_model_c::find_bone_logic_property_as_float32( bone_logic_c const * logic, string8_c const & name, float32_c & value )
	{
		assert( _is_loaded );
		assert( logic != nullptr );
		value = 0.0f;
		for ( uint32_c i = logic->property_start; i < logic->property_end; i++ )
		{
			bone_logic_property_c const * bone_logic_property = &_data.bone_logic_property_list[ i ];
			if ( name == &_data.string_table[ bone_logic_property->name ] )
			{
				string8_c value_string( core_list_mode_e_static_volatile, &_data.string_table[ bone_logic_property->value ] );
				return ops::convert_string8_to_float32( value_string, value );
			}
		}
		return false;
	}

	boolean_c resource_file_model_c::find_bone_logic_property_as_string8( bone_logic_c const * logic, string8_c const & name, string8_c & value )
	{
		assert( _is_loaded );
		assert( logic != nullptr );
		value = "";
		for ( uint32_c i = logic->property_start; i < logic->property_end; i++ )
		{
			bone_logic_property_c const * bone_logic_property = &_data.bone_logic_property_list[ i ];
			if ( name == &_data.string_table[ bone_logic_property->name ] )
			{
				value = string8_c( core_list_mode_e_static_volatile, &_data.string_table[ bone_logic_property->value ] );
				return true;
			}
		}
		return false;
	}

	// aligns input value to next nearest multiple of 16.
	sint32_c align_data_offset( sint32_c offset )
	{
		if ( offset % 16 != 0 )
		{
			offset += 16 - ( offset % 16 );
		}
		return offset;
	}

	void_c resource_file_model_c::_load( data_stream_c * stream )
	{
		assert( stream != nullptr );
		assert( _is_loaded == false );

		sint32_c stream_size = stream->get_size();

		data_scribe_binary_c scribe;
		scribe.open( stream, ops::get_native_byte_order() );

		// check signature.
		char8_c signature_and_version[ 4 ];
		if ( !stream->load( signature_and_version, 4 ) )
		{
			goto cancel;
		}
		if ( signature_and_version[ 0 ] == 'c' && signature_and_version[ 1 ] == 'm' )
		{
			if ( ops::get_native_byte_order() != byte_order_e_little )
			{
				debug_log( log_type_e_information, L"resource_file_model_c::_load() : loading file where byte order is little endian but this environment's native byte order is big endian." );
			}
			scribe.set_byte_order_( byte_order_e::byte_order_e_little );
		}
		else if ( signature_and_version[ 0 ] == 'C' && signature_and_version[ 1 ] == 'M' )
		{
			if ( ops::get_native_byte_order() != byte_order_e_big )
			{
				debug_log( log_type_e_information, L"resource_file_model_c::_load() : loading file where byte order is big endian but this environment's native byte order is little endian." );
			}
			scribe.set_byte_order_( byte_order_e::byte_order_e_big );
		}
		else
		{
			goto cancel;
		}

		// check version.
		if ( !ops::memory_compare( &signature_and_version[ 2 ], "01", 2 ) )
		{
			goto cancel;
		}

		// if the file's byte order is the same as the environment's then load the whole file into memory at once.
		if ( scribe.get_byte_order() == ops::get_native_byte_order() )
		{
			_raw_data_size = stream->get_size();
			_raw_data = new uint8_c[ _raw_data_size ];
			if ( !stream->set_position( 0 ) )
			{
				goto cancel;
			}
			if ( !stream->load( _raw_data, _raw_data_size ) )
			{
				goto cancel;
			}
			if ( !stream->set_position( 4 ) )
			{
				goto cancel;
			}
		}

		// load chunks.
		sint32_c chunk_count = 0;
		if ( !scribe.load_sint32( chunk_count ) )
		{
			goto cancel;
		}
		for ( sint32_c i = 0; i < chunk_count; i++ )
		{
			chunk_header_c chunk_header;
			if ( !chunk_header.load( scribe ) )
			{
				goto cancel;
			}

			//
			//
			// string_table
			if ( ops::memory_compare( chunk_header.signature, string_table_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.string_table.construct_mode_static_from_array( reinterpret_cast< char8_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.data_size );
				}
				else
				{
					_data.string_table.set_length_absolute( chunk_header.data_size );
					if ( _data.string_table.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					if ( !stream->load( _data.string_table.get_internal_array(), chunk_header.data_size ) )
					{
						goto cancel;
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// mesh_bone_name_list
			else if ( ops::memory_compare( chunk_header.signature, mesh_bone_name_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.mesh_bone_name_list.construct_mode_static_from_array( reinterpret_cast< mesh_bone_name_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.mesh_bone_name_list.set_length_absolute( chunk_header.count );
					if ( _data.mesh_bone_name_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !scribe.load_uint16( _data.mesh_bone_name_list[ j ].name ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// mesh_list
			else if ( ops::memory_compare( chunk_header.signature, mesh_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.mesh_list.construct_mode_static_from_array( reinterpret_cast< mesh_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.mesh_list.set_length_absolute( chunk_header.count );
					if ( _data.mesh_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.mesh_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// mesh_draw_list
			else if ( ops::memory_compare( chunk_header.signature, mesh_draw_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.mesh_draw_list.construct_mode_static_from_array( reinterpret_cast< mesh_draw_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.mesh_draw_list.set_length_absolute( chunk_header.count );
					if ( _data.mesh_draw_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.mesh_draw_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// mesh_vertex_list_base
			else if ( ops::memory_compare( chunk_header.signature, mesh_vertex_base_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.mesh_vertex_list_base.construct_mode_static_from_array( reinterpret_cast< mesh_vertex_base_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.mesh_vertex_list_base.set_length_absolute( chunk_header.count );
					if ( _data.mesh_vertex_list_base.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.mesh_vertex_list_base[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// mesh_vertex_list_bone_weight
			else if ( ops::memory_compare( chunk_header.signature, mesh_vertex_bone_weight_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.mesh_vertex_list_bone_weight.construct_mode_static_from_array( reinterpret_cast< mesh_vertex_bone_weight_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.mesh_vertex_list_bone_weight.set_length_absolute( chunk_header.count );
					if ( _data.mesh_vertex_list_bone_weight.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.mesh_vertex_list_bone_weight[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// mesh_index_list
			else if ( ops::memory_compare( chunk_header.signature, mesh_index_c::get_signature(), 4 ) )
			{
				if ( _data.mesh_vertex_list_base.get_length() <= 65535 )
				{
					// 16-bit indices safety check.
					if ( sizeof( uint16_c ) * chunk_header.count != chunk_header.data_size )
					{
						goto cancel;
					}
				}
				else
				{
					// 32-bit indices safety check.
					if ( sizeof( uint32_c ) * chunk_header.count != chunk_header.data_size )
					{
						goto cancel;
					}
				}
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					if ( _data.mesh_vertex_list_base.get_length() <= 65535 )
					{
						// 16-bit indices.
						_data.mesh_index_list.construct_mode_static_from_array( reinterpret_cast< uint16_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
					}
					else
					{
						// 32-bit indices.
						_data.mesh_index_list.construct_mode_static_from_array( reinterpret_cast< uint16_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count * 2 );
					}
				}
				else
				{
					if ( _data.mesh_vertex_list_base.get_length() <= 65535 )
					{
						// 16-bit indices.
						_data.mesh_index_list.set_length_absolute( chunk_header.count );
						if ( _data.mesh_index_list.get_internal_array_size() != chunk_header.data_size )
						{
							goto cancel;
						}
						sint32_c return_stream_position = stream->get_position();
						if ( !stream->set_position( chunk_header.data_offset ) )
						{
							goto cancel;
						}
						for ( sint32_c j = 0; j < chunk_header.count; j++ )
						{
							if ( !scribe.load_uint16( _data.mesh_index_list[ j ] ) )
							{
								goto cancel;
							}
						}
						if ( !stream->set_position( return_stream_position ) )
						{
							goto cancel;
						}
					}
					else
					{
						// 32-bit indices.
						_data.mesh_index_list.set_length_absolute( chunk_header.count * 2 );
						if ( _data.mesh_index_list.get_internal_array_size() != chunk_header.data_size )
						{
							goto cancel;
						}
						sint32_c return_stream_position = stream->get_position();
						if ( !stream->set_position( chunk_header.data_offset ) )
						{
							goto cancel;
						}
						uint32_c * mesh_index_array = reinterpret_cast< uint32_c * >( _data.mesh_index_list.get_internal_array() );
						for ( sint32_c j = 0; j < chunk_header.count; j++ )
						{
							if ( !scribe.load_uint32( mesh_index_array[ j ] ) )
							{
								goto cancel;
							}
						}
						if ( !stream->set_position( return_stream_position ) )
						{
							goto cancel;
						}
					}
				}
			}

			//
			//
			// mesh_shape_key_vertex_list
			else if ( ops::memory_compare( chunk_header.signature, mesh_shape_key_vertex_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.mesh_shape_key_vertex_list.construct_mode_static_from_array( reinterpret_cast< mesh_shape_key_vertex_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.mesh_shape_key_vertex_list.set_length_absolute( chunk_header.count );
					if ( _data.bone_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.mesh_shape_key_vertex_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// mesh_shape_key_list
			else if ( ops::memory_compare( chunk_header.signature, mesh_shape_key_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.mesh_shape_key_list.construct_mode_static_from_array( reinterpret_cast< mesh_shape_key_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.mesh_shape_key_list.set_length_absolute( chunk_header.count );
					if ( _data.mesh_shape_key_list.get_internal_array_size() == chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j =0; j < chunk_header.count; j++ )
					{
						if ( !_data.mesh_shape_key_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// bone_list
			else if ( ops::memory_compare( chunk_header.signature, bone_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.bone_list.construct_mode_static_from_array( reinterpret_cast< bone_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.bone_list.set_length_absolute( chunk_header.count );
					if ( _data.bone_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.bone_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// bone_logic_list
			else if ( ops::memory_compare( chunk_header.signature, bone_logic_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.bone_logic_list.construct_mode_static_from_array( reinterpret_cast< bone_logic_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.bone_logic_list.set_length_absolute( chunk_header.count );
					if ( _data.bone_logic_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.bone_logic_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// bone_logic_property_list
			else if ( ops::memory_compare( chunk_header.signature, bone_logic_property_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.bone_logic_property_list.construct_mode_static_from_array( reinterpret_cast< bone_logic_property_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.bone_logic_property_list.set_length_absolute( chunk_header.count );
					if ( _data.bone_logic_property_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.bone_logic_property_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// bone_attachment_list
			else if ( ops::memory_compare( chunk_header.signature, attachment_point_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.attachment_point_list.construct_mode_static_from_array( reinterpret_cast< attachment_point_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.attachment_point_list.set_length_absolute( chunk_header.count );
					if ( _data.attachment_point_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.attachment_point_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// animation_list
			else if ( ops::memory_compare( chunk_header.signature, animation_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.animation_list.construct_mode_static_from_array( reinterpret_cast< animation_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.animation_list.set_length_absolute( chunk_header.count );
					if ( _data.animation_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.animation_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// animation_object_list
			else if ( ops::memory_compare( chunk_header.signature, animation_object_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.animation_object_list.construct_mode_static_from_array( reinterpret_cast< animation_object_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.animation_object_list.set_length_absolute( chunk_header.count );
					if ( _data.animation_object_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.animation_object_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// animation_property_list
			else if ( ops::memory_compare( chunk_header.signature, animation_property_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.animation_property_list.construct_mode_static_from_array( reinterpret_cast< animation_property_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.animation_property_list.set_length_absolute( chunk_header.count );
					if ( _data.animation_property_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.animation_property_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// animation_event_list
			else if ( ops::memory_compare( chunk_header.signature, animation_event_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.animation_event_list.construct_mode_static_from_array( reinterpret_cast< animation_event_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.animation_event_list.set_length_absolute( chunk_header.count );
					if ( _data.animation_event_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.animation_event_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// physics_body_list
			else if ( ops::memory_compare( chunk_header.signature, physics_rigid_body_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.physics_body_list.construct_mode_static_from_array( reinterpret_cast< physics_rigid_body_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.physics_body_list.set_length_absolute( chunk_header.count );
					if ( _data.physics_body_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.physics_body_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// physics_shape_list
			else if ( ops::memory_compare( chunk_header.signature, physics_shape_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.physics_shape_list.construct_mode_static_from_array( reinterpret_cast< physics_shape_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.physics_shape_list.set_length_absolute( chunk_header.count );
					if ( _data.physics_shape_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.physics_shape_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// physics_vertex_list
			else if ( ops::memory_compare( chunk_header.signature, physics_vertex_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.physics_vertex_list.construct_mode_static_from_array( reinterpret_cast< physics_vertex_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.physics_vertex_list.set_length_absolute( chunk_header.count );
					if ( _data.physics_vertex_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.physics_vertex_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// physics_index_list
			else if ( ops::memory_compare( chunk_header.signature, physics_index_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.physics_index_list.construct_mode_static_from_array( reinterpret_cast< physics_index_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.physics_index_list.set_length_absolute( chunk_header.count );
					if ( _data.physics_index_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.physics_index_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// physics_constraint_list
			else if ( ops::memory_compare( chunk_header.signature, physics_constraint_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.physics_constraint_list.construct_mode_static_from_array( reinterpret_cast< physics_constraint_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.physics_constraint_list.set_length_absolute( chunk_header.count );
					if ( _data.physics_constraint_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.physics_constraint_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// physics_parameter_list
			else if ( ops::memory_compare( chunk_header.signature, physics_parameter_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.physics_parameter_list.construct_mode_static_from_array( reinterpret_cast< physics_parameter_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.physics_parameter_list.set_length_absolute( chunk_header.count );
					if ( _data.physics_parameter_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.physics_parameter_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// light_list
			else if ( ops::memory_compare( chunk_header.signature, light_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.light_list.construct_mode_static_from_array( reinterpret_cast< light_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.light_list.set_length_absolute( chunk_header.count );
					if ( _data.light_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.light_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}

			//
			//
			// property_list
			else if ( ops::memory_compare( chunk_header.signature, property_c::get_signature(), 4 ) )
			{
				if ( scribe.get_byte_order() == ops::get_native_byte_order() )
				{
					_data.property_list.construct_mode_static_from_array( reinterpret_cast< property_c const * >( &_raw_data[ chunk_header.data_offset ] ), chunk_header.count );
				}
				else
				{
					_data.property_list.set_length_absolute( chunk_header.count );
					if ( _data.property_list.get_internal_array_size() != chunk_header.data_size )
					{
						goto cancel;
					}
					sint32_c return_stream_position = stream->get_position();
					if ( !stream->set_position( chunk_header.data_offset ) )
					{
						goto cancel;
					}
					for ( sint32_c j = 0; j < chunk_header.count; j++ )
					{
						if ( !_data.property_list[ j ].load( scribe ) )
						{
							goto cancel;
						}
					}
					if ( !stream->set_position( return_stream_position ) )
					{
						goto cancel;
					}
				}
			}
		}

		// we could do some safety checks here.
		// ...

		// calculate post-load extras for bones.
		_data.bone_extras_list.construct_mode_dynamic( _data.bone_list.get_length(), _data.bone_list.get_length() );
		for ( sint32_c i = 0; i < _data.bone_extras_list.get_length(); i++ )
		{
			bone_c const & bone = _data.bone_list[ i ];
			bone_extras_c & bone_extras = _data.bone_extras_list.get_internal_array()[ i ];

			// calculate bone length.
			bone_extras.length = ops::length_float32( vector32x3_c( bone.tail_position ) - vector32x3_c( bone.head_position ) );
			
			// calculate object space rest transform.
			bone_extras.object_space_rest_transform = make_bone_transform( vector64x3_c( bone.head_position ), vector64x3_c( bone.tail_position ), bone.roll, vector32x3_c( 0.0, 0.0, 1.0 ) );
			bone_extras.object_space_rest_transform_inverted = bone_extras.object_space_rest_transform.get_inverted();

			// calculate local rest transform.
			sint32_c mother_bone_index = find_bone_index_with_bone_name( bone.mother_bone_name );
			if ( mother_bone_index >= 0 )
			{
				assert( mother_bone_index < i ); // flat list, ordered by hierarchy, means that our mother bone should have its post-load extras data calculated by now.
				bone_extras.local_space_rest_transform = bone_extras.object_space_rest_transform * _data.bone_extras_list[ mother_bone_index ].object_space_rest_transform_inverted;
			}
			else
			{
				bone_extras.local_space_rest_transform = bone_extras.object_space_rest_transform;
			}
		}

		// calculate mesh bounds for static mesh.
		_data.mesh_box.minimum = vector32x3_c( _data.mesh_vertex_list_base[ 0 ].position );
		_data.mesh_box.maximum = vector32x3_c( _data.mesh_vertex_list_base[ 0 ].position );
		for ( sint32_c i = 1; i < _data.mesh_vertex_list_base.get_length(); i++ )
		{
			_data.mesh_box.accumulate_bounds( vector32x3_c( _data.mesh_vertex_list_base[ i ].position ) );
		}

		// create gpu resources.
		// gpus are always little endian as far as i know, we may need to swap endianness.
		if ( _data.mesh_vertex_list_base.get_length() > 0 )
		{
			_data._cpu_vertex_buffer_mesh_base.construct_mode_static_from_array( reinterpret_cast< video_renderer_vertex_mesh_base_c * >( _data.mesh_vertex_list_base.get_internal_array() ), _data.mesh_vertex_list_base.get_length() );
			_data._gpu_vertex_buffer_mesh_base = engine.get_video_interface()->create_vertex_buffer( &video_renderer_interface_c::vertex_format_mesh_base, _data._cpu_vertex_buffer_mesh_base.get_length(), _data._cpu_vertex_buffer_mesh_base.get_internal_array(), _data._cpu_vertex_buffer_mesh_base.get_internal_array_size(), false, false, false );
		}
		if ( _data.mesh_vertex_list_bone_weight.get_length() > 0 )
		{
			_data._cpu_vertex_buffer_mesh_bone_weight.construct_mode_static_from_array( reinterpret_cast< video_renderer_vertex_mesh_bone_weight_c * >( _data.mesh_vertex_list_bone_weight.get_internal_array() ), _data.mesh_vertex_list_bone_weight.get_length() );
			_data._gpu_vertex_buffer_mesh_bone_weight = engine.get_video_interface()->create_vertex_buffer( &video_renderer_interface_c::vertex_format_mesh_base, _data._cpu_vertex_buffer_mesh_bone_weight.get_length(), _data._cpu_vertex_buffer_mesh_bone_weight.get_internal_array(), _data._cpu_vertex_buffer_mesh_bone_weight.get_internal_array_size(), false, false, false );
		}
		if ( _data.mesh_index_list.get_length() > 0 )
		{
			if ( _data.mesh_vertex_list_base.get_length() <= 65535 )
			{
				_data._gpu_mesh_index_buffer = engine.get_video_interface()->create_index_buffer( video_index_format_e_uint16, _data.mesh_index_list.get_internal_array_size() / sizeof( uint16_c ), _data.mesh_index_list.get_internal_array(), _data.mesh_index_list.get_internal_array_size(), false, false );
			}
			else
			{
				_data._gpu_mesh_index_buffer = engine.get_video_interface()->create_index_buffer( video_index_format_e_uint32, _data.mesh_index_list.get_internal_array_size() / sizeof( uint32_c ), _data.mesh_index_list.get_internal_array(), _data.mesh_index_list.get_internal_array_size(), false, false );
			}
		}

		// done.
		_is_loaded = true;

		on_loaded.invoke( this );

		return;

	cancel:
		if ( _raw_data_size > 0 )
		{
			delete[] _raw_data;
			_raw_data = nullptr;
			_raw_data_size = 0;
		}
		_data.reset();
	}

	void_c resource_file_model_c::_unload()
	{
		assert( _is_loaded == true );

		on_unloaded.invoke( this );

		_is_loaded = false;

		_data.reset();
		if ( _raw_data_size > 0 )
		{
			delete[] _raw_data;
			_raw_data = nullptr;
			_raw_data_size = 0;
		}
	}

	resource_file_model_c::resource_file_model_c( string16_c const & file_path )
		: resource_file_c( file_path )
		, _raw_data_size( 0 )
		, _raw_data( nullptr )
		, _data()
	{
	}

	resource_file_model_c::data_c const & resource_file_model_c::get_data() const
	{
		return _data;
	}

	void_c resource_file_model_c::sample_animation_object( sint32_c animation_object_index, float32_c time, vector32x3_c & result_position, quaternion32_c & result_rotation, vector32x3_c & result_scale ) const
	{
		animation_object_c const * animation_object = &_data.animation_object_list[ animation_object_index ];
		for ( uint32_c i = animation_object->property_start; i < animation_object->property_end; i++ )
		{
			animation_property_c const * animation_property = &_data.animation_property_list[ i ];
			animation_key_c const * animation_key_previous = nullptr;
			animation_key_c const * animation_key = nullptr;
			for ( uint32_c j = animation_property->key_start; j < animation_property->key_end; j++ )
			{
				animation_key_previous = animation_key;
				animation_key = &_data.animation_key_list[ j ];
				if ( animation_key->time >= time )
				{
					break;
				}
			}
			if ( animation_property->type == animation_property_c::type_e_position )
			{
				if ( animation_key != nullptr )
				{
					if ( animation_key_previous == nullptr || animation_key->time < time )
					{
						result_position = vector32x3_c( animation_key->value );
					}
					else
					{
						assert( animation_key_previous != nullptr );
						result_position = ops::interpolate_linear( vector32x3_c( animation_key_previous->value ), vector32x3_c( animation_key->value ), ( time - animation_key_previous->time ) / ( animation_key->time - animation_key_previous->time ) );
					}
				}
			}
			else if ( animation_property->type == animation_property_c::type_e_rotation )
			{
				if ( animation_key != nullptr )
				{
					if ( animation_key_previous == nullptr || animation_key->time < time )
					{
						result_rotation = quaternion32_c( animation_key->value );
					}
					else
					{
						assert( animation_key_previous != nullptr );
						result_rotation = quaternion32_c( ops::interpolate_linear( vector32x4_c( animation_key_previous->value ), vector32x4_c( animation_key->value ), ( time - animation_key_previous->time ) / ( animation_key->time - animation_key_previous->time ) ).as_array() );
					}
				}
			}
			else if ( animation_property->type == animation_property_c::type_e_scale )
			{
				if ( animation_key != nullptr )
				{
					if ( animation_key_previous == nullptr || animation_key->time < time )
					{
						result_scale = vector32x3_c( animation_key->value );
					}
					else
					{
						assert( animation_key_previous  != nullptr );
						result_scale = ops::interpolate_linear( vector32x3_c( animation_key_previous->value ), vector32x3_c( animation_key->value ), ( time - animation_key_previous->time ) / ( animation_key->time - animation_key_previous->time ) );
					}
				}
			}
		}
	}

	transform3d_c resource_file_model_c::make_bone_transform( vector64x3_c const & bone_head, vector64x3_c const & bone_tail, float32_c bone_roll, vector32x3_c const & global_up )
	{
		// this algorithm is essentially copied from blender:
		//     https://github.com/blender/blender/blob/master/source/blender/blenkernel/intern/armature.c
		//     void vec_roll_to_mat3_normalized(const float nor[3], const float roll, float mat[3][3])
		//
		// in Blender, the bone's main axis is along the y axis of the matrix.
		// but in cheonsa, the bone's main axis is along the z axis of the matrix.
		// this is the difference and it can cause some confusion.
		// this is also why quaternions exported from blender are swizzled, so that they define an orientation in this space.
		transform3d_c result;
		vector32x3_c axis = ops::normal_vector32x3( vector32x3_c( bone_tail - bone_head ) ); // this is the axis of the bone from head to tail.
		vector32x3_c tangent = ops::cross_product_vector32x3( global_up, axis ); // this is the axis that the bone_rotation will be around.
		if ( ops::length_squared_float32( tangent ) > constants< float32_c >::angle_near_zero() )
		{
			// bone axis is not pointing straight up or straight down.
			float32_c angle = ops::math_arc_cosine( ops::dot_product_float32( global_up, axis ) );
			result.rotation = ops::rotation_quaternion32_from_axis_angle( vector32x4_c( tangent.a, tangent.b, tangent.c, angle ) );
		}
		else
		{
			// bone axis is pointing straight up or straight down.
			float32_c angle = ops::dot_product_float32( global_up, axis ) > 0 ? 0.0f : constants< float32_c >::pi();
			result.rotation = ops::rotation_quaternion32_from_axis_angle( vector32x4_c( global_up.a, global_up.b, global_up.c, angle ) );
		}
		if ( bone_roll != 0.0f )
		{
			result.rotation = result.rotation * ops::rotation_quaternion32_from_axis_angle( vector32x4_c( axis.a, axis.b, axis.c, bone_roll ) );
		}
		result.position = bone_head;
		return result;
	}

	void axis_angle_normalized_to_mat3_ex( matrix32x3x3_c & mat, vector32x3_c const & axis, float32_c const angle_sin, float32_c const angle_cos)
	{
		vector32x3_c nsi;
		float32_c ico;
		float n_00, n_01, n_11, n_02, n_12, n_22;

		//BLI_ASSERT_UNIT_V3(axis);

		/* now convert this to a 3x3 matrix */

		ico = (1.0f - angle_cos);
		nsi.a = axis.a * angle_sin;
		nsi.b = axis.b * angle_sin;
		nsi.c = axis.c * angle_sin;

		n_00 = (axis.a * axis.a) * ico;
		n_01 = (axis.a * axis.b) * ico;
		n_11 = (axis.b * axis.b) * ico;
		n_02 = (axis.a * axis.c) * ico;
		n_12 = (axis.b * axis.c) * ico;
		n_22 = (axis.c * axis.c) * ico;

		mat.a.a = n_00 + angle_cos;
		mat.a.b = n_01 + nsi.c;
		mat.a.c = n_02 - nsi.b;
		mat.b.a = n_01 - nsi.c;
		mat.b.b = n_11 + angle_cos;
		mat.b.c = n_12 + nsi.a;
		mat.c.a = n_02 + nsi.b;
		mat.c.b = n_12 - nsi.a;
		mat.c.c = n_22 + angle_cos;
	}

	void axis_angle_normalized_to_mat3( matrix32x3x3_c & mat, vector32x3_c const & axis, const float angle )
	{
		axis_angle_normalized_to_mat3_ex(mat, axis, sinf(angle), cosf(angle));
	}

	transform3d_c resource_file_model_c::make_bone_transform2( vector64x3_c const & bone_head, vector64x3_c const & bone_tail, float32_c bone_roll )
	{
		// this algorithm is copied from blender:
		//     https://github.com/blender/blender/blob/master/source/blender/blenkernel/intern/armature.c
		//     void vec_roll_to_mat3_normalized(const float nor[3], const float roll, float mat[3][3])

		#define THETA_THRESHOLD_NEGY 1.0e-9f
		#define THETA_THRESHOLD_NEGY_CLOSE 1.0e-5f

		transform3d_c result;

		float32_c theta;
		matrix32x3x3_c bone_roll_matrix;
		matrix32x3x3_c bone_base_matrix;

		vector32x3_c bone_axis = ops::normal_vector32x3( vector32x3_c( bone_tail - bone_head ) );

		theta = 1.0f + bone_axis.b;

		/* With old algo, 1.0e-13f caused T23954 and T31333, 1.0e-6f caused T27675 and T30438,
		* so using 1.0e-9f as best compromise.
		*
		* New algo is supposed much more precise, since less complex computations are performed,
		* but it uses two different threshold values...
		*
		* Note: When theta is close to zero, we have to check we do have non-null X/Z components as well
		*       (due to float precision errors, we can have nor = (0.0, 0.99999994, 0.0)...).
		*/
		if ( theta > THETA_THRESHOLD_NEGY_CLOSE || ( ( bone_axis.a || bone_axis.c ) && theta > THETA_THRESHOLD_NEGY ) )
		{
			/* nor is *not* -Y.
			* We got these values for free... so be happy with it... ;)
			*/
			bone_base_matrix.a.b = -bone_axis.a;
			bone_base_matrix.b.a = bone_axis.a;
			bone_base_matrix.b.b = bone_axis.b;
			bone_base_matrix.b.c = bone_axis.c;
			bone_base_matrix.c.b = -bone_axis.c;
			if ( theta > THETA_THRESHOLD_NEGY_CLOSE )
			{
				/* If nor is far enough from -Y, apply the general case. */
				bone_base_matrix.a.a = 1 - bone_axis.a * bone_axis.a / theta;
				bone_base_matrix.c.c = 1 - bone_axis.c * bone_axis.c / theta;
				bone_base_matrix.c.a = bone_base_matrix.a.c = -bone_axis.a * bone_axis.c / theta;
			}
			else
			{
				/* If nor is too close to -Y, apply the special case. */
				theta = bone_axis.a * bone_axis.a + bone_axis.c * bone_axis.c;
				bone_base_matrix.a.a = ( bone_axis.a + bone_axis.c ) * ( bone_axis.a - bone_axis.c ) / -theta;
				bone_base_matrix.c.c = -bone_base_matrix.a.a;
				bone_base_matrix.c.a = bone_base_matrix.a.c = 2.0f * bone_axis.a * bone_axis.c / theta;
			}
		}
		else
		{
			/* If nor is -Y, simple symmetry by Z axis. */
			bone_base_matrix.a.b = bone_base_matrix.a.c = 0.0;
			bone_base_matrix.b.a = bone_base_matrix.b.c = 0.0;
			bone_base_matrix.a.a = bone_base_matrix.b.b = -1.0;
			bone_base_matrix.c.a = bone_base_matrix.c.b = 0.0;
			bone_base_matrix.c.c = 1.0;
		}

		/* Make Roll matrix */
		axis_angle_normalized_to_mat3( bone_roll_matrix, bone_axis, bone_roll );

		/* Combine and output result */
		//mul_m3_m3m3(mat, rMatrix, bMatrix);
		result.set_rotation_from_unscaled_basis( bone_roll_matrix * bone_base_matrix );
		result.position = bone_head;

		return result;

		#undef THETA_THRESHOLD_NEGY
		#undef THETA_THRESHOLD_NEGY_CLOSE
	}

}
