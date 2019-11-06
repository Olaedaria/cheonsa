#include "cheonsa_scene_types.h"
#include "cheonsa_scene.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	scene_material_c::scene_material_c()
	{
		reset();
	}

	void_c scene_material_c::reset()
	{
		cull_mode = video_cull_type_e_back;
		fill_mode = video_fill_type_e_face;
		blend_type = video_blend_type_e_set;
		//is_lit = true;
		is_mirror = false;
		is_overlay = false;
		is_masked = false;
		is_waved = false;
		colors[ 0 ] = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		colors[ 1 ] = vector32x4_c( 0.0f, 0.0f, 0.0f, 0.0f );
		colors[ 2 ] = vector32x4_c( 0.0f, 0.0f, 0.0f, 0.0f );
		colors[ 3 ] = vector32x4_c( 0.0f, 0.0f, 0.0f, 0.0f );
		textures[ 0 ] = nullptr;
		textures[ 1 ] = nullptr;
		textures[ 2 ] = nullptr;
		textures[ 3 ] = nullptr;
		pixel_shader = nullptr;
	}

	scene_material_c & scene_material_c::operator = ( scene_material_c const & other )
	{
		cull_mode = other.cull_mode;
		fill_mode = other.fill_mode;
		blend_type = other.blend_type;
		//is_lit = other.is_lit;
		is_mirror = other.is_mirror;
		is_overlay = other.is_overlay;
		is_masked = other.is_masked;
		is_waved = other.is_waved;
		colors[ 0 ] = other.colors[ 0 ];
		colors[ 1 ] = other.colors[ 1 ];
		colors[ 2 ] = other.colors[ 2 ];
		colors[ 3 ] = other.colors[ 3 ];
		textures[ 0 ] = other.textures[ 0 ];
		textures[ 1 ] = other.textures[ 1 ];
		textures[ 2 ] = other.textures[ 2 ];
		textures[ 3 ] = other.textures[ 3 ];
		pixel_shader = other.pixel_shader;
		return *this;
	}

	scene_camera_c::scene_camera_c()
		: _projection_type( scene_projection_type_e_perspective )
		, _perspective_center( 0.0f, 0.0f )
		, _perspective_clip_near( 1.0 )
		, _perspective_clip_far( 1000.0 )
		, _perspective_field_of_view( 75.0f * constants< float32_c >::degrees_to_radians() )
		, _orthographic_center( 0.0f, 0.0f )
		, _orthographic_clip_near( 1.0 )
		, _orthographic_clip_far( 1000.0 )
		, _orthographic_field_of_view( 100.0 )
	{
	}

	space_transform_c const & scene_camera_c::get_world_space_transform() const
	{
		return _world_space_transform;
	}

	void_c scene_camera_c::set_world_space_transform( space_transform_c const & value )
	{
		_world_space_transform = value;
	}

	scene_projection_type_e scene_camera_c::get_projection_type() const
	{
		return _projection_type;
	}

	void_c scene_camera_c::set_projection_type( scene_projection_type_e value )
	{
		_projection_type = value;
	}

	vector32x2_c const & scene_camera_c::get_perspective_center() const
	{
		return _perspective_center;
	}

	void_c scene_camera_c::set_perspective_center( vector32x2_c const & value )
	{
		_perspective_center = value;
	}

	float32_c scene_camera_c::get_perspective_clip_near() const
	{
		return _perspective_clip_near;
	}

	void_c scene_camera_c::set_perspective_clip_near( float32_c value )
	{
		_perspective_clip_near = value;
	}

	float32_c scene_camera_c::get_perspective_clip_far() const
	{
		return _perspective_clip_far;
	}

	void_c scene_camera_c::set_perspective_clip_far( float32_c value )
	{
		_perspective_clip_far = value;
	}

	float32_c scene_camera_c::get_perspective_field_of_view() const
	{
		return _perspective_field_of_view;
	}

	void_c scene_camera_c::set_perspective_field_of_view( float32_c value )
	{
		_perspective_field_of_view = value;
	}

	vector32x2_c const & scene_camera_c::get_orthographic_center() const
	{
		return _orthographic_center;
	}

	void_c scene_camera_c::set_orthographic_ccenter( vector32x2_c const value )
	{
		_orthographic_center = value;
	}

	float32_c scene_camera_c::get_orthographic_clip_near() const
	{
		return _orthographic_clip_near;
	}

	void_c scene_camera_c::set_orthographic_clip_near( float32_c value )
	{
		_orthographic_clip_near = value;
	}

	float32_c scene_camera_c::get_orthographic_clip_far() const
	{
		return _orthographic_clip_far;
	}

	void_c scene_camera_c::set_orthographic_clip_far( float32_c value )
	{
		_orthographic_clip_far = value;
	}

	float32_c scene_camera_c::get_orthographic_field_of_view() const
	{
		return _orthographic_field_of_view;
	}

	void_c scene_camera_c::set_orthographic_field_of_view( float32_c value )
	{
		_orthographic_field_of_view = value;
	}

	//matrix32x4x4_c scene_camera_c::make_view_transform_at_origin() const
	//{
	//	matrix32x4x4_c result;
	//	result = ops::make_matrix32x4x4_view_from_look_at( vector32x3_c( 0.0f, 0.0f, 0.0f ), _world_space_transform.get_unscaled_basis_b(), _world_space_transform.get_unscaled_basis_c() );
	//	return result;
	//}

	matrix32x4x4_c scene_camera_c::make_view_transform_relative_to_origin( vector64x3_c origin ) const
	{
		matrix32x4x4_c result;
		result = ops::make_matrix32x4x4_view_from_look_at( vector32x3_c( _world_space_transform.position - origin ), _world_space_transform.get_unscaled_basis_b(), _world_space_transform.get_unscaled_basis_c() );
		return result;
	}

	matrix32x4x4_c scene_camera_c::make_projection_transform( sint32_c canvas_width, sint32_c canvas_height ) const
	{
		matrix32x4x4_c result;
		if ( _projection_type == scene_projection_type_e_perspective )
		{
			float32_c scale_factor = ops::make_float32_fov_b_off_center_scale_factor( _perspective_field_of_view, _perspective_clip_near );
			float32_c half_width = static_cast< float32_c >( canvas_width ) / 2.0f;
			float32_c half_height = static_cast< float32_c >( canvas_height ) / 2.0f;
			float32_c offset_x = _perspective_center.a * half_width;
			float32_c offset_y = _perspective_center.b * half_height;
			//result = ops::make_matrix32x4x4_projection_perspective_off_center( ( offset_x - half_width ) * scale_factor, ( offset_x + half_width ) * scale_factor, ( offset_y - half_height ) * scale_factor, ( offset_y + half_height ) * scale_factor, _perspective_clip_near, _perspective_clip_far );
			float32_c fov = 75.0f * constants< float32_c >::degrees_to_radians();
			result = ops::make_matrix32x4x4_projection_perspective( fov, static_cast< float32_c >( canvas_width ) / static_cast< float32_c >( canvas_height ), 0.01f, 1000.0f );
		}
		else if ( _projection_type == scene_projection_type_e_orthographic )
		{
			float32_c offset_x = _orthographic_center.a * static_cast< float32_c >( canvas_width ) * 0.5f;
			float32_c offset_y = _orthographic_center.b * static_cast< float32_c >( canvas_height ) * 0.5f;
			float32_c half_width = static_cast< float32_c >( canvas_width ) / 2.0f;
			float32_c half_height = static_cast< float32_c >( canvas_height ) / 2.0f;
			result = ops::make_matrix32x4x4_projection_orthographic_off_center( offset_x - half_width, offset_x + half_width, offset_y - half_height, offset_y + half_height, _orthographic_clip_near, _orthographic_clip_far );
		}
		return result;
	}

	ray64_c scene_camera_c::build_ray_through_canvas( vector32x2_c const & canvas_coordinates, sint32_c canvas_width, sint32_c canvas_height ) const
	{
		segment64_c segment = build_segment_through_canvas( canvas_coordinates, canvas_width, canvas_height );
		return ray64_c( segment.point_a, ops::make_vector64x3_normalized( segment.point_b - segment.point_a ) );
	}

	segment64_c scene_camera_c::build_segment_through_canvas( vector32x2_c const & canvas_coordinates, sint32_c canvas_width, sint32_c canvas_height ) const
	{
		segment64_c result;
		float64_c x = ( canvas_coordinates.a / static_cast< float64_c >( canvas_width ) ) * 2.0 - 1.0; // transform from range (0..1) to (-1..1)
		float64_c y = ( canvas_coordinates.b / static_cast< float64_c >( canvas_height ) ) * 2.0 - 1.0;
		matrix32x3x3_c world_space_transform_basis = _world_space_transform.get_unscaled_basis();
		if ( _projection_type == scene_projection_type_e_perspective )
		{
			vector64x2_c far_size_half;
			far_size_half.b = ops::math_tangent( _perspective_field_of_view * 0.5 ) * _perspective_clip_far ;
			far_size_half.a = far_size_half.b * (  static_cast< float32_c >( canvas_width ) / static_cast< float32_c >( canvas_height ) );
			vector64x3_c far_center = _world_space_transform.position + vector64x3_c( world_space_transform_basis.c * _perspective_clip_far );
			vector64x3_c far_center_plus_offset = far_center + ( vector64x3_c( world_space_transform_basis.a ) * ( x * far_size_half.a ) ) + ( vector64x3_c( world_space_transform_basis.b ) * ( y * far_size_half.b ) );
			result.point_a = _world_space_transform.position;
			result.point_b = far_center_plus_offset;
		}
		else if ( _projection_type == scene_projection_type_e_orthographic )
		{
			result.point_a = ( _world_space_transform.position + vector64x3_c( world_space_transform_basis.c ) * _orthographic_clip_near ) + ( world_space_transform_basis.a * x ) + ( world_space_transform_basis.b * y );
			result.point_b = result.point_a + vector64x3_c( world_space_transform_basis.c ) * ( _orthographic_clip_far - _orthographic_clip_near );
		}
		return result;
	}

	vector32x2_c scene_camera_c::transform_point_from_world_space_to_canvas_space( vector64x3_c const & world_space_point, sint32_c canvas_width, sint32_c canvas_height ) const
	{
		vector32x2_c result;
		vector32x3_c local_point = vector32x3_c( world_space_point - _world_space_transform.position );
		matrix32x4x4_c view_transform = make_view_transform_relative_to_origin( vector64x3_c( 0.0, 0.0, 0.0 ) );
		matrix32x4x4_c projection_transform = make_projection_transform( canvas_width, canvas_height );
		vector32x4_c transformed_point = ops::make_vector32x4_transformed_point( local_point, view_transform * projection_transform );
		result.a = static_cast< float32_c >( transformed_point.a / transformed_point.d ) * 0.5f + 0.5f; // scale from range (-1..1) to (0..1)
		result.b = static_cast< float32_c >( transformed_point.b / transformed_point.d ) * 0.5f + 0.5f; // scale from range (-1..1) to (0..1)
		return result;
	}

	scene_light_c::scene_light_c()
		: _scene( nullptr )
		, _world_space_transform()
		, _render_enable( true )
		, _shadow_cast_enable( true )
		, _type( scene_light_type_e_point )
		, _color( 1.0f, 1.0f, 1.0f )
		, _brightness( 1.0f )
		, _range( 10.0f )
		, _local_space_obb()
		, _world_space_aabb()
		, _shadow_view_start( 0 )
		, _shadow_view_count( 0 )
		, _render_frame_last( 0 )
		, _scene_tree_leaf_node( nullptr )
		, _scene_tree_list_node( this )
	{
	}

	scene_c * scene_light_c::get_scene() const
	{
		return _scene;
	}

	space_transform_c const & scene_light_c::get_world_space_transform() const
	{
		return _world_space_transform;
	}

	void_c scene_light_c::set_world_space_transform( space_transform_c const & value )
	{
		_world_space_transform = value;
		update_bounding_boxes();
	}

	boolean_c scene_light_c::get_render_enable() const
	{
		return _render_enable;
	}

	void_c scene_light_c::set_render_enable( boolean_c value )
	{
		if ( _render_enable != value )
		{
			_render_enable = value;
			if ( _scene != nullptr )
			{
				_scene->invalidate_light_probes_with_light( this );
			}
		}
	}

	boolean_c scene_light_c::get_shadow_cast_enable() const
	{
		return _shadow_cast_enable;
	}

	void_c scene_light_c::set_shadow_cast_enable( boolean_c value )
	{
		if ( _shadow_cast_enable != value )
		{
			_shadow_cast_enable = value;
			if ( _scene != nullptr )
			{
				_scene->invalidate_light_probes_with_light( this );
			}
		}
	}

	scene_light_type_e scene_light_c::get_type() const
	{
		return _type;
	}

	void_c scene_light_c::set_type( scene_light_type_e value )
	{
		if ( _type != value )
		{
			boolean_c was_global = _type == scene_light_type_e_direction;
			boolean_c now_global = value == scene_light_type_e_direction;
			if ( _scene != nullptr )
			{
				_scene->invalidate_light_probes_with_light( this );
				if ( was_global == true && now_global == false )
				{
					_scene->_global_lights_list.remove( this );
					_scene->_local_lights_tree.insert_or_update_item( this );
				}
				else if ( was_global == false && now_global == true )
				{
					_scene->_local_lights_tree.remove_item( this );
					_scene->_global_lights_list.insert_at_end( this );
				}
			}
			_type = value;
			update_bounding_boxes();
		}
	}

	vector32x3_c const & scene_light_c::get_color() const
	{
		return _color;
	}

	void_c scene_light_c::set_color( vector32x3_c const & value )
	{
		if ( _color != value )
		{
			_color = value;
			if ( _scene != nullptr )
			{
				_scene->invalidate_light_probes_with_light( this );
			}
		}
	}

	float32_c scene_light_c::get_brightness() const
	{
		return _brightness;
	}

	void_c scene_light_c::set_brightness( float32_c value )
	{
		if ( _brightness != value )
		{
			_brightness = value;
			if ( _scene != nullptr )
			{
				_scene->invalidate_light_probes_with_light( this );
			}
			_range = ops::math_square_root( _brightness );
			update_bounding_boxes();
		}
	}

	float32_c scene_light_c::get_range() const
	{
		return _range;
	}

	//void_c scene_light_c::set_range( float32_c value )
	//{
	//	if ( _range != value )
	//	{
	//		if ( _scene )
	//		{
	//			_scene->invalidate_light_probes_with_light( this );
	//		}
	//		_range = value;
	//		update_bounding_boxes_and_scene_tree();
	//	}
	//}

	float32_c scene_light_c::get_cone_angle() const
	{
		return _cone_angle;
	}

	void_c scene_light_c::set_cone_angle( float32_c value )
	{
		if ( _cone_angle != value )
		{
			if ( _type == scene_light_type_e_cone && _scene != nullptr )
			{
				_scene->invalidate_light_probes_with_light( this );
			}
			_cone_angle = value;
			if ( _type == scene_light_type_e_cone )
			{
				update_bounding_boxes();
			}
		}
	}

	box32x3_c const & scene_light_c::get_local_space_obb() const
	{
		return _local_space_obb;
	}

	box64x3_c const & scene_light_c::get_world_space_aabb() const
	{
		return _world_space_aabb;
	}

	void_c scene_light_c::update_bounding_boxes()
	{
		if ( _type == scene_light_type_e_point )
		{
			_local_space_obb.minimum.a = -_range;
			_local_space_obb.minimum.b = -_range;
			_local_space_obb.minimum.c = -_range;
			_local_space_obb.maximum.a = _range;
			_local_space_obb.maximum.b = _range;
			_local_space_obb.maximum.c = _range;
		}
		else if ( _type == scene_light_type_e_cone )
		{
			// a cone would make more sense...
			float32_c span = ops::math_tangent( _cone_angle * 0.5f ) * 2.0f * _range;
			_local_space_obb.minimum.a = -span;
			_local_space_obb.minimum.b = 0.0f;
			_local_space_obb.minimum.c = -span;
			_local_space_obb.maximum.a = span;
			_local_space_obb.maximum.b = _range;
			_local_space_obb.maximum.c = span;
		}
		else if ( _type == scene_light_type_e_direction )
		{
			_local_space_obb.minimum.a = 1.0;
			_local_space_obb.minimum.b = 1.0;
			_local_space_obb.minimum.c = 1.0;
			_local_space_obb.maximum.a = -1.0;
			_local_space_obb.maximum.b = -1.0;
			_local_space_obb.maximum.c = -1.0;
		}

		_world_space_aabb = ops::make_aabb_from_obb( box64x3_c( _local_space_obb ), _world_space_transform );

		if ( _scene != nullptr )
		{
			if ( _type != scene_light_type_e_direction )
			{
				_scene->_local_lights_tree.insert_or_update_item( this );
			}
			_scene->invalidate_light_probes_with_light( this );
		}
	}

	scene_node_c::scene_node_c()
		: _name()
		, _mother( nullptr )
		, _daughters()
		, _daughters_node( this )
		, _local_space_transform()
		, _world_space_transform()
		, _auto_update_suspend_count( 0 )
	{
	}

	scene_node_c::~scene_node_c()
	{
		core_linked_list_c< scene_node_c * >::node_c const * daughter_list_node = _daughters.get_first();
		while ( daughter_list_node )
		{
			core_linked_list_c< scene_node_c * >::node_c const * daughter_list_node_next = daughter_list_node->get_next();
			delete daughter_list_node->get_value();
			daughter_list_node = daughter_list_node_next;
		}
	}

	string8_c const & scene_node_c::get_name() const
	{
		return _name;
	}

	void_c scene_node_c::set_name( string8_c const & value )
	{
		_name = value;
	}

	scene_node_c const * scene_node_c::get_root_scene_node() const
	{
		scene_node_c const * result = this;
		while ( result->_mother )
		{
			result = result->_mother;
		}
		return result;
	}

	scene_node_c * scene_node_c::get_root_scene_node()
	{
		scene_node_c * result = this;
		while ( result->_mother )
		{
			result = result->_mother;
		}
		return result;
	}

	scene_node_c const * scene_node_c::get_mother() const
	{
		return _mother;
	}

	scene_node_c * scene_node_c::get_mother()
	{
		return _mother;
	}

	void_c scene_node_c::set_mother( scene_node_c * value, boolean_c and_preserve_world_space_transform )
	{
		if ( _mother )
		{
			_mother->remove_daughter( this, and_preserve_world_space_transform );
		}
		if ( value )
		{
			_mother->add_daughter( value, and_preserve_world_space_transform );
		}
	}

	core_linked_list_c< scene_node_c * > const & scene_node_c::get_daughters() const
	{
		return _daughters;
	}

	void_c scene_node_c::add_daughter( scene_node_c * value, boolean_c and_preserve_world_space_transform )
	{
		assert( value );
		assert( value->_mother == nullptr );
		value->_mother = this;
		_daughters.insert_at_end( &value->_daughters_node );
		if ( and_preserve_world_space_transform )
		{
			value->_local_space_transform = value->_world_space_transform * get_world_space_transform().get_inverted();
		}
		if ( _auto_update_suspend_count == 0 )
		{
			value->update_transform_hierarchy();
		}
	}

	void_c scene_node_c::remove_daughter( scene_node_c * value, boolean_c and_preserve_world_space_transform )
	{
		assert( value );
		assert( value->_mother == this );
		value->_mother = nullptr;
		_daughters.remove( &value->_daughters_node );
		if ( and_preserve_world_space_transform )
		{
			value->_local_space_transform = value->_world_space_transform;
			if ( value->_auto_update_suspend_count == 0 )
			{
				value->update_transform_hierarchy();
			}
		}
	}

	scene_node_c const * scene_node_c::find_daughter_with_name( string8_c const & name ) const
	{
		if ( _name == name )
		{
			return this;
		}
		core_linked_list_c< scene_node_c * >::node_c * daughter_list_node = _daughters.get_first();
		while ( daughter_list_node )
		{
			scene_node_c * result = daughter_list_node->get_value()->find_daughter_with_name( name );
			if ( result )
			{
				return result;
			}
			daughter_list_node = daughter_list_node->get_next();
		}
		return nullptr;
	}

	scene_node_c * scene_node_c::find_daughter_with_name( string8_c const & name )
	{
		return const_cast< scene_node_c * >( const_cast< scene_node_c const * >( this )->find_daughter_with_name( name ) );
	}

	scene_node_c const * scene_node_c::find_daughter_with_path( string8_c const & path ) const
	{
		if ( _name.get_length() == 0 || path.get_length() == 0 )
		{
			return nullptr;
		}

		// extract first path node and see if it matches our name.
		sint32_c path_end = path.get_length();
		sint32_c delimiter_index;
		if ( ops::string8_find_index_of( path, string8_c( mode_e_static, "/" ), delimiter_index ) )
		{
			path_end = delimiter_index;
			if ( path_end == 0 )
			{
				return nullptr;
			}
		}
		if ( path_end != _name.get_length() )
		{
			return nullptr;
		}
		char8_c const * name_characters = _name.character_list.get_internal_array();
		char8_c const * path_characters = path.character_list.get_internal_array();
		for ( sint32_c i = 0; i < path_end; i++ )
		{
			if ( name_characters[ i ] != path_characters[ i ] )
			{
				return nullptr;
			}
		}
		
		// path node matches.
		if ( path_end == path.get_length() )
		{
			// reached end of path, return this.
			return this;
		}
		else
		{
			// there is more path to process, process daughters.
			string8_c sub_path; // get the rest of the path after the first slash.
			sub_path.character_list.construct_mode_static_from_array( &path_characters[ path_end + 1 ], path.get_length() - ( path_end + 1 ) );
			core_linked_list_c< scene_node_c * >::node_c * daughter_list_node = _daughters.get_first();
			while ( daughter_list_node )
			{
				scene_node_c * result = daughter_list_node->get_value()->find_daughter_with_path( sub_path );
				if ( result )
				{
					return result;
				}
				daughter_list_node = daughter_list_node->get_next();
			}
		}

		return nullptr;
	}

	scene_node_c * scene_node_c::find_daughter_with_path( string8_c const & path )
	{
		return const_cast< scene_node_c * >( const_cast< scene_node_c const * >( this )->find_daughter_with_path( path ) );
	}

	space_transform_c const & scene_node_c::get_local_space_transform() const
	{
		return _local_space_transform;
	}

	vector64x3_c const & scene_node_c::get_local_space_position() const
	{
		return _local_space_transform.position;
	}

	void_c scene_node_c::set_local_space_position( vector64x3_c const & value )
	{
		_local_space_transform.position = value;
		if ( _auto_update_suspend_count == 0 )
		{
			update_transform_hierarchy();
		}
	}

	quaternion32_c const & scene_node_c::get_local_space_rotation() const
	{
		return _local_space_transform.rotation;
	}

	void_c scene_node_c::set_local_space_rotation( quaternion32_c const & value )
	{
		_local_space_transform.rotation = value;
		if ( _auto_update_suspend_count == 0 )
		{
			update_transform_hierarchy();
		}
	}

	vector32x3_c const & scene_node_c::get_local_space_scale() const
	{
		return _local_space_transform.scale;
	}

	void_c scene_node_c::set_local_space_scale( vector32x3_c const & value )
	{
		_local_space_transform.scale = value;
		if ( _auto_update_suspend_count == 0 )
		{
			update_transform_hierarchy();
		}
	}

	void_c scene_node_c::set_local_space_scale( float32_c value )
	{
		_local_space_transform.scale.a = value;
		_local_space_transform.scale.b = value;
		_local_space_transform.scale.c = value;
		if ( _auto_update_suspend_count == 0 )
		{
			update_transform_hierarchy();
		}
	}

	space_transform_c const & scene_node_c::get_world_space_transform() const
	{
		return _world_space_transform;
	}

	void_c scene_node_c::set_world_space_transform( space_transform_c const & value )
	{
		if ( _mother )
		{
			space_transform_c mother_global_transform_inverted = _mother->_world_space_transform.get_inverted();
			_local_space_transform.position = ops::make_vector64x3_transformed_point( value.position, mother_global_transform_inverted );
			_local_space_transform.rotation = value.rotation * mother_global_transform_inverted.rotation;
			_local_space_transform.scale = value.scale * mother_global_transform_inverted.scale; //value.scale / _mother->_world_space_transform.scale;
		}
		else
		{
			_local_space_transform = value;
		}
		if ( _auto_update_suspend_count == 0 )
		{
			update_transform_hierarchy();
		}
	}

	vector64x3_c const & scene_node_c::get_world_space_position() const
	{
		return _world_space_transform.position;
	}

	void_c scene_node_c::set_world_space_position( vector64x3_c const & value )
	{
		if ( _mother )
		{
			space_transform_c mother_global_transform_inverted = _mother->_world_space_transform.get_inverted();
			_local_space_transform.position = ops::make_vector64x3_transformed_point( value, mother_global_transform_inverted );
		}
		else
		{
			_local_space_transform.position = value;
		}
		_world_space_transform.position = value;
		if ( _auto_update_suspend_count == 0 )
		{
			update_transform_hierarchy();
		}
	}


	quaternion32_c const & scene_node_c::get_world_space_rotation() const
	{
		return _world_space_transform.rotation;
	}

	void_c scene_node_c::set_world_space_rotation( quaternion32_c const & value )
	{
		if ( _mother )
		{
			quaternion32_c mother_global_transform_inverted = ops::make_quaternion32_inverted( _mother->_world_space_transform.rotation );
			_local_space_transform.rotation = value * mother_global_transform_inverted;
		}
		else
		{
			_local_space_transform.rotation = value;
		}
		_world_space_transform.rotation = value;
		if ( _auto_update_suspend_count == 0 )
		{
			update_transform_hierarchy();
		}
	}

	vector32x3_c const & scene_node_c::get_world_space_scale() const
	{
		return _world_space_transform.scale;
	}

	void_c scene_node_c::set_world_space_scale( vector32x3_c const & value )
	{
		if ( _mother )
		{
			_local_space_transform.scale = value / _mother->_world_space_transform.scale;
		}
		else
		{
			_local_space_transform.scale = value;
		}
		_world_space_transform.scale = value;
		if ( _auto_update_suspend_count == 0 )
		{
			update_transform_hierarchy();
		}
	}

	void_c scene_node_c::set_world_space_scale( float32_c value )
	{
		if ( _mother )
		{
			_local_space_transform.scale.a = value / _mother->_world_space_transform.scale.a;
			_local_space_transform.scale.b = value / _mother->_world_space_transform.scale.b;
			_local_space_transform.scale.c = value / _mother->_world_space_transform.scale.c;
		}
		else
		{
			_local_space_transform.scale.a = value;
			_local_space_transform.scale.b = value;
			_local_space_transform.scale.c = value;
		}
		_world_space_transform.scale.a = value;
		_world_space_transform.scale.b = value;
		_world_space_transform.scale.c = value;
		if ( _auto_update_suspend_count == 0 )
		{
			update_transform_hierarchy();
		}
	}

	box32x3_c const & scene_node_c::get_local_space_bounding_box() const
	{
		return _local_space_bounding_box;
	}

	void_c scene_node_c::set_local_space_bounding_box( box32x3_c const & value )
	{
		_local_space_bounding_box = value;
		_world_space_bounding_box = ops::make_aabb_from_obb( box64x3_c( _local_space_bounding_box ), _world_space_transform );
	}

	box64x3_c const & scene_node_c::get_world_space_bounding_box() const
	{
		return _world_space_bounding_box;
	}

	void_c scene_node_c::suspend_auto_update()
	{
		_auto_update_suspend_count++;
	}

	void_c scene_node_c::resume_auto_update()
	{
		assert( _auto_update_suspend_count > 0 );
		_auto_update_suspend_count--;
	}

	void_c scene_node_c::update_transform()
	{
		if ( _mother )
		{
			_world_space_transform.position = ops::make_vector64x3_transformed_point( _local_space_transform.position, _mother->_world_space_transform );
			_world_space_transform.rotation = _local_space_transform.rotation * _mother->_world_space_transform.rotation;
			_world_space_transform.scale = _local_space_transform.scale * _mother->_world_space_transform.scale;
		}
		else
		{
			_world_space_transform.position = _local_space_transform.position;
			_world_space_transform.rotation = _local_space_transform.rotation;
			_world_space_transform.scale = _local_space_transform.scale;
		}
	}

	void_c scene_node_c::update_transform_hierarchy()
	{
		update_transform();
		core_linked_list_c< scene_node_c * >::node_c * daughter_list_node = _daughters.get_first();
		while ( daughter_list_node )
		{
			daughter_list_node->get_value()->update_transform_hierarchy();
			daughter_list_node = daughter_list_node->get_next();
		}
	}

}
