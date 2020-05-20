#include "cheonsa_physics_shape.h"
#include "cheonsa__ops.h"
#include "btBulletCollisionCommon.h"
#include <cassert>

namespace cheonsa
{

	physics_shape_c::physics_shape_c()
		: _type( physics_shape_type_e_none )
		, _bullet_shape( nullptr )
		, _bullet_triangle_vertex_array( nullptr )
		, _frame()
		, _reference_count( 0 )
	{
	}

	physics_shape_c::~physics_shape_c()
	{
		assert( _reference_count == 0 );
		uninitialize();
	}

	physics_shape_c * physics_shape_c::make_new_instance()
	{
		return new physics_shape_c();
	}

	void_c physics_shape_c::add_reference()
	{
		_reference_count++;
	}

	void_c physics_shape_c::remove_reference()
	{
		assert( _reference_count > 0 );
		_reference_count--;
		if ( _reference_count == 0 )
		{
			delete this;
		}
	}

	physics_shape_type_e physics_shape_c::get_type() const
	{
		return _type;
	}

	vector64x3_c physics_shape_c::get_local_scaling() const
	{
		btVector3 const & btLocalScaling = _bullet_shape->getLocalScaling();
		return vector64x3_c( btLocalScaling.x() / _frame.scale.a, btLocalScaling.y() / _frame.scale.b, btLocalScaling.z() / _frame.scale.c );
	}

	void_c physics_shape_c::set_local_scaling( vector64x3_c const & value )
	{
		_bullet_shape->setLocalScaling( btVector3( value.a * _frame.scale.a, value.b * _frame.scale.b, value.c * _frame.scale.c ) );
	}

	void_c physics_shape_c::initialize_compound( core_list_c< physics_shape_c * > const & physics_shape_list )
	{
		assert( _type == physics_shape_type_e_none );
		assert( physics_shape_list.get_length() > 0 );
		_type = physics_shape_type_e_compound;
		_bullet_shape = new btCompoundShape();
		for ( sint32_c i = 0; i < physics_shape_list.get_length(); i++ )
		{
			physics_shape_c * physics_shape = physics_shape_list[ i ];
			_sub_shape_list.insert( -1, physics_shape );
			assert( physics_shape->get_type() >= physics_shape_type_e_box && physics_shape->get_type() <= physics_shape_type_e_triangle_mesh );
			physics_shape->_reference_count++;
			static_cast< btCompoundShape * >( _bullet_shape )->addChildShape( btTransform( btMatrix3x3( btQuaternion( _frame.rotation.a, _frame.rotation.b, _frame.rotation.c, _frame.rotation.d ) ), btVector3( _frame.position.a, _frame.position.b, _frame.position.c ) ), physics_shape->_bullet_shape );
		}
	}

	void_c physics_shape_c::initialize_sphere( transform3d_c const & frame, float64_c radius, float64_c margin )
	{
		assert( _type == physics_shape_type_e_none );
		_type = physics_shape_type_e_sphere;
		_frame = frame;
		_bullet_shape = new btSphereShape( radius );
		_bullet_shape->setMargin( margin );
		_bullet_shape->setLocalScaling( btVector3( _frame.scale.a, _frame.scale.b, _frame.scale.c ) );
	}

	float64_c physics_shape_c::get_sphere_radius() const
	{
		assert( _type == physics_shape_type_e_sphere );
		return reinterpret_cast< btSphereShape * >( _bullet_shape )->getRadius();
	}

	void_c physics_shape_c::initialize_box( transform3d_c const & frame, vector64x3_c const & dimensions, float64_c margin )
	{
		assert( _type == physics_shape_type_e_none );
		_type = physics_shape_type_e_box;
		_frame = frame;
		_bullet_shape = new btBoxShape( btVector3( dimensions.a * 0.5, dimensions.b * 0.5, dimensions.c * 0.5 ) );
		_bullet_shape->setMargin( margin );
		_bullet_shape->setLocalScaling( btVector3( _frame.scale.a, _frame.scale.b, _frame.scale.c ) );
	}


	vector64x3_c physics_shape_c::get_box_half_extents() const
	{
		assert( _type == physics_shape_type_e_box );
		btVector3 BulletHalfExtents = reinterpret_cast< btBoxShape * >( _bullet_shape )->getHalfExtentsWithoutMargin();
		return vector64x3_c( BulletHalfExtents.x(), BulletHalfExtents.y(), BulletHalfExtents.z() );
	}

	void_c physics_shape_c::initialize_capsule( transform3d_c const & frame, float64_c radius, float64_c segment_length, float64_c margin )
	{
		assert( _type == physics_shape_type_e_none );
		_type = physics_shape_type_e_capsule;
		_frame = frame;
		_bullet_shape = new btCapsuleShapeZ( radius, segment_length + ( radius * 2.0 ) );
		_bullet_shape->setMargin( margin );
		_bullet_shape->setLocalScaling( btVector3( _frame.scale.a, _frame.scale.b, _frame.scale.c ) );
	}

	float64_c physics_shape_c::get_capsule_radius() const
	{
		assert( _type == physics_shape_type_e_capsule );
		return reinterpret_cast< btCapsuleShapeZ * >( _bullet_shape )->getRadius();
	}

	float64_c physics_shape_c::get_capsule_segment_length() const
	{
		assert( _type == physics_shape_type_e_capsule );
		return reinterpret_cast< btCapsuleShapeZ * >( _bullet_shape )->getHalfHeight() * 2.0;
	}

	void_c physics_shape_c::initialize_cylinder( transform3d_c const & frame, float64_c radius, float64_c height, float64_c margin )
	{
		assert( _type == physics_shape_type_e_none );
		_type = physics_shape_type_e_cylinder;
		_frame = frame;
		_bullet_shape = new btCylinderShapeZ( btVector3( radius, radius, height * 0.5f ) );
		_bullet_shape->setMargin( margin );
		_bullet_shape->setLocalScaling( btVector3( _frame.scale.a, _frame.scale.b, _frame.scale.c ) );
	}

	float64_c physics_shape_c::get_cylinder_radius() const
	{
		assert( _type == physics_shape_type_e_cylinder );
		return reinterpret_cast< btCylinderShapeZ * >( _bullet_shape )->getHalfExtentsWithoutMargin().getX();
	}

	float64_c physics_shape_c::get_cylinder_height() const
	{
		assert( _type == physics_shape_type_e_cylinder );
		return reinterpret_cast< btCylinderShapeZ * >( _bullet_shape )->getHalfExtentsWithoutMargin().getY() * 2.0;
	}

	void_c physics_shape_c::initialize_cone( transform3d_c const & frame, float64_c radius, float64_c height, float64_c margin )
	{
		assert( _type == physics_shape_type_e_none );
		_type = physics_shape_type_e_cone;
		_frame = frame;
		_bullet_shape = new btConeShapeZ( radius, height );
		_bullet_shape->setMargin( margin );
		_bullet_shape->setLocalScaling( btVector3( _frame.scale.a, _frame.scale.b, _frame.scale.c ) );
	}

	float64_c physics_shape_c::get_cone_radius() const
	{
		assert( _type == physics_shape_type_e_cone );
		return reinterpret_cast< btConeShapeZ * >( _bullet_shape )->getRadius();
	}

	float64_c physics_shape_c::get_cone_height() const
	{
		assert( _type == physics_shape_type_e_cone );
		return reinterpret_cast< btConeShapeZ * >( _bullet_shape )->getHeight();
	}

	void_c physics_shape_c::initialize_convex_hull( transform3d_c const & frame, uint32_c vertex_count, vector64x3_c const * vertex_list, float64_c margin )
	{
		assert( _type == physics_shape_type_e_none );
		_type = physics_shape_type_e_convex_hull;
		_frame = frame;
		_bullet_shape = new btConvexHullShape( reinterpret_cast< btScalar const * >( vertex_list ), vertex_count, sizeof( vector64x3_c ) );
		_bullet_shape->setMargin( margin );
		_bullet_shape->setLocalScaling( btVector3( _frame.scale.a, _frame.scale.b, _frame.scale.c ) );
	}

	void_c physics_shape_c::initialize_triangle_mesh( transform3d_c const & frame, uint32_c vertex_count, vector64x3_c const * vertex_list, uint32_c index_count, uint16_c const * index_list, float64_c margin )
	{
		assert( _type == physics_shape_type_e_none );
		_type = physics_shape_type_e_triangle_mesh;
		_frame = frame;
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
		_bullet_shape->setLocalScaling( btVector3( _frame.scale.a, _frame.scale.b, _frame.scale.c ) );
		//_bullet_shape = new btScaledBvhTriangleMeshShape( reinterpret_cast< btBvhTriangleMeshShape * >( _bullet_shape ), btVector3( local_scaling.a, local_scaling.b, local_scaling.c ) );
	}

	void_c physics_shape_c::uninitialize()
	{
		if ( _type != physics_shape_type_e_none )
		{
			for ( sint32_c i = 0; i < _sub_shape_list.get_length(); i++ )
			{
				_sub_shape_list[ i ]->remove_reference();
			}
			_sub_shape_list.remove_all();
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
			_type = physics_shape_type_e_none;
		}
	}

	boolean_c physics_shape_c::get_is_initialized() const
	{
		return _bullet_shape != nullptr;
	}

	transform3d_c const & physics_shape_c::get_frame() const
	{
		return _frame;
	}

}
