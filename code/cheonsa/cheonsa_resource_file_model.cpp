#include "cheonsa_resource_file_model.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	video_primitive_type_e resource_file_model_c::mesh_draw_c::get_video_primitive_type() const
	{
		switch ( static_cast< primitive_type_e >( primitive_type_ ) )
		{
			case primitive_type_e::triangle_list:
				return video_primitive_type_e_triangle_list;
			case primitive_type_e::triangle_strip:
				return video_primitive_type_e_triangle_strip;
			case primitive_type_e::line_list:
				return video_primitive_type_e_line_list;
			case primitive_type_e::line_strip:
				return video_primitive_type_e_line_strip;
			case primitive_type_e::point_list:
				return video_primitive_type_e_point_list;
		}
		return video_primitive_type_e_point_list;
	}

	uint16_c resource_file_model_c::physics_body_c::get_physics_layer() const
	{
		switch ( static_cast< layer_e >( layer_ ) )
		{
		case layer_e::static_:
			return physics_layer_e_static;
		case layer_e::static_no_clip:
			return physics_layer_e_static_no_clip;
		case layer_e::dynamic:
			return physics_layer_e_dynamic;
		case layer_e::dynamic_no_clip:
			return physics_layer_e_dynamic_no_clip;
		case layer_e::character:
			return physics_layer_e_character;
		case layer_e::character_no_clip:
			return physics_layer_e_character_no_clip;
		case layer_e::camera:
			return physics_layer_e_camera;
		}
		return 0;
	}

	uint16_c resource_file_model_c::physics_body_c::get_physics_layer_mask() const
	{
		return physics_get_layer_mask_for_layer( get_physics_layer() );
	}

	physics_shape_type_e resource_file_model_c::physics_shape_c::get_physics_shape_type() const
	{
		switch ( static_cast< type_e >( type_ ) )
		{
		case type_e::sphere:
			return physics_shape_type_e_sphere;
		case type_e::box:
			return physics_shape_type_e_box;
		case type_e::capsule:
			return physics_shape_type_e_capsule;
		case type_e::cylinder:
			return physics_shape_type_e_cylinder;
		case type_e::cone:
			return physics_shape_type_e_cone;
		case type_e::convex_hull:
			return physics_shape_type_e_convex_hull;
		case type_e::triangle_mesh:
			return physics_shape_type_e_triangle_mesh;
		}
		return physics_shape_type_e_none;
	}

	physics_constraint_type_e resource_file_model_c::physics_constraint_c::get_physics_constraint_type() const
	{
		switch ( static_cast< type_e >( type_ ) )
		{
		case type_e::fixed:
			return physics_constraint_type_e_fixed;
		case type_e::point:
			return physics_constraint_type_e_point;
		case type_e::hinge:
			return physics_constraint_type_e_hinge;
		case type_e::cone:
			return physics_constraint_type_e_cone;
		case type_e::generic:
			return physics_constraint_type_e_generic;
		}
		return physics_constraint_type_e_none;
	}

	scene_light_type_e resource_file_model_c::light_c::get_scene_light_type() const
	{
		switch ( static_cast< type_e >( type_ ) )
		{
		case type_e::direction:
			return scene_light_type_e_direction;
		case type_e::point:
			return scene_light_type_e_point;
		case type_e::cone:
			return scene_light_type_e_cone;
		}
		return scene_light_type_e_point;
	}

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
		bone_attachment_list.remove_all();
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
		light_list.remove_all();
		property_list.remove_all();
		string_table.remove_all();
		bone_extras_list.remove_all();
		if ( mesh_vertex_buffer_base != nullptr )
		{
			delete mesh_vertex_buffer_base;
			mesh_vertex_buffer_base = nullptr;
		}
		if ( mesh_vertex_buffer_bone_weight != nullptr )
		{
			delete mesh_vertex_buffer_bone_weight;
			mesh_vertex_buffer_bone_weight = nullptr;
		}
		if ( mesh_index_buffer != nullptr )
		{
			delete mesh_index_buffer;
			mesh_index_buffer = nullptr;
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
				string8_c value_string( mode_e_static_volatile, &_data.string_table[ bone_logic_property->value ] );
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
				string8_c value_string( mode_e_static_volatile, &_data.string_table[ bone_logic_property->value ] );
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
				value = string8_c( mode_e_static_volatile, &_data.string_table[ bone_logic_property->value ] );
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

	boolean_c resource_file_model_c::_load( data_stream_c * stream )
	{
		assert( stream != nullptr );
		assert( _is_loaded == false );

		// check signature and version.
		char8_c signature[ 4 ];
		if ( !stream->load( signature, 4 ) )
		{
			goto clean_up;
		}
		if ( signature[ 0 ] != 'c' || signature[ 1 ] != 'h' || signature[ 2 ] != 'm' )
		{
			goto clean_up;
		}
		if ( signature[ 3 ] != 1 )
		{
			goto clean_up;
		}
		stream->set_position( 0 );

		// load the whole file into memory.
		sint32_c _raw_data_size = stream->get_size();
		_raw_data = new uint8_c[ _raw_data_size ];
		if ( !stream->load( _raw_data, _raw_data_size ) )
		{
			goto clean_up;
		}

		// extract data from header.
		if ( signature[ 3 ] >= 1 )
		{
			header_v1_c const * header = reinterpret_cast< header_v1_c const * >( _raw_data );

			sint32_c data_offset = align_data_offset( sizeof( header_v1_c ) );

			sint32_c data_end = data_offset + ( header->mesh_bone_name_count * sizeof( uint16_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.mesh_bone_name_list.construct_mode_static_from_array( reinterpret_cast< uint16_c * >( &_raw_data[ data_offset ] ), header->mesh_bone_name_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->mesh_count * sizeof( mesh_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.mesh_list.construct_mode_static_from_array( reinterpret_cast< mesh_c * >( &_raw_data[ data_offset ] ), header->mesh_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->mesh_count * sizeof( mesh_draw_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.mesh_draw_list.construct_mode_static_from_array( reinterpret_cast< mesh_draw_c * >( &_raw_data[ data_offset ] ), header->mesh_draw_count );
			data_offset = align_data_offset( data_end );

			if ( header->mesh_vertex_count > 0 )
			{
				data_end = data_offset + ( header->mesh_vertex_count * sizeof( mesh_vertex_base_c ) );
				if ( data_end > _raw_data_size ) { goto clean_up; }
				_data.mesh_vertex_list_base.construct_mode_static_from_array( reinterpret_cast< mesh_vertex_base_c * >( &_raw_data[ data_offset ] ), header->mesh_vertex_count );
				data_offset = align_data_offset( data_end );
				_data.mesh_vertex_buffer_base = global_engine_instance.interfaces.video_interface->create_vertex_buffer( &video_renderer_interface_c::vertex_format_mesh_base, _data.mesh_vertex_list_base.get_length(), _data.mesh_vertex_list_base.get_internal_array(), _data.mesh_vertex_list_base.get_internal_array_size_used(), false, false, false );

				if ( header->mesh_bone_name_count > 0 )
				{
					data_end = data_offset + ( header->mesh_vertex_count * sizeof( mesh_vertex_bone_weight_c ) );
					if ( data_end > _raw_data_size ) { goto clean_up; }
					_data.mesh_vertex_list_bone_weight.construct_mode_static_from_array( reinterpret_cast< mesh_vertex_bone_weight_c * >( &_raw_data[ data_offset ] ), header->mesh_vertex_count );
					data_offset = align_data_offset( data_end );
					_data.mesh_vertex_buffer_bone_weight = global_engine_instance.interfaces.video_interface->create_vertex_buffer( &video_renderer_interface_c::vertex_format_mesh_base, _data.mesh_vertex_list_bone_weight.get_length(), _data.mesh_vertex_list_bone_weight.get_internal_array(), _data.mesh_vertex_list_bone_weight.get_internal_array_size_used(), false, false, false );
				}
			}

			if ( header->mesh_index_count > 0 )
			{
				if ( header->mesh_vertex_count <= 65535 )
				{
					data_end = data_offset + ( header->mesh_index_count * sizeof( uint16_c ) );
					if ( data_end > _raw_data_size ) { goto clean_up; }
					_data.mesh_index_list.construct_mode_static_from_array( reinterpret_cast< uint16_c * >( &_raw_data[ data_offset ] ), header->mesh_index_count );
					data_offset = align_data_offset( data_end );
					_data.mesh_index_buffer = global_engine_instance.interfaces.video_interface->create_index_buffer( video_index_format_e_uint16, header->mesh_index_count, _data.mesh_index_list.get_internal_array(), _data.mesh_index_list.get_internal_array_size_used(), false, false );
				}
				else
				{
					data_end = data_offset + ( header->mesh_index_count * sizeof( uint32_c ) );
					if ( data_end > _raw_data_size ) { goto clean_up; }
					_data.mesh_index_list.construct_mode_static_from_array( reinterpret_cast< uint16_c * >( &_raw_data[ data_offset ] ), header->mesh_index_count * 2 ); // * 2 to account for actual type of uint32_c.
					data_offset = align_data_offset( data_end );
					_data.mesh_index_buffer = global_engine_instance.interfaces.video_interface->create_index_buffer( video_index_format_e_uint32, header->mesh_index_count, _data.mesh_index_list.get_internal_array(), _data.mesh_index_list.get_internal_array_size_used(), false, false );
				}
			}

			data_end = data_offset + ( header->bone_count * sizeof( bone_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.bone_list.construct_mode_static_from_array( reinterpret_cast< bone_c * >( &_raw_data[ data_offset ] ), header->bone_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->bone_logic_count * sizeof( bone_logic_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.bone_logic_list.construct_mode_static_from_array( reinterpret_cast< bone_logic_c * >( &_raw_data[ data_offset ] ), header->bone_logic_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->bone_logic_property_count * sizeof( bone_logic_property_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.bone_logic_property_list.construct_mode_static_from_array( reinterpret_cast< bone_logic_property_c * >( &_raw_data[ data_offset ] ), header->bone_logic_property_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->bone_attachment_count * sizeof( bone_attachment_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.bone_attachment_list.construct_mode_static_from_array( reinterpret_cast< bone_attachment_c * >( &_raw_data[ data_offset ] ), header->bone_attachment_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->animation_count + sizeof( animation_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.animation_list.construct_mode_static_from_array( reinterpret_cast< animation_c * >( &_raw_data[ data_offset ] ), header->animation_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->animation_object_count * sizeof( animation_object_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.animation_object_list.construct_mode_static_from_array( reinterpret_cast< animation_object_c * >( &_raw_data[ data_offset ] ), header->animation_object_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->animation_property_count * sizeof( animation_property_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.animation_property_list.construct_mode_static_from_array( reinterpret_cast< animation_property_c * >( &_raw_data[ data_offset ] ), header->animation_property_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->animation_key_count * sizeof( animation_key_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.animation_key_list.construct_mode_static_from_array( reinterpret_cast< animation_key_c * >( &_raw_data[ data_offset ] ), header->animation_key_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->animation_event_count * sizeof( animation_event_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.animation_event_list.construct_mode_static_from_array( reinterpret_cast< animation_event_c * >( &_raw_data[ data_offset ] ), header->animation_event_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->animation_event_count * sizeof( physics_body_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.physics_body_list.construct_mode_static_from_array( reinterpret_cast< physics_body_c * >( &_raw_data[ data_offset ] ), header->physics_body_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->animation_event_count * sizeof( physics_shape_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.physics_shape_list.construct_mode_static_from_array( reinterpret_cast< physics_shape_c * >( &_raw_data[ data_offset ] ), header->physics_body_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->animation_event_count * sizeof( physics_vertex_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.physics_vertex_list.construct_mode_static_from_array( reinterpret_cast< physics_vertex_c * >( &_raw_data[ data_offset ] ), header->physics_body_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->animation_event_count * sizeof( uint16_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.physics_index_list.construct_mode_static_from_array( reinterpret_cast< uint16_c * >( &_raw_data[ data_offset ] ), header->physics_body_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->animation_event_count * sizeof( physics_constraint_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.physics_constraint_list.construct_mode_static_from_array( reinterpret_cast< physics_constraint_c * >( &_raw_data[ data_offset ] ), header->physics_body_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->physics_parameter_count * sizeof( float32_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.physics_parameters_list.construct_mode_static_from_array( reinterpret_cast< float32_c * >( &_raw_data[ data_offset ] ), header->physics_parameter_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->light_count * sizeof( light_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.light_list.construct_mode_static_from_array( reinterpret_cast< light_c * >( &_raw_data[ data_offset ] ), header->light_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->light_count * sizeof( property_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.property_list.construct_mode_static_from_array( reinterpret_cast< property_c * >( &_raw_data[ data_offset ] ), header->property_count );
			data_offset = align_data_offset( data_end );

			data_end = data_offset + ( header->light_count * sizeof( char8_c ) );
			if ( data_end > _raw_data_size ) { goto clean_up; }
			_data.string_table.construct_mode_static_from_array( reinterpret_cast< char8_c * >( &_raw_data[ data_offset ] ), header->string_table_length );
			data_offset = align_data_offset( data_end );
		}

		// calculate post-load extras for bones.
		_data.bone_extras_list.construct_mode_dynamic( _data.bone_list.get_length(), _data.bone_list.get_length() );
		for ( sint32_c i = 0; i < _data.bone_extras_list.get_length(); i++ )
		{
			bone_c const & bone = _data.bone_list[ i ];
			bone_extras_c & bone_extras = _data.bone_extras_list.get_internal_array()[ i ];

			// calculate bone length.
			bone_extras.length = ops::make_float32_length( vector32x3_c( bone.tail_position ) - vector32x3_c( bone.head_position ) );
			
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

		// done.
		_is_loaded = true;
		return true;

	clean_up:
		_data.reset();
		delete[] _raw_data;
		_raw_data = nullptr;
		return false;
	}

	void_c resource_file_model_c::_unload()
	{
		assert( _is_loaded == true );
		assert( _raw_data_size > 0 && _raw_data != nullptr );
		_data.reset();
		delete[] _raw_data;
		_raw_data = nullptr;
		_raw_data_size = 0;
		_is_loaded = false;
	}

	resource_file_model_c::resource_file_model_c()
		: resource_file_c()
		, _raw_data_size( 0 )
		, _raw_data( nullptr )
		, _data()
	{
	}

	resource_file_model_c::~resource_file_model_c()
	{
		assert( _is_loaded == false );
	}

	resource_file_model_c::data_c const & resource_file_model_c::get_data() const
	{
		return _data;
	}

	space_transform_c resource_file_model_c::make_bone_transform( vector64x3_c const & bone_head, vector64x3_c const & bone_tail, float32_c bone_roll, vector32x3_c const & global_up )
	{
		// this algorithm is essentially copied from a file in Blender.
		// in Blender, the bone's main axis is along the y axis of the matrix.
		// but in cheonsa, the bone's main axis is along the z axis of the matrix.
		// this is the difference and it can cause some confusion.
		space_transform_c result;
		vector32x3_c axis = ops::make_vector32x3_normalized( vector32x3_c( bone_tail - bone_head ) ); // this is the axis of the bone from head to tail.
		vector32x3_c tangent = ops::make_vector32x3_cross_product( global_up, axis ); // this is the axis that the bone_rotation will be around.
		if ( ops::make_float32_length_squared( tangent ) > constants< float32_c >::angle_near_zero() )
		{
			// bone axis is not pointing straight up or straight down.
			float32_c angle = ops::math_arc_cosine( ops::make_float32_dot_product( global_up, axis ) );
			result.rotation = ops::make_quaternion32_from_axis_angle( vector32x4_c( tangent.a, tangent.b, tangent.c, angle ) );
		}
		else
		{
			// bone axis is pointing straight up or straight down.
			float32_c angle = ops::make_float32_dot_product( global_up, axis ) > 0 ? 0.0f : constants< float32_c >::pi();
			result.rotation = ops::make_quaternion32_from_axis_angle( vector32x4_c( global_up.a, global_up.b, global_up.c, angle ) );
		}
		if ( bone_roll != 0.0f )
		{
			result.rotation = result.rotation * ops::make_quaternion32_from_axis_angle( vector32x4_c( axis.a, axis.b, axis.c, bone_roll ) );
		}
		result.position = bone_head;
		return result;
	}

}
