#include "cheonsa_scene_component_body.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene.h"
#include "cheonsa__ops.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

namespace cheonsa
{

	void_c scene_component_body_c::_copy_world_space_transform_from_game_to_physics( void_c * instance, vector64x3_c & world_space_position, matrix32x3x3_c & world_space_basis )
	{
		assert( instance );
		scene_component_body_c * resolved_instance = reinterpret_cast< scene_component_body_c * >( instance );
		world_space_position = resolved_instance->get_scene_object()->get_world_space_transform().position;
		world_space_basis = ops::make_matrix32x3x3_basis_from_quaternion32( resolved_instance->get_scene_object()->get_world_space_transform().rotation );
	}

	void_c scene_component_body_c::_copy_world_space_transform_from_physics_to_game( void_c * instance, vector64x3_c const & world_space_position, matrix32x3x3_c const & world_space_basis )
	{
		assert( instance );
		scene_component_body_c * resolved_instance = reinterpret_cast< scene_component_body_c * >( instance );
		assert( resolved_instance->_ignore_property_modified == false );
		space_transform_c world_space_transform;
		world_space_transform.position = world_space_position;
		world_space_transform.rotation = ops::make_quaternion32_from_matrix32x3x3_basis( world_space_basis );
		world_space_transform.scale = resolved_instance->_scene_object->get_world_space_transform().scale; // preserve current scale, physics simulation doesn't change it.
		resolved_instance->_ignore_property_modified = true; // set to true to ignore handling property modified callback upon calling the next function.
		resolved_instance->_scene_object->set_world_space_transform( world_space_transform );
		resolved_instance->_ignore_property_modified = false;
	}

	//void_c scene_component_body_c::_copy_transform_from_physics( void_c * instance, matrix64x3x4_c * value )
	//{
	//	scene_component_body_c * resolved_instance = reinterpret_cast<scene_component_body_c *>( instance );
	//	resolved_instance->_ignore_property_modified = true;
	//	resolved_instance->_scene_object->set_position_rotation_scale( resolved_instance->_physics_body.get_scene_position(), resolved_instance->_physics_body.get_scene_rotation(), resolved_instance->_physics_body.get_scene_scale() );
	//	resolved_instance->_ignore_property_modified = false;
	//	
	//	//if ( resolved_instance->_is_active )
	//	//{
	//	//	// update scene transform of scene object.
	//	//	resolved_instance->_scene_object->_world_transform = *value;
	//	//	resolved_instance->_scene_object->_position = value->get_position();
	//	//	resolved_instance->_scene_object->_rotation = value->get_rotation();
	//	//	resolved_instance->_scene_object->_scale = value->get_scale();
	//	//	// notify other components of the change.
	//	//	for ( sint32_c i = 0; i < resolved_instance->_scene_object->_component_list.get_count(); i++ )
	//	//	{
	//	//		scene_component_c * component = resolved_instance->_scene_object->_component_list[i];
	//	//		if ( component != resolved_instance )
	//	//		{
	//	//			component->_handle_after_scene_transform_modified();
	//	//		}
	//	//	}
	//	//	// propagate scene transform changes down the hierarchy.
	//	//	resolved_instance->_scene_object->update_scene_transform_of_daughters();
	//	//}
	//}

	//void_c scene_component_body_c::_handle_after_added_to_object()
	//{
	//	if ( _physics_body._bullet_rigid_body && !_physics_body.get_kinematic() && _is_active )
	//	{
	//		_scene_object->_scene_transform_is_set_directly_by_some_thing_else++;
	//	}
	//}

	//void_c scene_component_body_c::_handle_before_removed_from_object()
	//{
	//	if ( _physics_body._bullet_rigid_body && !_physics_body.get_kinematic() && _is_active )
	//	{
	//		_scene_object->_scene_transform_is_set_directly_by_some_thing_else--;
	//	}
	//}

	void_c scene_component_body_c::_handle_after_added_to_scene()
	{
		scene_component_c::_handle_after_added_to_scene();
		if ( _is_enabled == true && _is_added_to_phyics_scene == false )
		{
			assert( _physics_shape );
			_scene_object->get_scene()->get_physics_scene()->add_body( &_physics_body );
			_is_added_to_phyics_scene = true;
		}
	}

	void_c scene_component_body_c::_handle_before_removed_from_scene()
	{
		scene_component_c::_handle_before_removed_from_scene();
		if ( _is_added_to_phyics_scene == true )
		{
			_scene_object->get_scene()->get_physics_scene()->remove_body( &_physics_body );
			_is_added_to_phyics_scene = false;
		}
	}

	void_c scene_component_body_c::_handle_after_property_modified( scene_object_property_e property )
	{
		if ( _ignore_property_modified == false ) // we will ignore handling this if we were the ones that originated the event.
		{
			// respond to if scene object scale changed.
			if ( _scene_object && _is_initialized && property == scene_object_property_e_scale && vector64x3_c( _scene_object->get_world_space_scale() ) != _physics_shape->get_local_scaling() )
			{
				reinitialize();
			}

			// the scene object's transform was modified by game logic or the editor or something, we have to update the physics object to match.
			_physics_body.set_world_space_transform( _scene_object->get_world_space_transform() );
		}
	}

	scene_component_body_c::scene_component_body_c( boolean_c is_active )
		: scene_component_c()
		, _physics_body()
		, _physics_shape_list()
		, _is_initialized( false )
		, _is_enabled( false )
		, _is_added_to_phyics_scene( false )
		//, _is_active( is_active )
		, _ignore_property_modified( false )
	{
	}

	scene_component_body_c::~scene_component_body_c()
	{
	}

	boolean_c scene_component_body_c::get_enabled() const
	{
		return _is_enabled;
	}

	void_c scene_component_body_c::set_enabled( boolean_c value )
	{
		if ( _is_enabled != value )
		{
			_is_enabled = value;
			if ( _is_enabled == true && _is_added_to_phyics_scene == false )
			{
				_handle_after_added_to_scene(); // add the _physics_body to the physics scene.
			}
			else if ( _is_enabled == false && _is_added_to_phyics_scene == true )
			{
				_handle_before_removed_from_scene(); // remove the _physics_body from the physics scene.
			}
		}
	}

	sint32_c scene_component_body_c::get_physics_shape_count() const
	{
		return _physics_shape_list.get_length();
	}

	physics_shape_c * scene_component_body_c::get_physics_shape( sint32_c index ) const
	{
		return _physics_shape_list[ index ];
	}

	physics_shape_c * scene_component_body_c::add_physics_shape()
	{
		assert( _is_initialized == false );
		physics_shape_c * physics_shape = new physics_shape_c();
		_physics_shape_list.insert_at_end( physics_shape );
		return physics_shape;
	}

	void_c scene_component_body_c::remove_physics_shape( physics_shape_c * value )
	{
		assert( _is_initialized == false );
		assert( _physics_shape_list.find_index_of( value ) >= 0 );
		_physics_shape_list.remove( value );
		delete value;
	}

	void_c scene_component_body_c::remove_physics_shape( sint32_c index )
	{
		assert( _is_initialized == false );
		physics_shape_c * value = _physics_shape_list[ index ];
		_physics_shape_list.remove_at_index( index );
		delete value;
	}

	void_c scene_component_body_c::remove_all_physics_shapes()
	{
		assert( _is_initialized == false );
		_physics_shape_list.remove_and_delete_all();
	}

	physics_body_c const & scene_component_body_c::get_physics_body() const
	{
		return _physics_body;
	}

	physics_body_c & scene_component_body_c::get_physics_body()
	{
		return _physics_body;
	}

	void_c scene_component_body_c::initialize( float64_c mass, uint32_c layer, uint32_c layer_mask, boolean_c kinematic )
	{
		assert( _scene_object );
		assert( _is_initialized == false );
		assert( _is_added_to_phyics_scene == false );
		assert( _physics_shape_list.get_length() > 0 );

		// initialize _physics_shape.
		_physics_shape = new physics_shape_c();
		_physics_shape->_initialize_compound( vector64x3_c( _scene_object->get_world_space_scale() ) );
		for ( sint32_c i = 0; i < _physics_shape_list.get_length(); i++ )
		{
			_physics_shape->_add_compound_shape( _physics_shape_list[ i ] );
		}

		// initialize the physics body.
		assert( _physics_body._bullet_rigid_body == nullptr );
		_physics_body.initialize( 
			this,
			&scene_component_body_c::_copy_world_space_transform_from_game_to_physics,
			&scene_component_body_c::_copy_world_space_transform_from_physics_to_game,
			_physics_shape,
			mass,
			kinematic,
			layer,
			layer_mask );
		//if ( !kinematic && _is_active && _scene_object )
		//{
		//	_scene_object->_scene_transform_is_set_directly_by_some_thing_else++;
		//}
		if ( _scene_object && _scene_object->get_scene() && _is_enabled )
		{
			_scene_object->get_scene()->get_physics_scene()->add_body( &_physics_body );
			_is_added_to_phyics_scene = true;
		}
	}

	void_c scene_component_body_c::uninitialize()
	{
		assert( _is_initialized == true );

		if ( _is_added_to_phyics_scene )
		{
			_scene_object->get_scene()->get_physics_scene()->remove_body( &_physics_body );
			_is_added_to_phyics_scene = false;
		}
		//if ( !_physics_body.get_kinematic() && _is_active && _scene_object )
		//{
		//	_scene_object->_scene_transform_is_set_directly_by_some_thing_else--;
		//}
		_physics_body.release();
	}

	void_c scene_component_body_c::reinitialize()
	{
		assert( _is_initialized == true );

		float64_c mass = _physics_body.get_mass();
		uint32_c layer = _physics_body.get_layer();
		uint32_c layer_mask = _physics_body.get_layer_mask();
		boolean_c kinematic = _physics_body.get_kinematic();

		uninitialize();
		initialize( mass, layer, layer_mask, kinematic );
	}

}
