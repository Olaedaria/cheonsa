#include "cheonsa_scene_component_model.h"
#include "cheonsa_scene_component_light_probe.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene.h"
#include "cheonsa_ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	scene_component_model_c::animation_map_c::animation_map_c()
		: source_model( nullptr )
		, source_animation( nullptr )
	{
	}

	scene_component_model_c::animation_player_c::object_player_c::property_player_c::property_player_c()
		: property_type( 0 )
		//, property_index( 0 )
		, key_current( 0 )
		, key_minimum( 0 )
		, key_maximum( 0 )
	{
	}

	scene_component_model_c::animation_player_c::object_player_c::object_player_c()
		: object_name()
		, object_index_for_bone( -1 )
		, property_player_list()
		, property_player_for_position( nullptr )
		, property_player_for_rotation( nullptr )
		, property_player_for_scale( nullptr )
	{
	}

	void_c scene_component_model_c::animation_player_c::_handle_on_animation_resources_updated( scene_component_model_c * model )
	{
		// save current time before we unbind.
		if ( _time_to_bind_type == time_to_bind_type_e_seconds )
		{
			_time_to_bind = _time;
		}
		else
		{
			_time_to_bind = 0.5f;
			float32_c span = ( _source_animation->time_out - _source_animation->time_in );
			if ( span > 0.0f )
			{
				_time_to_bind = ( _time - _source_animation->time_in ) / span;
			}
		}

		// unbind.
		_source_model = nullptr;
		_source_animation = nullptr;
		_object_player_list.remove_all();

		// acquire new animation.
		animation_map_c * animation_map = _model->_animation_map_dictionary.find_value_pointer( _animation_name );
		if ( animation_map != nullptr )
		{
			_source_model = animation_map->source_model;
			_source_animation = animation_map->source_animation;

			// instantiate and bind object players.
			uint32_c animation_object_count = _source_animation->object_end - _source_animation->object_start;
			_object_player_list.construct_mode_dynamic( animation_object_count );
			for ( uint32_c i = 0; i < animation_object_count; i++ )
			{
				resource_object_model_c::animation_object_c const * source_animation_object = &_source_model->get_data().animation_object_list[ _source_animation->object_start + i ];

				object_player_c * animation_object_player = &_object_player_list[ i ];

				// bind animation to bone.
				animation_object_player->object_name = string8_c( mode_e_static, &_source_model->get_data().string_table[ source_animation_object->name ] );
				animation_object_player->object_index_for_bone = _model->find_bone_index_with_name( animation_object_player->object_name );
				if ( animation_object_player->object_index_for_bone >= 0 )
				{
					// object being bound to is a bone.
					animation_object_player->property_player_list.construct_mode_dynamic( 3, 3 );
					animation_object_player->property_player_list[ 0 ].property_type = static_cast< uint32_c >( resource_object_model_c::animation_property_c::type_e::position );
					//animation_object_player->property_player_list[ 0 ].property_index = 0;
					animation_object_player->property_player_list[ 1 ].property_type = static_cast< uint32_c >( resource_object_model_c::animation_property_c::type_e::rotation );
					//animation_object_player->property_player_list[ 1 ].property_index = 1;
					animation_object_player->property_player_list[ 2 ].property_type = static_cast< uint32_c >( resource_object_model_c::animation_property_c::type_e::scale );
					//animation_object_player->property_player_list[ 2 ].property_index = 2;
					uint32_c animation_property_count = source_animation_object->property_end - source_animation_object->property_start;
					for ( uint32_c j = 0; j < animation_property_count; j++ )
					{
						resource_object_model_c::animation_property_c const * source_animation_property = &_source_model->get_data().animation_property_list[ source_animation_object->property_start + j ];
						for ( sint32_c k = 0; k < animation_object_player->property_player_list.get_length(); k++ )
						{
							if ( source_animation_property->type == animation_object_player->property_player_list[ k ].property_type )
							{
								object_player_c::property_player_c * animation_property_player = &animation_object_player->property_player_list[ k ];
								animation_property_player->key_current = source_animation_property->key_start;
								animation_property_player->key_minimum = source_animation_property->key_start;
								animation_property_player->key_maximum = source_animation_property->key_end;
								break;
							}
						}
					}
					animation_object_player->property_player_for_position = animation_object_player->property_player_list[ 0 ].key_minimum >= 0 ? & animation_object_player->property_player_list[ 0 ] : 0;
					animation_object_player->property_player_for_rotation = animation_object_player->property_player_list[ 1 ].key_minimum >= 0 ? & animation_object_player->property_player_list[ 1 ] : 0;
					animation_object_player->property_player_for_scale = animation_object_player->property_player_list[ 2 ].key_minimum >= 0 ? & animation_object_player->property_player_list[ 2 ] : 0;
				}
				// bind animation to particle emmitter
				// ...
				// bind animation to material
				// ...
				// bind animation to light
				// ...
			}

			// bind time.
			if ( _time_to_bind_type == time_to_bind_type_e_seconds )
			{
				set_time( _time_to_bind );
			}
			else
			{
				set_time_percent( _time_to_bind );
			}
		}
	}

	scene_component_model_c::animation_player_c::animation_player_c( scene_component_model_c * model )
		: _model( model )
		, _animation_name()
		, _source_model( nullptr )
		, _source_animation( nullptr )
		, _object_player_list()
		, _blend_type( blend_type_e_mix )
		, _blend_weight( 0.0f )
		, _loop_count_current( 0 )
		, _loop_count_target( 0 )
		, _speed( 1.0f )
		, _time( 0.0f )
		, _time_to_bind_type( time_to_bind_type_e_seconds )
		, _time_to_bind( 0.0f )
		, _time_from_other( nullptr )
	{
		assert( _model != nullptr );
		_model->_on_animation_resources_updated.subscribe( this, &animation_player_c::_handle_on_animation_resources_updated );
	}

	scene_component_model_c::animation_player_c::~animation_player_c()
	{
		_model->_on_animation_resources_updated.unsubscribe( this, &animation_player_c::_handle_on_animation_resources_updated );
	}

	void_c scene_component_model_c::animation_player_c::update( float32_c time_step )
	{
		assert( _source_animation != nullptr );

		if ( _blend_weight == 0.0f )
		{
			return;
		}

		if ( _time_from_other == nullptr )
		{
			if ( _loop_count_target > 0 && _loop_count_current >= _loop_count_target )
			{
				return;
			}
			//if ( _speed_type == speed_type_e_normal )
			//{
			//	time_step *= _speed;
			//}
			//else
			//{
			//	time_step *= ( _speed / _animation->locomotion_speed );
			//}
			time_step *= _speed;
			if ( time_step != 0.0f )
			{
				_time = static_cast< float32_c >( _time + time_step );
				if ( _time >= _source_animation->time_out || _time < _source_animation->time_in )
				{
					// time went past bounds.
					_loop_count_current++;
					if ( _loop_count_target > 0 && _loop_count_current >= _loop_count_target )
					{
						if ( _time >= _source_animation->time_out )
						{
							// clamp time to end.
							set_time( _source_animation->time_out );
						}
						else
						{
							// clamp time to start.
							set_time( _source_animation->time_in );
						}
					}
					else
					{
						// wrap time around.
						set_time( ops::math_wrap( _time - _source_animation->time_in, _source_animation->time_out - _source_animation->time_in ) + _source_animation->time_in );
					}
				}
				else
				{
					if ( time_step > 0.0f )
					{
						// moving forward, update key indices where needed
						for ( sint32_c object_player_index = 0; object_player_index < _object_player_list.get_length(); object_player_index++ )
						{
							animation_player_c::object_player_c & object_player = _object_player_list[ object_player_index ];
							for ( sint32_c property_player_index = 0; property_player_index < object_player.property_player_list.get_length(); property_player_index++ )
							{
								animation_player_c::object_player_c::property_player_c & property_player = object_player.property_player_list[ property_player_index ];
								if ( property_player.key_current >= 0 )
								{
									for ( sint32_c key_index = property_player.key_current; key_index < property_player.key_maximum; key_index++ )
									{
										if ( _source_model->get_data().animation_key_list[ key_index ].time > _time )
										{
											break;
										}
										property_player.key_current = key_index;
									}
								}
							}
						}
					}
					else
					{
						// moving backward
						sint32_c object_count = _object_player_list.get_length();
						for ( sint32_c i = 0; i < object_count; i++ )
						{
							object_player_c & object_player = _object_player_list[ i ];
							for ( sint32_c j = 0; j < object_player.property_player_list.get_length(); j++ )
							{
								object_player_c::property_player_c & property_player = object_player.property_player_list[ j ];
								if ( property_player.key_current >= 0 )
								{
									for ( sint32_c k = property_player.key_current; k >= property_player.key_minimum; k-- )
									{
										if ( _source_model->get_data().animation_key_list[ k ].time < _time )
										{
											property_player.key_current = k;
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
			set_time( _time_from_other->get_time() );
		}
	}

	void_c scene_component_model_c::animation_player_c::play( sint32_c loop_count )
	{
		_loop_count_current = 0;
		_loop_count_target = loop_count;
		if ( _source_animation != nullptr )
		{
			set_time( _source_animation->time_in );
		}
	}

	void_c scene_component_model_c::animation_player_c::set_animation( string8_c const & value )
	{
		_animation_name = value;
		_handle_on_animation_resources_updated( nullptr );
	}

	void_c scene_component_model_c::animation_player_c::clear_animation()
	{
		_animation_name = "";
		_source_animation = nullptr;
	}

	scene_component_model_c::animation_player_c::blend_type_e scene_component_model_c::animation_player_c::get_blend_type()
	{
		return _blend_type;
	}

	void_c scene_component_model_c::animation_player_c::set_blend_type( blend_type_e value )
	{
		_blend_type = value;
	}

	float32_c scene_component_model_c::animation_player_c::get_blend_weight()
	{
		return _blend_weight;
	}

	void_c scene_component_model_c::animation_player_c::set_blend_weight( float32_c value )
	{
		_blend_weight = value;
	}

	void_c scene_component_model_c::animation_player_c::traverse_blend_weight( float32_c value, float32_c amount )
	{
		_blend_weight = ops::traverse( _blend_weight, value, amount );
	}

	float32_c scene_component_model_c::animation_player_c::get_animation_move_speed()
	{
		if ( _source_animation )
		{
			return _source_animation->move_speed;
		}
		return 1.0f;
	}

	float32_c scene_component_model_c::animation_player_c::get_speed()
	{
		return _speed;
	}

	void_c scene_component_model_c::animation_player_c::set_speed( float32_c value )
	{
		_speed = value;
	}

	float32_c scene_component_model_c::animation_player_c::get_time()
	{
		return _time;
	}

	void_c scene_component_model_c::animation_player_c::set_time( float32_c value )
	{
		_time_from_other = nullptr;
		_time_to_bind_type = time_to_bind_type_e_seconds;
		if ( _source_animation != nullptr )
		{
			_time = value;
			for ( sint32_c object_player_index = 0; object_player_index < _object_player_list.get_length(); object_player_index++ )
			{
				object_player_c & object_player = _object_player_list[ object_player_index ];
				for ( sint32_c property_player_index = 0; property_player_index < object_player.property_player_list.get_length(); property_player_index++ )
				{
					object_player_c::property_player_c & property_player = object_player.property_player_list[ property_player_index ];
					for ( sint32_c key_index = property_player.key_minimum; key_index < property_player.key_maximum; key_index++ )
					{
						if ( _source_model->get_data().animation_key_list[ key_index ].time > _time )
						{
							break;
						}
						property_player.key_current = key_index;
					}
				}
			}
		}
		else
		{
			_time_to_bind = value;
		}
	}

	float32_c scene_component_model_c::animation_player_c::get_time_percent()
	{
		if ( _source_animation )
		{
			return ( _time - _source_animation->time_in ) / ( _source_animation->time_out - _source_animation->time_in );
		}
		return 0.0f;
	}

	void_c scene_component_model_c::animation_player_c::set_time_percent( float32_c value )
	{
		_time_from_other = nullptr;
		_time_to_bind_type = time_to_bind_type_e_percent;
		if ( _source_animation != nullptr )
		{
			set_time( value * ( _source_animation->time_out - _source_animation->time_in ) + _source_animation->time_in );
		}
		else
		{
			_time_to_bind = value;
		}
	}

	void_c scene_component_model_c::animation_player_c::set_to_use_time_from_other( animation_player_c * other )
	{
		_time_from_other = other;
	}

	void_c scene_component_model_c::animation_player_c::sample_property_player( animation_player_c::object_player_c::property_player_c * property_player, vector32x4_c & result )
	{
		sint32_c key_current = property_player->key_current;
		sint32_c key_minimum = property_player->key_minimum;
		sint32_c key_maximum = property_player->key_maximum - 1;

		//sint32_c key_a_index = ops::clamp( key_current - 1, key_minimum, key_maximum );
		sint32_c key_b_index = ops::math_clamp( key_current, key_minimum, key_maximum );
		sint32_c key_c_index = ops::math_clamp( key_current + 1, key_minimum, key_maximum );
		//sint32_c key_d_index = ops::clamp( key_current + 2, key_minimum, key_maximum );

		//const resource_object_model_c::animation_key_c & key_a = _source_model->_header_extras.animation_key_list[ key_a_index ];
		const resource_object_model_c::animation_key_c & key_b = _source_model->get_data().animation_key_list[ key_b_index ];
		const resource_object_model_c::animation_key_c & key_c = _source_model->get_data().animation_key_list[ key_c_index ];
		//const resource_object_model_c::animation_key_c & key_d = _source_model->_header_extras.animation_key_list[ key_d_index ];

		float32_c weight = 1.0f;
		if ( key_c.time > key_b.time )
		{
			weight = ( _time - key_b.time ) / ( key_c.time - key_b.time );
		}

		result.a = ops::interpolate_cosine( key_b.value[ 0 ], key_c.value[ 0 ], weight );
		result.b = ops::interpolate_cosine( key_b.value[ 1 ], key_c.value[ 1 ], weight );
		result.c = ops::interpolate_cosine( key_b.value[ 2 ], key_c.value[ 2 ], weight );
		result.d = ops::interpolate_cosine( key_b.value[ 3 ], key_c.value[ 3 ], weight );
		//result.a = ops::interpolate_hermite( key_a.value.a, key_b.value.a, key_c.value.a, key_d.value.a, weight, 0.0f, 0.0f );
		//result.b = ops::interpolate_hermite( key_a.value.b, key_b.value.b, key_c.value.b, key_d.value.b, weight, 0.0f, 0.0f );
		//result.c = ops::interpolate_hermite( key_a.value.c, key_b.value.c, key_c.value.c, key_d.value.c, weight, 0.0f, 0.0f );
		//result.d = ops::interpolate_hermite( key_a.value.d, key_b.value.d, key_c.value.d, key_d.value.d, weight, 0.0f, 0.0f );
	}

	boolean_c scene_component_model_c::animation_player_c::sample_position( sint32_c object_player_index, vector32x3_c & result )
	{
		object_player_c & object_player = _object_player_list[ object_player_index ];
		if ( object_player.property_player_for_position != nullptr )
		{
			vector32x4_c intermediate_result;
			sample_property_player( object_player.property_player_for_position, intermediate_result );
			result.a = intermediate_result.a;
			result.b = intermediate_result.b;
			result.c = intermediate_result.c;
			return true;
		}
		return false;
	}

	boolean_c scene_component_model_c::animation_player_c::sample_rotation( sint32_c object_player_index, quaternion32_c & result )
	{
		object_player_c & object_player = _object_player_list[ object_player_index ];
		if ( object_player.property_player_for_rotation != nullptr )
		{
			vector32x4_c intermediate_result;
			sample_property_player( object_player.property_player_for_rotation, intermediate_result );
			result.a = intermediate_result.a;
			result.b = intermediate_result.b;
			result.c = intermediate_result.c;
			result.d = intermediate_result.d;
			result = ops::make_quaternion32_normalized( result );
			return true;
		}
		return false;
	}

	boolean_c scene_component_model_c::animation_player_c::sample_scale( sint32_c object_player_index, vector32x3_c & result )
	{
		object_player_c & object_player = _object_player_list[ object_player_index ];
		if ( object_player.property_player_for_scale != nullptr )
		{
			vector32x4_c intermediate_result;
			sample_property_player( object_player.property_player_for_scale, intermediate_result );
			result.a = intermediate_result.a;
			result.b = intermediate_result.b;
			result.c = intermediate_result.c;
			return true;
		}
		return false;
	}

	scene_component_model_c::material_assignment_c::material_assignment_c( string8_c const & target_mesh_name )
		: _material()
		, _source_model_materials( nullptr )
		, _target_mesh_name( target_mesh_name )
	{
	}

	scene_material_c & scene_component_model_c::material_assignment_c::get_material()
	{
		return _material;
	}

	scene_component_model_c::light_c::light_c()
		: _source_light( nullptr )
		, _mother_bone( nullptr )
		, _local_space_transform()
		, _object_space_transform()
		, _scene_light()
	{
	}

	scene_light_c & scene_component_model_c::light_c::get_scene_light() const
	{
		return const_cast< scene_light_c & >( _scene_light );
	}

	scene_component_model_c::bone_c * scene_component_model_c::light_c::get_mother_bone() const
	{
		return _mother_bone;
	}

	space_transform_c const & scene_component_model_c::light_c::get_local_space_transform() const
	{
		return _local_space_transform;
	}

	space_transform_c const & scene_component_model_c::light_c::get_object_space_transform() const
	{
		return _object_space_transform;
	}

	scene_component_model_c::mesh_c::mesh_c()
		: _render_enable( true )
		, _mother_bone( nullptr )
		, _source_mesh( nullptr )
		, _material_assignment( nullptr )
	{
	}

	boolean_c scene_component_model_c::mesh_c::get_render_enable() const
	{
		return _render_enable;
	}

	void_c scene_component_model_c::mesh_c::set_render_enable( boolean_c value )
	{
		_render_enable = value;
	}

	scene_component_model_c::bone_c * scene_component_model_c::mesh_c::get_mother_bone() const
	{
		return _mother_bone;
	}

	resource_object_model_c::mesh_c const * scene_component_model_c::mesh_c::get_source_mesh() const
	{
		return _source_mesh;
	}

	scene_component_model_c::material_assignment_c * scene_component_model_c::mesh_c::get_material_assignment() const
	{
		return _material_assignment;
	}

	scene_component_model_c::bone_logic_c::bone_logic_c( scene_component_model_c * model, resource_object_model_c::bone_logic_c const * source_bone_logic )
		: _model( model )
		, _source_bone_logic( source_bone_logic )
		, _is_ready( false )
		, _is_simulated( false )
		, _this_bone_name()
		, _this_bone( nullptr )
		, _that_bone_name()
		, _that_bone( nullptr )
	{
		_model->_model_resource->find_bone_logic_property_as_string8( _source_bone_logic, string8_c( mode_e_static, "this_bone_name" ), _this_bone_name );
		_model->_model_resource->find_bone_logic_property_as_string8( _source_bone_logic, string8_c( mode_e_static, "that_bone_name" ), _that_bone_name );
	}

	void_c scene_component_model_c::bone_logic_c::start()
	{
	}

	void_c scene_component_model_c::bone_logic_c::update( float32_c time_step )
	{
	}

	scene_component_model_c::bone_logic_copy_transform_c::bone_logic_copy_transform_c( scene_component_model_c * model, resource_object_model_c::bone_logic_c const * source_bone_logic )
		: bone_logic_c( model, source_bone_logic )
	{
	}

	void_c scene_component_model_c::bone_logic_copy_transform_c::start()
	{
		_is_ready = false;
		_this_bone = _model->find_bone_with_name( _this_bone_name, false );
		_that_bone = _model->find_bone_with_name( _that_bone_name, true );
		_is_ready = _this_bone && _that_bone;
	}

	void_c scene_component_model_c::bone_logic_copy_transform_c::update( float32_c time_step )
	{
		if ( !_is_ready )
		{
			return;
		}

		_this_bone->_object_space_transform = _that_bone->_object_space_transform;
	}

	scene_component_model_c::bone_logic_copy_animation_c::bone_logic_copy_animation_c( scene_component_model_c * model, resource_object_model_c::bone_logic_c const * source_bone_logic )
		: bone_logic_c( model, source_bone_logic )
		, position_weight( 1.0f )
		, rotation_weight( 1.0f )
		, scale_weight( 1.0f )
	{
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( mode_e_static, "position_weight" ), position_weight );
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( mode_e_static, "rotation_weight" ), rotation_weight );
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( mode_e_static, "scale_weight" ), scale_weight );
	}

	void_c scene_component_model_c::bone_logic_copy_animation_c::start()
	{
		_is_ready = false;
		_this_bone = _model->find_bone_with_name( _this_bone_name, false );
		_that_bone = _model->find_bone_with_name( _that_bone_name, true );
		_is_ready = _this_bone && _that_bone;
	}

	void_c scene_component_model_c::bone_logic_copy_animation_c::update( float32_c time_step )
	{
		if ( !_is_ready )
		{
			return;
		}

		_this_bone->_local_space_transform.position = _that_bone->_local_space_transform.position * position_weight + _this_bone->_local_space_transform.position * ( 1.0f - position_weight );
		_this_bone->_local_space_transform.rotation = _that_bone->_local_space_transform.rotation * rotation_weight + _this_bone->_local_space_transform.rotation * ( 1.0f - rotation_weight );
		_this_bone->_local_space_transform.scale = _that_bone->_local_space_transform.scale * scale_weight + _this_bone->_local_space_transform.scale * ( 1.0f - scale_weight );
		_this_bone->build_object_space_transform_from_local_space_transform();
	}

	scene_component_model_c::bone_logic_muscle_c::bone_logic_muscle_c( scene_component_model_c * model, resource_object_model_c::bone_logic_c const * source_bone_logic )
		: bone_logic_c( model, source_bone_logic )
		, compression_factor( 1.0f )
	{
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( mode_e_static, "compression_factor" ), compression_factor );
	}

	void_c scene_component_model_c::bone_logic_muscle_c::start()
	{
		_is_ready = false;
		_this_bone = _model->find_bone_with_name( _this_bone_name, false );
		_that_bone = _model->find_bone_with_name( _that_bone_name, true );
		_is_ready = _this_bone && _that_bone;
	}

	void_c scene_component_model_c::bone_logic_muscle_c::update( float32_c time_step )
	{
		if ( !_is_ready )
		{
			return;
		}

		// build world space transform, because it is not updated automatically.
		// the head (position) of this bone will be where it's supposed to be.
		// we need to figure out where the tail of the bone is supposed to be.
		_this_bone->build_world_space_transform_from_object_space_transform( _model->_scene_object->get_world_space_transform() );

		// transform the rest pose tail position by a skin transform.
		space_transform_c object_space_skin_transform = _that_bone->_world_space_transform * _model->_scene_object->get_world_space_transform().get_inverted();
		vector64x3_c target_tail_position = ops::make_vector64x3_transformed_point( vector64x3_c( _this_bone->_source_bone->tail_position ), object_space_skin_transform );
		vector32x3_c target_axis = vector32x3_c( target_tail_position - _this_bone->_world_space_transform.position ); // tail position minus head position.
		float32_c target_axis_length = ops::make_float32_length( target_axis ); // get length, we'll need it to calculate compression.
		target_axis = target_axis / target_axis_length; // normalize target axis.

		float32_c scale_along = target_axis_length / _this_bone->_source_bone_extras->length; // scale of bone along main axis needed to stretch/compress the muscle.
		float32_c scale_across = 1.0f / ( ( ( scale_along - 1.0f ) * compression_factor ) + 1.0f ); // scale of bone around main axis needed to shrink/grow the muscle.
		_this_bone->_object_space_transform.set_unscaled_basis( ops::make_matrix32x3x3_basis_socket_rotated_c( _this_bone->_object_space_transform.get_unscaled_basis(), target_axis ) );
		_this_bone->_object_space_transform.scale.b = scale_along;
		_this_bone->_object_space_transform.scale.a = scale_across;
		_this_bone->_object_space_transform.scale.c = scale_across;

		_this_bone->build_object_space_skin_matrix_from_object_space_transform();
	}

	scene_component_model_c::bone_logic_jiggle_c::particle_c::particle_c()
		: tension( 0.25f )
		, damping( 0.15f )
		, limit( 0.1f )
		, gravity( 0.0f )
		, position()
		, velocity()
	{
	}

	void_c scene_component_model_c::bone_logic_jiggle_c::particle_c::update( float32_c time_step, vector64x3_c const & position_target, vector64x3_c const & model_velocity, vector64x3_c const & scene_gravity )
	{
		vector64x3_c position_start = position;
		position += velocity;
		position += scene_gravity * ( gravity * time_step );
		float64_c extra_tension = ops::math_saturate( ( ops::make_float64_length( position - position_target ) / limit ) + tension );
		position = ( position * ( 1.0 - extra_tension ) ) + ( position_target * extra_tension );
		velocity = ( position - position_start );
		velocity -= ( velocity - model_velocity ) * ( damping );
	}

	scene_component_model_c::bone_logic_jiggle_c::bone_logic_jiggle_c( scene_component_model_c * model, resource_object_model_c::bone_logic_c const * source_bone_logic )
		: bone_logic_c( model, source_bone_logic )
		, _head_particle()
		, _tail_particle()
	{
		_is_simulated = true;

		float32_c t;
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( mode_e_static, "head_tension" ), t ); _head_particle.tension = t;
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( mode_e_static, "head_damping" ), t ); _head_particle.damping = t;
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( mode_e_static, "head_limit" ), t ); _head_particle.limit = t;
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( mode_e_static, "head_gravity" ), t ); _head_particle.gravity = t;
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( mode_e_static, "tail_tension" ), t ); _tail_particle.tension = t;
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( mode_e_static, "tail_damping" ), t ); _tail_particle.damping = t;
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( mode_e_static, "tail_limit" ), t ); _tail_particle.limit = t;
		model->_model_resource->find_bone_logic_property_as_float32( source_bone_logic, string8_c( mode_e_static, "tail_gravity" ), t ); _tail_particle.gravity = t;
	}

	void_c scene_component_model_c::bone_logic_jiggle_c::start()
	{
		_is_ready = false;
		_this_bone = _model->find_bone_with_name( _this_bone_name, false );
		if ( _this_bone != nullptr )
		{
			_head_particle.position = _this_bone->_object_space_transform.position;
			_tail_particle.position = _this_bone->_object_space_transform.position + ( _this_bone->_object_space_transform.get_scaled_basis_c() * _this_bone->_source_bone_extras->length );
			_is_ready = true;
		}
	}

	void_c scene_component_model_c::bone_logic_jiggle_c::update( float32_c time_step )
	{
		if ( !_is_ready )
		{
			return;
		}

		//vector64x3_c scene_gravity = _model->_scene_object->_scene->_physics_scene.sample_gravity_force( _model->_scene_object->_world_transform.d );
		vector64x3_c scene_gravity = vector64x3_c( 0.0, 0.0, -9.8 );

		_this_bone->build_world_space_transform_from_object_space_transform( _model->get_scene_object()->get_world_space_transform() );

		_head_particle.update( time_step, _this_bone->_world_space_transform.position, _model->_object_velocity, scene_gravity );
		_tail_particle.update( time_step, _head_particle.position + _this_bone->_world_space_transform.get_scaled_basis_c() * _this_bone->_source_bone_extras->length, _model->_object_velocity, scene_gravity );

		vector64x3_c target_axis = _tail_particle.position - _head_particle.position;
		float64_c target_axis_length = ops::make_float64_length( target_axis );
		target_axis_length = ops::make_float64_length( target_axis ) / _this_bone->_source_bone_extras->length;
		target_axis = ops::make_vector64x3_normalized( target_axis );

		_this_bone->_world_space_transform = resource_object_model_c::make_bone_transform( _head_particle.position, _tail_particle.position, 0.0f, _this_bone->_world_space_transform.get_scaled_basis_c() );
		_this_bone->build_object_space_transform_from_world_space_transform( _model->get_scene_object()->get_world_space_transform().get_inverted() );
	}

	scene_component_model_c::bone_c::bone_c()
		: _model( nullptr )
		, _source_bone( nullptr )
		, _source_bone_extras( nullptr )
		, _mother_bone( nullptr )
		, _local_space_transform()
		, _object_space_transform()
		, _world_space_transform()
		, _object_space_skin_matrix()
		, _daughter_bone_list()
		, _daughter_bone_list_node( this )
	{
	}

	void_c scene_component_model_c::bone_c::build_object_space_transform_from_local_space_transform()
	{
		if ( _mother_bone != nullptr )
		{
			_object_space_transform = _local_space_transform * _source_bone_extras->local_space_rest_transform * _mother_bone->_object_space_transform;
		}
		else
		{
			_object_space_transform = _local_space_transform * _source_bone_extras->local_space_rest_transform;
		}
	}

	void_c scene_component_model_c::bone_c::build_world_space_transform_from_object_space_transform( space_transform_c const & scene_object_world_space_transform )
	{
		_world_space_transform = _object_space_transform * scene_object_world_space_transform;
	}

	void_c scene_component_model_c::bone_c::build_object_space_transform_from_world_space_transform( space_transform_c const & scene_object_world_space_transform_inverted )
	{
		_object_space_transform = _world_space_transform * scene_object_world_space_transform_inverted;
	}

	void_c scene_component_model_c::bone_c::build_object_space_skin_matrix_from_object_space_transform()
	{
		_object_space_skin_matrix = ops::make_matrix32x4x4_from_space_transform( _source_bone_extras->object_space_rest_transform_inverted * _object_space_transform );
	}

	void_c scene_component_model_c::bone_c::update_hierarchy()
	{
		build_object_space_transform_from_local_space_transform();
		build_object_space_skin_matrix_from_object_space_transform();
		core_linked_list_c< bone_c * >::node_c const * daughter_bone_list_node = _daughter_bone_list.get_first();
		while ( daughter_bone_list_node != nullptr )
		{
			daughter_bone_list_node->get_value()->update_hierarchy();
			daughter_bone_list_node = daughter_bone_list_node->get_next();
		}
	}

	space_transform_c const & scene_component_model_c::bone_c::get_local_space_transform() const
	{
		return _local_space_transform;
	}

	space_transform_c const & scene_component_model_c::bone_c::get_object_space_transform() const
	{
		return _object_space_transform;
	}

	space_transform_c const & scene_component_model_c::bone_c::get_world_space_transform() const
	{
		return _world_space_transform;
	}

	space_transform_c & scene_component_model_c::bone_c::get_world_space_transform()
	{
		return _world_space_transform;
	}

	scene_component_model_c::bone_attachment_c::bone_attachment_c()
		: _source_bone_attachment( nullptr )
		, _local_space_transform()
		, _object_space_transform()
		, _world_space_transform()
		, _mother_bone( nullptr )
	{
	}

	space_transform_c const & scene_component_model_c::bone_attachment_c::get_local_space_transform() const
	{
		return _local_space_transform;
	}

	space_transform_c const & scene_component_model_c::bone_attachment_c::get_object_space_transform() const
	{
		return _object_space_transform;
	}

	space_transform_c const & scene_component_model_c::bone_attachment_c::get_world_space_transform() const
	{
		return _object_space_transform;
	}

	void_c scene_component_model_c::_handle_model_resource_on_load( resource_object_c * resource )
	{
		assert( resource == _model_resource );
		assert( _model_resource_is_bound == false );
		assert( _model_resource.is_reference_set_and_loaded() == true );

		_model_resource_is_bound = true;

		_local_space_obb = box32x3_c( _model_resource->get_data().mesh_box );

		// instantiate meshes.
		if ( _model_resource->get_data().mesh_list.get_length() )
		{
			_mesh_list.construct_mode_dynamic( _model_resource->get_data().mesh_list.get_length(), _model_resource->get_data().mesh_list.get_length() );
			for ( sint32_c i = 0; i < _mesh_list.get_length(); i++ )
			{
				_mesh_list[ i ]._source_mesh = &_model_resource->get_data().mesh_list[ i ];
			}
		}
		apply_material_assignments();

		// instantiate bones.
		if ( _model_resource->get_data().bone_list.get_length() > 0 )
		{
			_bone_list.construct_mode_dynamic( _model_resource->get_data().bone_list.get_length(), _model_resource->get_data().bone_list.get_length() );
			for ( sint32_c i = 0; i < _bone_list.get_length(); i++ )
			{
				bone_c * bone = &_bone_list[ i ];
				bone->_model = this;
				bone->_source_bone = &_model_resource->get_data().bone_list[ i ];
				bone->_source_bone_extras = &_model_resource->get_data().bone_extras_list[ i ];
				sint32_c mother_bone_index = _model_resource->find_bone_index_with_bone_name( bone->_source_bone->mother_bone_name );
				if ( mother_bone_index >= 0 )
				{
					assert( mother_bone_index < i );
					bone->_mother_bone = &_bone_list[ mother_bone_index ];
					bone->_mother_bone->_daughter_bone_list.insert_at_end( &bone->_daughter_bone_list_node );
				}
				else
				{
					_root_bone_list.insert_at_end( &bone->_daughter_bone_list_node );
				}
			}
		}
		_update_bone_skin_matrix_list_hierarchy();

		// instantiate bone logics.
		for ( sint32_c i = 0; i < _model_resource->get_data().bone_logic_list.get_length(); i++ )
		{
			bone_logic_c * bone_logic = _instantiate_new_bone_logic( &_model_resource->get_data().bone_logic_list[ i ] );
			if ( bone_logic != nullptr )
			{
				_bone_logic_list.insert_at_end( bone_logic );
			}
		}

		// instantiate bone attachments.
		if ( _model_resource->get_data().bone_attachment_list.get_length() > 0 )
		{
			sint32_c count = _model_resource->get_data().bone_attachment_list.get_length();
			_bone_attachment_list.construct_mode_dynamic( count, count );
			for ( sint32_c i = 0; i < count; i++ )
			{
				bone_attachment_c * bone_attachment = &_bone_attachment_list[ i ];
				bone_attachment->_source_bone_attachment = &_model_resource->get_data().bone_attachment_list[ i ];
				sint32_c bone_index = _model_resource->find_bone_index_with_bone_name( bone_attachment->_source_bone_attachment->mother_bone_name );
				bone_attachment->_mother_bone = bone_index >= 0 ? &_bone_list[ bone_index ] : nullptr;
				space_transform_c object_space_rest_transform;
				object_space_rest_transform.position = vector64x3_c( bone_attachment->_source_bone_attachment->position );
				object_space_rest_transform.rotation = quaternion32_c( bone_attachment->_source_bone_attachment->rotation );
				object_space_rest_transform.set_uniform_scale( bone_attachment->_source_bone_attachment->scale );
				bone_attachment->_local_space_transform = object_space_rest_transform * bone_attachment->_mother_bone->_source_bone_extras->object_space_rest_transform_inverted;
			}
		}

		// instantiate lights.
		if ( _model_resource->get_data().light_list.get_length() > 0 )
		{
			sint32_c count = _model_resource->get_data().light_list.get_length();
			_light_list.construct_mode_dynamic( count, count );
			for ( sint32_c i = 0; i < count; i++ )
			{
				light_c * light = &_light_list[ i ];
				light->_source_light = &_model_resource->get_data().light_list[ i ];
				sint32_c bone_index = _model_resource->find_bone_index_with_bone_name( light->_source_light->mother_bone_name );
				light->_mother_bone = bone_index >= 0 ? &_bone_list[ bone_index ] : nullptr;
				light->_local_space_transform.position = vector64x3_c( light->_source_light->position );
				light->_local_space_transform.rotation = quaternion32_c( light->_source_light->rotation );
				light->_scene_light.set_type( light->_source_light->get_scene_light_type() );
				light->_scene_light.set_color( light->_source_light->color );
				light->_scene_light.set_brightness( light->_source_light->brightness );
				light->_scene_light.set_cone_angle( light->_source_light->cone_angle );
				light->_scene_light.set_render_enable( ( light->_source_light->flags & static_cast< uint16_c >( resource_object_model_c::light_c::flags_e::render_enable ) ) != 0 );
				light->_scene_light.set_shadow_cast_enable( ( light->_source_light->flags & static_cast< uint16_c >( resource_object_model_c::light_c::flags_e::shadow_cast_enable ) ) != 0 );
				if ( _scene_object != nullptr && _scene_object->get_scene() != nullptr )
				{
					_scene_object->get_scene()->add_light( &light->_scene_light );
				}
			}
		}

		// refresh animation map dictionary.
		refresh_animation_map_dictionary();

		// initialize bone skin matrix list.
		_bone_skin_matrix_list.construct_mode_dynamic( _model_resource->get_data().mesh_bone_name_list.get_length(), _model_resource->get_data().mesh_bone_name_list.get_length() );

		// invalidate light probes if needed.
		if ( _automatic_light_probe_invalidation_enable && _light_cast_enable )
		{
			invalidate_light_probes();
		}

		// reseat bones, bone attachments, and lights.
		reseat_bones();
		reseat_bone_attachments_and_lights();

		// restart all bone logics in the network.
		get_root_model()->_restart_bone_logics();

		// update with 0 time step so that animations apply but don't advance.
		update( 0.0f );

		// notify listeners that we are now bound.
		on_model_resource_bound.invoke( this );

		// notify animation players that we are now bound.
		_on_animation_resources_updated.invoke( this );
	}

	void_c scene_component_model_c::_handle_model_resource_on_un_load( resource_object_c * resource )
	{
		assert( resource == _model_resource );

		// mark our base as un bound.
		_model_resource_is_bound = false;

		// notify animation players that we are now un bound.
		_on_animation_resources_updated.invoke( this );

		// notify listeners that we are now un bound.
		on_model_resource_un_bound.invoke( this );

		// update animation lookup dictionary.
		refresh_animation_map_dictionary();

		// make daughters let go of any references that they might have to our bones (since we set our _model_resource_is_bound to false, our daughters won't try to look us up again).
		core_linked_list_c< scene_component_model_c * >::node_c const * daughter_model_list_node = _daughter_model_list.get_first();
		if ( daughter_model_list_node != nullptr )
		{
			while ( daughter_model_list_node != nullptr )
			{
				daughter_model_list_node->get_value()->_restart_bone_logics();
				daughter_model_list_node = daughter_model_list_node->get_next();
			}
			_update_bone_skin_matrix_list();
			daughter_model_list_node = _daughter_model_list.get_first();
			while ( daughter_model_list_node != nullptr )
			{
				daughter_model_list_node->get_value()->_update_bone_skin_matrix_list();
				daughter_model_list_node = daughter_model_list_node->get_next();
			}
		}

		// delete bone attachments.
		_bone_attachment_list.remove_all();

		// delete bone logics.
		for ( sint32_c i = 0; i < _bone_logic_list.get_length(); i++ )
		{
			delete _bone_logic_list[ i ];
		}
		_bone_logic_list.remove_all();

		// delete bones.
		_root_bone_list.remove_all_quick_and_dirty();
		_bone_list.remove_all();
		_bone_skin_matrix_list.remove_all();
		_update_bone_skin_matrix_list_hierarchy();

		// delete mesh stuff.
		unapply_material_assignments();
		if ( _mesh_vertex_buffer_skinned )
		{
			delete _mesh_vertex_buffer_skinned;
			_mesh_vertex_buffer_skinned = nullptr;
		}
		_mesh_list.remove_all();

		// invalidate light probes.
		if ( _automatic_light_probe_invalidation_enable && _light_cast_enable )
		{
			invalidate_light_probes();
		}

		// reset local space bounding box.
		_reset_local_space_obb();
	}

	void_c scene_component_model_c::_handle_animations_resource_on_load( resource_object_c * resource )
	{
		refresh_animation_map_dictionary();
		_on_animation_resources_updated.invoke( this );
	}

	void_c scene_component_model_c::_handle_animations_resource_on_un_load( resource_object_c * resource )
	{
		refresh_animation_map_dictionary();
		_on_animation_resources_updated.invoke( this );
	}

	void_c scene_component_model_c::_handle_material_map_resource_on_load( resource_object_c * resource )
	{
		assert( resource == _material_map_resource );

		unapply_material_assignments();

		for ( sint32_c i = 0; i < _material_map_resource->things.get_length(); i++ )
		{
			material_assignment_c * material_assignment = new material_assignment_c( _material_map_resource->things[ i ]->target_name );
			material_assignment->_source_model_materials = dynamic_cast< resource_object_material_map_c * >( resource );
			material_assignment->_material = _material_map_resource->things[ i ]->material;
			material_assignment_list.insert_at_end( material_assignment );
		}

		apply_material_assignments();
	}

	void_c scene_component_model_c::_handle_material_map_resource_on_unload( resource_object_c * resource )
	{
		assert( resource == _material_map_resource );

		unapply_material_assignments();

		for ( sint32_c i = 0; i < material_assignment_list.get_length(); i++ )
		{
			if ( material_assignment_list[ i ]->_source_model_materials == resource )
			{
				material_assignment_list.remove_at_index( i );
				i--;
			}
		}

		apply_material_assignments();
	}

	video_vertex_buffer_c * scene_component_model_c::_get_vertex_buffer_for_rendering() const
	{
		assert( _model_resource_is_bound );
		if ( _bone_skin_matrix_list.get_length() > 0 )
		{
			assert( _mesh_vertex_buffer_skinned != nullptr );
			return _mesh_vertex_buffer_skinned;
		}
		else
		{
			assert( _model_resource != nullptr );
			assert( _model_resource->get_data().mesh_vertex_buffer_base != nullptr );
			return _model_resource->get_data().mesh_vertex_buffer_base;
		}
	}

	void_c scene_component_model_c::_gather_meshes_with_name( string8_c const & name, core_list_c< mesh_c * > & result )
	{
		string8_c name_2;
		name_2 = name;
		boolean_c wild_card = ops::string8_ends_with( name_2.character_list.get_internal_array(), "*" );
		if ( wild_card )
		{
			name_2 = ops::string8_sub_string( name_2, 0, name_2.character_list.get_length() - 2 );
			for ( sint32_c i = 0; i < _mesh_list.get_length(); i++ )
			{
				if ( ops::string8_starts_with( &_model_resource->get_data().string_table[ _mesh_list[ i ]._source_mesh->name ], name_2.character_list.get_internal_array() ) )
				{
					result.insert_at_end( &_mesh_list[ i ] );
				}
			}
		}
		else
		{
			for ( sint32_c i = 0; i < _mesh_list.get_length(); i++ )
			{
				if ( name_2 == &_model_resource->get_data().string_table[ _mesh_list[ i ]._source_mesh->name ] )
				{
					result.insert_at_end( &_mesh_list[ i ] );
				}
			}
		}
	}

	void_c scene_component_model_c::unapply_material_assignments()
	{
		for ( sint32_c i = 0; i < _mesh_list.get_length(); i++ )
		{
			_mesh_list[ i ]._material_assignment = nullptr;
		}
	}

	void_c scene_component_model_c::apply_material_assignments()
	{
		for ( sint32_c i = 0; i < material_assignment_list.get_length(); i++ )
		{
			material_assignment_c * material_assignment = material_assignment_list[ i ];
			core_list_c< mesh_c * > mesh_list;
			_gather_meshes_with_name( material_assignment->_target_mesh_name, mesh_list );
			for ( sint32_c j = 0; j < mesh_list.get_length(); j++ )
			{
				mesh_list[ j ]->_material_assignment = material_assignment;
			}
		}
	}

	scene_component_model_c::bone_logic_c * scene_component_model_c::_instantiate_new_bone_logic( resource_object_model_c::bone_logic_c const * source_bone_logic )
	{
		string8_c source_bone_logic_type( mode_e_static, &_model_resource->get_data().string_table[ source_bone_logic->type ] );
		if ( source_bone_logic_type == bone_logic_copy_transform_c::get_type_static() )
		{
			return new bone_logic_copy_transform_c( this, source_bone_logic );
		}
		else if ( source_bone_logic_type == bone_logic_copy_animation_c::get_type_static() )
		{
			return new bone_logic_copy_animation_c( this, source_bone_logic );
		}
		else if ( source_bone_logic_type == bone_logic_jiggle_c::get_type_static() )
		{
			return new bone_logic_jiggle_c( this, source_bone_logic );
		}
		return nullptr;
	}

	scene_component_model_c::bone_c * scene_component_model_c::find_bone_at_path( string8_c const & path )
	{
		core_list_c< string8_c > path_nodes;
		ops::string8_split_at_delimiter( path, string8_c( mode_e_static, "/" ), path_nodes );
		bone_c * bone = nullptr;
		core_linked_list_c< bone_c * >::node_c const * bone_list_node = _root_bone_list.get_first();
		sint32_c i = 0;
		while ( bone_list_node != nullptr && i < path_nodes.get_length() )
		{
			while ( bone_list_node != nullptr )
			{
				if ( path_nodes[ i ] == &_model_resource->get_data().string_table[ bone_list_node->get_value()->_source_bone->name ] )
				{
					bone = bone_list_node->get_value();
					if ( i + 1 == path_nodes.get_length() )
					{
						return bone;
					}
					bone_list_node = bone->_daughter_bone_list.get_first();
					break;
				}
				bone_list_node = bone_list_node->get_next();
			}
			i++;
		}
		return nullptr;
	}

	scene_component_model_c::bone_c * scene_component_model_c::find_bone_with_name( string8_c const & name, boolean_c search_mother_first )
	{
		assert( _model_resource != nullptr && _model_resource_is_bound );

		if ( search_mother_first && _mother_model != nullptr && _mother_model->get_model_resource_is_bound() )
		{
			bone_c * bone = _mother_model->find_bone_with_name( name, false );
			if ( bone )
			{
				return bone;
			}
		}

		for ( sint32_c i = 0; i < _bone_list.get_length(); i++ )
		{
			bone_c * bone = &_bone_list[ i ];
			if ( name == &_model_resource->get_data().string_table[ bone->_source_bone->name ] )
			{
				return bone;
			}
		}
		return nullptr;
	}

	void_c scene_component_model_c::find_bones_with_name( string8_c const & name, core_list_c< bone_c * > & result )
	{
		assert( _model_resource != nullptr );
		string8_c name_2 = name;
		boolean_c wild_card = ops::string8_ends_with( name_2.character_list.get_internal_array(), "*" );
		if ( wild_card )
		{
			if ( name_2.character_list.get_mode_is_static() )
			{
				name_2.character_list.convert_from_static_to_dynamic();
			}
			sint32_c end = name_2.character_list.get_length() - 1;
			name_2.character_list.set_length( end );
			name_2.character_list[ end ] = '\0';
		}
		for ( sint32_c i = 0; i < _bone_list.get_length(); i++ )
		{
			bone_c * bone = &_bone_list[ i ];
			if ( wild_card )
			{
				if ( ops::string8_starts_with( &_model_resource->get_data().string_table[ bone->_source_bone->name ], name_2.character_list.get_internal_array() ) )
				{
					result.insert_at_end( bone );
				}
			}
			else
			{
				if ( name_2 == &_model_resource->get_data().string_table[ bone->_source_bone->name ] )
				{
					result.insert_at_end( bone );
				}
			}
		}
	}

	sint32_c scene_component_model_c::find_bone_index_with_name( string8_c const & name )
	{
		assert( _model_resource != nullptr );
		for ( sint32_c i = 0; i < _bone_list.get_length(); i++ )
		{
			bone_c * bone = &_bone_list[ i ];
			if ( name == &_model_resource->get_data().string_table[ bone->_source_bone->name ] )
			{
				return i;
			}
		}
		return -1;
	}

	scene_component_model_c::bone_attachment_c * scene_component_model_c::find_bone_attachment_with_name( string8_c const & name )
	{
		assert( _model_resource != nullptr );
		for ( sint32_c i = 0; i < _bone_attachment_list.get_length(); i++ )
		{
			bone_attachment_c * bone_attachment = &_bone_attachment_list[ i ];
			if ( name == &_model_resource->get_data().string_table[ bone_attachment->_source_bone_attachment->name ] )
			{
				return bone_attachment;
			}
		}
		return nullptr;
	}

	void_c scene_component_model_c::_update_bone_skin_matrix_list_hierarchy()
	{
		_update_bone_skin_matrix_list();
		core_linked_list_c< scene_component_model_c * >::node_c const * daughter_component_list_node = _daughter_model_list.get_first();
		while ( daughter_component_list_node != nullptr )
		{
			daughter_component_list_node->get_value()->_update_bone_skin_matrix_list_hierarchy();
			daughter_component_list_node = daughter_component_list_node->get_next();
		}
	}

	void_c scene_component_model_c::_update_bone_skin_matrix_list()
	{
		if ( _model_resource_is_bound == false )
		{
			_bone_skin_matrix_list.remove_all();
			return;
		}

		if ( _model_resource->get_data().mesh_bone_name_list.get_length() > 0 )
		{
			// instantiate transform list if needed.
			if ( _bone_skin_matrix_list.get_length() == 0 )
			{
				_bone_skin_matrix_list.construct_mode_dynamic( _model_resource->get_data().mesh_bone_name_list.get_length(), _model_resource->get_data().mesh_bone_name_list.get_length() );
			}

			// resolve bone skin matrices.
			// prioritize our model first, and traverse up the hierarchy until we find the bone.
			for ( sint32_c i = 0; i < _bone_skin_matrix_list.get_length(); i++ )
			{
				string8_c & bone_name = string8_c( mode_e_static, &_model_resource->get_data().string_table[ _model_resource->get_data().mesh_bone_name_list[ i ] ] );
				bone_c * bone = nullptr;
				scene_component_model_c * model_component = this;
				while ( model_component != nullptr )
				{
					if ( model_component->_model_resource_is_bound )
					{
						bone = model_component->find_bone_with_name( bone_name, false );
						if ( bone != nullptr )
						{
							break;
						}
					}
					model_component = model_component->_mother_model;
				}
				_bone_skin_matrix_list[ i ] = bone != nullptr ? & bone->_object_space_skin_matrix : nullptr;
			}
		}
	}

	void_c scene_component_model_c::_restart_bone_logics()
	{
		for ( sint32_c i = 0; i < _bone_logic_list.get_length(); i++ )
		{
			_bone_logic_list[ i ]->start();
		}

		core_linked_list_c< scene_component_model_c * >::node_c const * daughter_component_list_node = _daughter_model_list.get_first();
		while ( daughter_component_list_node != nullptr )
		{
			daughter_component_list_node->get_value()->_restart_bone_logics();
			daughter_component_list_node = daughter_component_list_node->get_next();
		}
	}

	scene_component_model_c * scene_component_model_c::get_mother_model()
	{
		return _mother_model;
	}

	void_c scene_component_model_c::set_mother_model( scene_component_model_c * model )
	{
		if ( _mother_model != nullptr )
		{
			_mother_model->_daughter_model_list.remove( &_daughter_model_list_node );
		}
		_mother_model = model;
		if ( _mother_model != nullptr )
		{
			_mother_model->_daughter_model_list.insert_at_end( &_daughter_model_list_node );
		}
		_update_bone_skin_matrix_list_hierarchy();
		_restart_bone_logics();
	}

	void_c scene_component_model_c::_handle_after_added_to_scene()
	{
		if ( _scene_object->get_scene()->_suspend_automatic_light_probe_invalidation == false )
		{
			invalidate_light_probes();
		}

		for ( sint32_c i = 0; i < _light_list.get_length(); i++ )
		{
			_scene_object->get_scene()->add_light( &_light_list[ i ]._scene_light );
		}
	}

	void_c scene_component_model_c::_handle_before_removed_from_scene()
	{
		for ( sint32_c i = 0; i < _light_list.get_length(); i++ )
		{
			_scene_object->get_scene()->remove_light( &_light_list[ i ]._scene_light );
		}

		if ( _scene_object->get_scene()->_suspend_automatic_light_probe_invalidation == false )
		{
			invalidate_light_probes();
		}
	}

	void_c scene_component_model_c::_handle_before_property_modified( scene_object_property_e property )
	{
		if ( _automatic_light_probe_invalidation_enable && ( property & scene_object_property_e_position || property & scene_object_property_e_rotation || property & scene_object_property_e_scale ) )
		{
			invalidate_light_probes();
		}
	}

	void_c scene_component_model_c::_handle_after_property_modified( scene_object_property_e property )
	{
		if ( _automatic_light_probe_invalidation_enable && ( property & scene_object_property_e_position || property & scene_object_property_e_rotation || property & scene_object_property_e_scale ) )
		{
			invalidate_light_probes();
		}
	}

	scene_component_model_c::scene_component_model_c()
		: scene_component_c()
		, _model_resource_is_bound( false )
		, _model_resource()
		, _animations_resource_list()
		, _material_map_resource()
		, _object_velocity()
		, _object_position_last()
		, _opacity( 1.0f )
		, _render_enable( true )
		, _shadow_cast_enable( true )
		, _light_cast_enable( true )
		, _automatic_light_probe_invalidation_enable( true )
		, _local_light_probe_enable( false )
		, _local_light_probe_component( nullptr )
		, _fixed_light_probe_component( nullptr )
		, _render_light_indices_count( 0 )
		, _render_light_indices()
		, _render_frame_last( 0 )
		, _lod_slice( 0 )
		, _physics_enable( true )
		, _mesh_list()
		, _vertex_skin_mode( vertex_skin_mode_e_gpu )
		, _mesh_vertex_list_cpu_skinned()
		, _mesh_vertex_buffer_skinned( nullptr )
		, material_assignment_list()
		, _bone_list()
		, _root_bone_list()
		, _bone_attachment_list()
		, _bone_skin_matrix_list()
		, _bone_logic_list()
		, _animation_map_dictionary()
		, animation_player_list()
		, _on_animation_resources_updated()
		, _mother_model( nullptr )
		, _daughter_model_list()
		, _daughter_model_list_node( this )
		, object_colors_enable( false )
		, object_colors()
		, object_textures_enable( false )
		, object_textures()
		, on_model_resource_bound()
		, on_model_resource_un_bound()
	{
	}

	scene_component_model_c::~scene_component_model_c()
	{
		assert( _mother_model == nullptr ); // should be disassociated before being deleted.
		assert( animation_player_list.get_length() == 0 ); // we don't want to risk taking responsibility for managing dangling animation player instances, so we assert here to warn about potential for leak. these animation players could be on the heap or the stack, there may be others that were allocated for us but are not currently in our list, etc. paranoid.
		set_material_map_resource( nullptr ); // releases reference and unsubscribes from events.
		remove_all_animations_resources(); // releases reference and unsubscribes from events.
		set_model_resource( nullptr ); // releases reference and unsubscribes from events.
	}

	void_c scene_component_model_c::update( float32_c time_step )
	{
		// update velocity.
		if ( _mother_model == nullptr )
		{
			_object_velocity = ( _scene_object->get_world_space_transform().position - _object_position_last );
			_object_position_last = _scene_object->get_world_space_transform().position;
		}

		// reset pose properties so we can apply layered animations properly.
		for ( sint32_c i = 0; i < _bone_list.get_length(); i++ )
		{
			_bone_list[ i ]._local_space_transform.reset();
		}

		// update animation players and sample animated properties.
		vector32x3_c sampled_position;
		quaternion32_c sampled_orientation;
		vector32x3_c sampled_scale;
		for ( sint32_c i = 0; i < animation_player_list.get_length(); i++ )
		{
			animation_player_c * animation_player = animation_player_list[ i ];
			if ( animation_player == nullptr || animation_player->_source_animation == nullptr || animation_player->_blend_weight == 0.0f )
			{
				continue;
			}
			animation_player->update( time_step );
			if ( animation_player->get_blend_type() == animation_player_c::blend_type_e_mix )
			{
				for ( sint32_c object_player_index = 0; object_player_index < animation_player->_object_player_list.get_length(); object_player_index++ )
				{
					animation_player_c::object_player_c & object_player = animation_player->_object_player_list[ object_player_index ];
					if ( object_player.object_index_for_bone >= 0 )
					{
						bone_c * bone = &_bone_list[ object_player.object_index_for_bone ];
						if ( animation_player->sample_position( object_player_index, sampled_position ) )
						{
							bone->_local_space_transform.position = vector64x3_c( ops::interpolate_linear( vector32x3_c( bone->_local_space_transform.position ), sampled_position, animation_player->_blend_weight ) );
						}
						if ( animation_player->sample_rotation( object_player_index, sampled_orientation ) )
						{
							bone->_local_space_transform.rotation = ops::interpolate_spherical_linear( bone->_local_space_transform.rotation, sampled_orientation, animation_player->_blend_weight );
						}
						if ( animation_player->sample_scale( object_player_index, sampled_scale ) )
						{
							bone->_local_space_transform.scale = ops::interpolate_linear( bone->_local_space_transform.scale, sampled_scale, animation_player->_blend_weight );
						}
					}
				}
			}
			else if ( animation_player->_blend_type == animation_player_c::blend_type_e_add )
			{
				for ( sint32_c object_player_index = 0; object_player_index < animation_player->_object_player_list.get_length(); object_player_index++ )
				{
					animation_player_c::object_player_c & object_player = animation_player->_object_player_list[ object_player_index ];
					if ( object_player.object_index_for_bone >= 0 )
					{
						bone_c * bone = &_bone_list[ object_player.object_index_for_bone ];
						if ( animation_player->sample_position( object_player_index, sampled_position ) )
						{
							bone->_local_space_transform.position = bone->_local_space_transform.position + sampled_position * animation_player->_blend_weight;
						}
						if ( animation_player->sample_rotation( object_player_index, sampled_orientation ) )
						{
							bone->_local_space_transform.rotation = bone->_local_space_transform.rotation * ops::interpolate_spherical_linear( quaternion32_c(), sampled_orientation, animation_player->_blend_weight );
						}
						if ( animation_player->sample_scale( object_player_index, sampled_scale ) )
						{
							bone->_local_space_transform.scale = bone->_local_space_transform.scale * ops::interpolate_linear( 1.0f, sampled_scale.a, animation_player->_blend_weight );
						}
					}
				}
			}
		}

		// update object space and world space transforms of bones from animated local space properties.
		core_linked_list_c< bone_c * >::node_c const * root_bone_list_node = _root_bone_list.get_first();
		while ( root_bone_list_node != nullptr )
		{
			root_bone_list_node->get_value()->update_hierarchy();
			root_bone_list_node = root_bone_list_node->get_next();
		}

		// update non-simulated bone logics.
		for ( sint32_c i = 0; i < _bone_logic_list.get_length(); i++ )
		{
			bone_logic_c * bone_logic = _bone_logic_list[ i ];
			if ( bone_logic->_is_simulated == false )
			{
				bone_logic->update( time_step );
			}
		}

		// update world space transforms of bone attachments and lights.
		reseat_bone_attachments_and_lights();

		// update daughter models.
		core_linked_list_c< scene_component_model_c * >::node_c const * daughter_component_list_node = _daughter_model_list.get_first();
		while ( daughter_component_list_node )
		{
			daughter_component_list_node->get_value()->update( time_step );
			daughter_component_list_node = daughter_component_list_node->get_next();
		}

		// update local space bounding box.
		if ( _bone_list.get_length() > 0 )
		{
			boolean_c first_bone = true;
			for ( sint32_c i = 0; i < _bone_list.get_length(); i++ )
			{
				bone_c * bone = &_bone_list[ i ];
				if ( ( bone->_source_bone->flags & static_cast< uint8_c >( resource_object_model_c::bone_c::flags_e::do_not_rebound ) ) != 0 )
				{
					continue;
				}
				vector32x3_c head_point = ops::make_vector32x3_transformed_point( vector32x3_c( bone->_source_bone->head_position ), bone->_object_space_skin_matrix );
				vector32x3_c tail_point = ops::make_vector32x3_transformed_point( vector32x3_c( bone->_source_bone->tail_position ), bone->_object_space_skin_matrix );
				float32_c head_radius = bone->_source_bone->head_radius * bone->_object_space_transform.scale.get_largest_element();
				float32_c tail_radius = bone->_source_bone->tail_radius * bone->_object_space_transform.scale.get_largest_element();
				if ( first_bone == false )
				{
					_local_space_obb.accumulate_bounds( head_point, head_radius );
					_local_space_obb.accumulate_bounds( tail_point, tail_radius );
				}
				else
				{
					first_bone = false;
					_local_space_obb.set_bounds( head_point, head_radius );
					_local_space_obb.accumulate_bounds( tail_point, tail_radius );
				}
			}
			if ( first_bone == true )
			{
				_local_space_obb = box32x3_c( _model_resource->get_data().mesh_box );
			}
		}
	}

	void_c scene_component_model_c::update_simulated_bone_logics( float32_c time_step )
	{
		// update simulated bone logics.
		for ( sint32_c i = 0; i < _bone_logic_list.get_length(); i++ )
		{
			bone_logic_c * bone_logic = _bone_logic_list[ i ];
			if ( bone_logic->_is_simulated == true )
			{
				bone_logic->update( time_step );
			}
		}
	}

	void_c scene_component_model_c::invalidate_light_probes()
	{
		_scene_object->get_scene()->invalidate_light_probes_with_box( box64x3_c( _local_space_obb ), _scene_object->get_world_space_transform() );
	}

	void_c scene_component_model_c::reseat()
	{
		assert( _scene_object );

		if ( _mother_model )
		{
			assert( _scene_object == _mother_model->_scene_object );
			//_object->_position = vector32x3_c();
			//_object->_rotation = quaternion64_c();
			//_object->_scale = vector32x3_c( 1.0f, 1.0f, 1.0f );
			//_object->_world_transform = _mother_model_component->_object->_world_transform;
			//_scene_transform_inverted = make_inverted( _object->_world_transform );
		}

		_object_position_last = _scene_object->get_world_space_transform().position;
		_object_velocity = vector64x3_c( 0.0f, 0.0f, 0.0f );

		reseat_bones(); // this may not be needed?
		for ( sint32_c i = 0; i < _bone_logic_list.get_length(); i++ )
		{
			_bone_logic_list[ i ]->start();
		}
		reseat_bone_attachments_and_lights();
	}

	void_c scene_component_model_c::reseat_bones()
	{
		core_linked_list_c< bone_c * >::node_c const * root_bone_list_node = _root_bone_list.get_first();
		while ( root_bone_list_node != nullptr )
		{
			root_bone_list_node->get_value()->update_hierarchy();
			root_bone_list_node = root_bone_list_node->get_next();
		}
	}

	void_c scene_component_model_c::reseat_bone_attachments_and_lights()
	{
		// update bone attachments.
		for ( sint32_c i = 0; i < _bone_attachment_list.get_length(); i++ )
		{
			bone_attachment_c * bone_attachment = &_bone_attachment_list[ i ];
			if ( bone_attachment->_mother_bone )
			{
				bone_attachment->_object_space_transform = bone_attachment->_local_space_transform * bone_attachment->_mother_bone->_object_space_transform;
			}
			else
			{
				bone_attachment->_object_space_transform = bone_attachment->_local_space_transform;
			}
			bone_attachment->_world_space_transform = bone_attachment->_object_space_transform * _scene_object->get_world_space_transform();
		}

		// update lights.
		for ( sint32_c i = 0; i < _light_list.get_length(); i++ )
		{
			light_c * light = &_light_list[ i ];
			if ( light->_mother_bone )
			{
				light->_object_space_transform = light->_local_space_transform * light->_mother_bone->_object_space_transform;
			}
			else
			{
				light->_object_space_transform = light->_local_space_transform;
			}
			light->get_scene_light().set_world_space_transform( light->_object_space_transform * _scene_object->get_world_space_transform() );
		}
	}

	scene_component_model_c * scene_component_model_c::get_root_model()
	{
		scene_component_model_c * result = this;
		while ( result->_mother_model )
		{
			result = result->_mother_model;
		}
		return result;
	}

	resource_object_model_c * scene_component_model_c::get_model_resource()
	{
		return _model_resource;
	}

	void_c scene_component_model_c::set_model_resource( resource_object_model_c * resource )
	{
		if ( _model_resource == resource )
		{
			return;
		}

		if ( _model_resource )
		{
			_model_resource->on_load.unsubscribe( this, &scene_component_model_c::_handle_model_resource_on_load );
			_model_resource->on_unload.unsubscribe( this, &scene_component_model_c::_handle_model_resource_on_un_load );
			if ( _model_resource_is_bound )
			{
				_handle_model_resource_on_un_load( _model_resource );
			}
		}

		_model_resource = resource;

		if ( _model_resource )
		{
			_model_resource->on_load.subscribe( this, &scene_component_model_c::_handle_model_resource_on_load );
			_model_resource->on_unload.subscribe( this, &scene_component_model_c::_handle_model_resource_on_un_load );
			if ( _model_resource->get_is_loaded() == true )
			{
				_handle_model_resource_on_load( _model_resource );
			}
		}
	}

	boolean_c scene_component_model_c::get_model_resource_is_bound()
	{
		return _model_resource_is_bound;
	}

	void_c scene_component_model_c::add_animations_resource( resource_object_model_c * resource )
	{
		assert( _animations_resource_list.find_index_of( resource ) < 0 );
		resource->on_load.subscribe( this, &scene_component_model_c::_handle_animations_resource_on_load );
		resource->on_unload.subscribe( this, &scene_component_model_c::_handle_animations_resource_on_un_load );
		_animations_resource_list.insert_at_end( resource );
	}

	void_c scene_component_model_c::remove_animations_resource( resource_object_model_c * resource )
	{
		assert( _animations_resource_list.find_index_of( resource ) >= 0 );
		resource->on_load.unsubscribe( this, &scene_component_model_c::_handle_animations_resource_on_load );
		resource->on_unload.unsubscribe( this, &scene_component_model_c::_handle_animations_resource_on_un_load );
		_animations_resource_list.remove( resource );
	}

	sint32_c scene_component_model_c::get_animations_resource_count()
	{
		return _animations_resource_list.get_length();
	}

	resource_object_model_c * scene_component_model_c::get_animations_resource_at_index( sint32_c index )
	{
		return _animations_resource_list[ index ];
	}

	void_c scene_component_model_c::remove_all_animations_resources()
	{
		for ( sint32_c i = 0; i < _animations_resource_list.get_length(); i++ )
		{
			resource_object_model_c * animations_resource = _animations_resource_list[ i ];
			animations_resource->on_load.unsubscribe( this, &scene_component_model_c::_handle_animations_resource_on_load );
			animations_resource->on_unload.unsubscribe( this, &scene_component_model_c::_handle_animations_resource_on_un_load );
		}
		_animations_resource_list.remove_all();
		refresh_animation_map_dictionary();
	}

	void_c scene_component_model_c::refresh_animation_map_dictionary()
	{
		core_list_c< resource_object_model_c * > animations_resource_list;
		if ( _model_resource->get_is_loaded() )
		{
			animations_resource_list.insert_at_end( _model_resource );
		}
		for ( sint32_c i = 0; i < _animations_resource_list.get_length(); i++ )
		{
			if ( _animations_resource_list[ i ]->get_is_loaded() )
			{
				animations_resource_list.insert_at_end( _animations_resource_list[ i ] );
			}
		}

		_animation_map_dictionary.remove_all();
		for ( sint32_c j = 0; j < animations_resource_list.get_length(); j++ )
		{
			resource_object_model_c * animation_resource = animations_resource_list[ j ];
			for ( sint32_c i = 0; i < animation_resource->get_data().animation_list.get_length(); i++ )
			{
				animation_map_c animation_map;
				animation_map.source_model = animation_resource;
				animation_map.source_animation = &animation_resource->get_data().animation_list[ i ];
				_animation_map_dictionary.insert( string8_c( mode_e_static, &animation_resource->get_data().string_table[ animation_map.source_animation->name ] ), animation_map );
			}
		}
	}

	resource_object_material_map_c * scene_component_model_c::get_material_map_resource()
	{
		return _material_map_resource;
	}

	void_c scene_component_model_c::set_material_map_resource( resource_object_material_map_c * value )
	{
		if ( _material_map_resource )
		{
			_material_map_resource->on_load.unsubscribe( this, & scene_component_model_c::_handle_material_map_resource_on_load );
			_material_map_resource->on_unload.unsubscribe( this, & scene_component_model_c::_handle_material_map_resource_on_unload );
			if ( _material_map_resource->get_is_loaded() )
			{
				_handle_material_map_resource_on_unload( _material_map_resource );
			}
		}

		_material_map_resource = value;

		if ( _material_map_resource )
		{
			_material_map_resource->on_load.subscribe( this, & scene_component_model_c::_handle_material_map_resource_on_load );
			_material_map_resource->on_unload.subscribe( this, & scene_component_model_c::_handle_material_map_resource_on_unload );
			if ( _material_map_resource->get_is_loaded() )
			{
				_handle_material_map_resource_on_load( _material_map_resource );
			}
		}
	}

	scene_component_model_c::vertex_skin_mode_e scene_component_model_c::get_vertex_skin_mode() const
	{
		return _vertex_skin_mode;
	}

	void_c scene_component_model_c::set_vertex_skin_mode( vertex_skin_mode_e value )
	{
		if ( _vertex_skin_mode != value )
		{
			_vertex_skin_mode = value;
			if ( _mesh_vertex_buffer_skinned )
			{
				delete _mesh_vertex_buffer_skinned;
				_mesh_vertex_buffer_skinned = nullptr;
			}
		}
	}

	float32_c scene_component_model_c::get_opacity() const
	{
		return _opacity;
	}

	void_c scene_component_model_c::set_opacity( float32_c value )
	{
		_opacity = value;
	}

	boolean_c scene_component_model_c::get_render_enable() const
	{
		return _render_enable;
	}

	void_c scene_component_model_c::set_render_enable( boolean_c value )
	{
		_render_enable = value;
	}

	boolean_c scene_component_model_c::get_shadow_cast_enable() const
	{
		return _shadow_cast_enable;
	}

	void_c scene_component_model_c::set_shadow_cast_enable( boolean_c value )
	{
		_shadow_cast_enable = value;
	}

	boolean_c scene_component_model_c::get_light_cast_enable() const
	{
		return _light_cast_enable;
	}

	void_c scene_component_model_c::set_light_cast_enable( boolean_c value )
	{
		_light_cast_enable = value;
	}

	boolean_c scene_component_model_c::get_automatic_light_probe_invalidation_enable() const
	{
		return _automatic_light_probe_invalidation_enable;
	}

	void_c scene_component_model_c::set_automatic_light_probe_invalidation_enable( boolean_c value )
	{
		_automatic_light_probe_invalidation_enable = value;
	}

	boolean_c scene_component_model_c::get_local_light_probe_enable() const
	{
		return _local_light_probe_enable;
	}

	void_c scene_component_model_c::set_local_light_probe_enable( boolean_c value )
	{
		if ( _local_light_probe_enable != value )
		{
			_local_light_probe_enable = value;
			if ( _local_light_probe_enable )
			{
				assert( _local_light_probe_component == nullptr );
				_local_light_probe_component = new scene_component_light_probe_c();
				_local_light_probe_component->_local_model = this;
				_scene_object->add_component( _local_light_probe_component );
			}
			else
			{
				assert( _local_light_probe_component );
				_scene_object->remove_component( _local_light_probe_component );
				delete _local_light_probe_component;
				_local_light_probe_component = nullptr;
			}
		}
	}

	boolean_c scene_component_model_c::get_physics_enable() const
	{
		return _physics_enable;
	}

	void_c scene_component_model_c::set_physics_enable( boolean_c value )
	{
		_physics_enable = value;
		//if ( _scene_object && _scene_object->_scene )
		//{
		//	if ( _physics_enable && !_physics_added_to_scene )
		//	{
		//		_add_physics_to_scene();
		//	}
		//	else if ( !_physics_enable && _physics_added_to_scene )
		//	{
		//		_remove_physics_from_scene();
		//	}
		//}
	}

}
