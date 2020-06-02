#include "cheonsa_physics_collision_object.h"
#include "cheonsa_physics_shape.h"
#include "btBulletDynamicsCommon.h"

namespace cheonsa
{

	physics_collision_object_c::physics_collision_object_c()
		: _bullet_collision_object( nullptr )
		, _shape( nullptr )
		, _layer( 0 )
		, _layer_mask( 0 )
		, _reference_count( 0 )
	{
	}

	physics_collision_object_c::~physics_collision_object_c()
	{
		assert( _reference_count == 0 );
		if ( _bullet_collision_object )
		{
			uninitialize();
		}
	}

	physics_collision_object_c * physics_collision_object_c::make_new_instance()
	{
		return new physics_collision_object_c();
	}

	void_c physics_collision_object_c::add_reference()
	{
		_reference_count++;
	}

	void_c physics_collision_object_c::remove_reference()
	{
		assert( _reference_count > 0 );
		_reference_count--;
		if ( _reference_count == 0 )
		{
			delete this;
		}
	}

	void_c physics_collision_object_c::initialize(
		void_c * object,
		physics_shape_c * shape,
		uint32_c layer,
		uint32_c layer_mask )
	{
		assert( _bullet_collision_object == nullptr );
		assert( object );
		assert( shape );
		assert( shape->get_is_initialized() );

		shape->add_reference();
		_shape = shape;
		_layer = layer;
		_layer_mask = layer_mask;

		_bullet_collision_object = new btCollisionObject();
		_bullet_collision_object->setUserPointer( this );
		_bullet_collision_object->setCollisionFlags( btCollisionObject::CF_STATIC_OBJECT );
		_bullet_collision_object->setCollisionShape( shape->_bullet_shape );
	}

	void_c physics_collision_object_c::uninitialize()
	{
		assert( _bullet_collision_object );
		assert( _bullet_collision_object->getWorldArrayIndex() == -1 ); // shouldn't be in a dynamics world at this point.

		delete _bullet_collision_object;
		_bullet_collision_object = nullptr;

		if ( _shape )
		{
			_shape->remove_reference();
			_shape = nullptr;
		}
	}

	boolean_c physics_collision_object_c::get_is_initialized() const
	{
		return _bullet_collision_object;
	}

	void_c physics_collision_object_c::get_world_space_transform( transform3d_c & world_space_transform ) const
	{
		assert( _bullet_collision_object );
		physics_make_space_transform_from_btTransform( _bullet_collision_object->getWorldTransform().getBasis(), _bullet_collision_object->getWorldTransform().getOrigin(), world_space_transform );
		world_space_transform.scale.a = static_cast< float32_c >( _shape->_bullet_shape->getLocalScaling().x() );
		world_space_transform.scale.b = static_cast< float32_c >( _shape->_bullet_shape->getLocalScaling()[ 1 ] );
		world_space_transform.scale.c = static_cast< float32_c >( _shape->_bullet_shape->getLocalScaling().getZ() );
	}

	void_c physics_collision_object_c::set_world_space_transform( transform3d_c const & world_space_transform )
	{
		assert( _bullet_collision_object );
		physics_make_btTransform_from_space_transform( world_space_transform, _bullet_collision_object->getWorldTransform().getBasis(), _bullet_collision_object->getWorldTransform().getOrigin() );
		_shape->_bullet_shape->setLocalScaling( btVector3( world_space_transform.scale.a, world_space_transform.scale.b, world_space_transform.scale.c ) );
	}

	uint32_c physics_collision_object_c::get_layer() const
	{
		return _layer;
	}

	uint32_c physics_collision_object_c::get_layer_mask() const
	{
		return _layer_mask;
	}

	float64_c physics_collision_object_c::get_friction() const
	{
		return _bullet_collision_object->getFriction();
	}

	void_c physics_collision_object_c::set_friction( float64_c value )
	{
		_bullet_collision_object->setFriction( value );
	}

	float64_c physics_collision_object_c::get_restitution() const
	{
		return _bullet_collision_object->getRestitution();
	}

	void_c physics_collision_object_c::set_restitution( float64_c value )
	{
		_bullet_collision_object->setRestitution( value );
	}

}
