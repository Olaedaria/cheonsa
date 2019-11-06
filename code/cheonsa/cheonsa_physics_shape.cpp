#include "cheonsa_physics_shape.h"
#include "btBulletCollisionCommon.h"
#include <cassert>

namespace cheonsa
{

	void_c physics_shape_c::_initialize_compound( vector64x3_c const & local_scaling )
	{
		assert( _shape_type == physics_shape_type_e_none );
		_shape_type = physics_shape_type_e_compound;
		_bullet_shape = new btCompoundShape();
		_bullet_shape->setLocalScaling( btVector3( local_scaling.a, local_scaling.b, local_scaling.c ) );
	}

	void_c physics_shape_c::_add_compound_shape( physics_shape_c * shape )
	{
		assert( _shape_type == physics_shape_type_e_compound );
		assert( shape );
		assert( shape->_bullet_shape );
		btTransform shape_local_space_btTransform;
		physics_make_btTransform_from_space_transform( shape->_local_space_transform, shape_local_space_btTransform.getBasis(), shape_local_space_btTransform.getOrigin() );
		reinterpret_cast< btCompoundShape * >( _bullet_shape )->addChildShape( shape_local_space_btTransform, shape->_bullet_shape );
	}

	void_c physics_shape_c::_remove_compound_shape( physics_shape_c * shape )
	{
		assert( _shape_type == physics_shape_type_e_compound );
		assert( shape );
		assert( shape->_bullet_shape );
		reinterpret_cast< btCompoundShape * >( _bullet_shape )->removeChildShape( shape->_bullet_shape );
	}

	physics_shape_c::physics_shape_c()
		: _shape_type( physics_shape_type_e_none )
		, _bullet_shape( nullptr )
		, _bullet_triangle_vertex_array( nullptr )
	{
	}

	physics_shape_c::~physics_shape_c()
	{
		uninitialize();
	}

	physics_shape_type_e physics_shape_c::get_shape_type() const
	{
		return _shape_type;
	}

	vector64x3_c physics_shape_c::get_local_scaling() const
	{
		btVector3 const & btLocalScaling = _bullet_shape->getLocalScaling();
		return vector64x3_c( btLocalScaling.x(), btLocalScaling.y(), btLocalScaling.z() );
	}

	void_c physics_shape_c::initialize_sphere( space_transform_c const & local_space_transform, float64_c radius, float64_c margin )
	{
		assert( _shape_type == physics_shape_type_e_none );
		_shape_type = physics_shape_type_e_sphere;
		_local_space_transform = local_space_transform;
		_bullet_shape = new btSphereShape( radius );
		_bullet_shape->setMargin( margin );
		_bullet_shape->setLocalScaling( btVector3( _local_space_transform.scale.a, _local_space_transform.scale.b, _local_space_transform.scale.c ) );
	}

	float64_c physics_shape_c::get_sphere_radius()
	{
		assert( _shape_type == physics_shape_type_e_sphere );
		return reinterpret_cast< btSphereShape * >( _bullet_shape )->getRadius();
	}

	void_c physics_shape_c::initialize_box( space_transform_c const & local_space_transform, vector64x3_c const & dimensions, float64_c margin )
	{
		assert( _shape_type == physics_shape_type_e_none );
		_shape_type = physics_shape_type_e_box;
		_local_space_transform = local_space_transform;
		_bullet_shape = new btBoxShape( btVector3( dimensions.a * 0.5, dimensions.b * 0.5, dimensions.c * 0.5 ) );
		_bullet_shape->setMargin( margin );
		_bullet_shape->setLocalScaling( btVector3( _local_space_transform.scale.a, local_space_transform.scale.b, local_space_transform.scale.c ) );
	}


	vector64x3_c physics_shape_c::get_box_half_extents()
	{
		assert( _shape_type == physics_shape_type_e_box );
		btVector3 BulletHalfExtents = reinterpret_cast< btBoxShape * >( _bullet_shape )->getHalfExtentsWithoutMargin();
		return vector64x3_c( BulletHalfExtents.x(), BulletHalfExtents.y(), BulletHalfExtents.z() );
	}

	void_c physics_shape_c::initialize_capsule( space_transform_c const & local_space_transform, float64_c radius, float64_c segment_length, float64_c margin )
	{
		assert( _shape_type == physics_shape_type_e_none );
		_shape_type = physics_shape_type_e_capsule;
		_local_space_transform = local_space_transform;
		_bullet_shape = new btCapsuleShapeZ( radius, segment_length + ( radius * 2.0 ) );
		_bullet_shape->setMargin( margin );
		_bullet_shape->setLocalScaling( btVector3( _local_space_transform.scale.a, _local_space_transform.scale.b, _local_space_transform.scale.c ) );
	}

	float64_c physics_shape_c::get_capsule_radius()
	{
		assert( _shape_type == physics_shape_type_e_capsule );
		return reinterpret_cast< btCapsuleShapeZ * >( _bullet_shape )->getRadius();
	}

	float64_c physics_shape_c::get_capsule_segment_length()
	{
		assert( _shape_type == physics_shape_type_e_capsule );
		return reinterpret_cast< btCapsuleShapeZ * >( _bullet_shape )->getHalfHeight() * 2.0;
	}

	void_c physics_shape_c::initialize_cylinder( space_transform_c const & local_space_transform, float64_c radius, float64_c height, float64_c margin )
	{
		assert( _shape_type == physics_shape_type_e_none );
		_shape_type = physics_shape_type_e_cylinder;
		_local_space_transform = local_space_transform;
		_bullet_shape = new btCylinderShapeZ( btVector3( radius, radius, height * 0.5f ) );
		_bullet_shape->setMargin( margin );
		_bullet_shape->setLocalScaling( btVector3( _local_space_transform.scale.a, _local_space_transform.scale.b, _local_space_transform.scale.c ) );
	}

	float64_c physics_shape_c::get_cylinder_radius()
	{
		assert( _shape_type == physics_shape_type_e_cylinder );
		return reinterpret_cast< btCylinderShapeZ * >( _bullet_shape )->getHalfExtentsWithoutMargin().getX();
	}

	float64_c physics_shape_c::get_cylinder_height()
	{
		assert( _shape_type == physics_shape_type_e_cylinder );
		return reinterpret_cast< btCylinderShapeZ * >( _bullet_shape )->getHalfExtentsWithoutMargin().getY() * 2.0;
	}

	void_c physics_shape_c::initialize_cone( space_transform_c const & local_space_transform, float64_c radius, float64_c height, float64_c margin )
	{
		assert( _shape_type == physics_shape_type_e_none );
		_shape_type = physics_shape_type_e_cone;
		_local_space_transform = local_space_transform;
		_bullet_shape = new btConeShapeZ( radius, height );
		_bullet_shape->setMargin( margin );
		_bullet_shape->setLocalScaling( btVector3( _local_space_transform.scale.a, _local_space_transform.scale.b, _local_space_transform.scale.c ) );
	}

	float64_c physics_shape_c::get_cone_radius()
	{
		assert( _shape_type == physics_shape_type_e_cone );
		return reinterpret_cast< btConeShapeZ * >( _bullet_shape )->getRadius();
	}

	float64_c physics_shape_c::get_cone_height()
	{
		assert( _shape_type == physics_shape_type_e_cone );
		return reinterpret_cast< btConeShapeZ * >( _bullet_shape )->getHeight();
	}

	void_c physics_shape_c::initialize_convex_hull( space_transform_c const & local_space_transform, uint32_c vertex_count, vector64x3_c const * vertex_list, float64_c margin )
	{
		assert( _shape_type == physics_shape_type_e_none );
		_shape_type = physics_shape_type_e_convex_hull;
		_local_space_transform = local_space_transform;
		_bullet_shape = new btConvexHullShape( reinterpret_cast< btScalar const * >( vertex_list ), vertex_count, sizeof( vector64x3_c ) );
		_bullet_shape->setMargin( margin );
		_bullet_shape->setLocalScaling( btVector3( _local_space_transform.scale.a, _local_space_transform.scale.b, _local_space_transform.scale.c ) );
	}

	void_c physics_shape_c::initialize_triangle_mesh( space_transform_c const & local_space_transform, uint32_c vertex_count, vector64x3_c const * vertex_list, uint32_c index_count, uint16_c const * index_list, float64_c margin )
	{
		assert( _shape_type == physics_shape_type_e_none );
		_shape_type = physics_shape_type_e_triangle_mesh;
		_local_space_transform = local_space_transform;
		btIndexedMesh bullet_indexed_mesh;
		bullet_indexed_mesh.m_numTriangles = index_count / 3;
		bullet_indexed_mesh.m_triangleIndexBase = reinterpret_cast< unsigned char const * >( index_list );
		bullet_indexed_mesh.m_triangleIndexStride = 6;
		bullet_indexed_mesh.m_numVertices = vertex_count;
		bullet_indexed_mesh.m_vertexBase = reinterpret_cast< unsigned char const * >( vertex_list );
		bullet_indexed_mesh.m_vertexStride = sizeof( vector64x3_c );
		bullet_indexed_mesh.m_indexType = PHY_SHORT;
		bullet_indexed_mesh.m_vertexType = PHY_FLOAT;
		_bullet_triangle_vertex_array = new btTriangleIndexVertexArray();
		_bullet_triangle_vertex_array->addIndexedMesh( bullet_indexed_mesh, PHY_SHORT );
		_bullet_shape = new btBvhTriangleMeshShape( _bullet_triangle_vertex_array, true );
		_bullet_shape->setMargin( margin );
		_bullet_shape->setLocalScaling( btVector3( _local_space_transform.scale.a, _local_space_transform.scale.b, _local_space_transform.scale.c ) );
		//_bullet_shape = new btScaledBvhTriangleMeshShape( reinterpret_cast< btBvhTriangleMeshShape * >( _bullet_shape ), btVector3( local_scaling.a, local_scaling.b, local_scaling.c ) );
	}

	void_c physics_shape_c::uninitialize()
	{
		if ( _bullet_shape )
		{
			delete _bullet_shape;
			_bullet_shape = nullptr;
		}
		if ( _bullet_triangle_vertex_array )
		{
			delete _bullet_triangle_vertex_array;
			_bullet_triangle_vertex_array = nullptr;
		}
		_shape_type = physics_shape_type_e_none;
	}

}
