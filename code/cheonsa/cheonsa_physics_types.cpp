#include "cheonsa_physics_types.h"
#include "cheonsa__ops.h"
#include "btBulletCollisionCommon.h"

namespace cheonsa
{

	void_c physics_make_btTransform_from_space_transform( space_transform_c const & in_transform, btMatrix3x3 & out_basis, btVector3 & out_origin )
	{
		matrix32x3x3_c in_basis = in_transform.get_unscaled_basis();
		out_basis.setValue( in_basis.a.a, in_basis.a.b, in_basis.a.c, in_basis.b.a, in_basis.b.b, in_basis.b.c, in_basis.c.a, in_basis.c.b, in_basis.c.c );
		vector64x3_c const & in_position = in_transform.position;
		out_origin.setValue( in_position.a, in_position.b, in_position.c );
	}

	void_c physics_make_space_transform_from_btTransform( btMatrix3x3 const & in_basis, btVector3 const & in_position, space_transform_c & out_transform )
	{
		matrix32x3x3_c out_basis;
		out_basis.a.a = static_cast< float32_c >( in_basis[ 0 ][ 0 ] );
		out_basis.a.b = static_cast< float32_c >( in_basis[ 0 ][ 1 ] );
		out_basis.a.c = static_cast< float32_c >( in_basis[ 0 ][ 2 ] );
		out_basis.b.a = static_cast< float32_c >( in_basis[ 1 ][ 0 ] );
		out_basis.b.b = static_cast< float32_c >( in_basis[ 1 ][ 1 ] );
		out_basis.b.c = static_cast< float32_c >( in_basis[ 1 ][ 2 ] );
		out_basis.c.a = static_cast< float32_c >( in_basis[ 2 ][ 0 ] );
		out_basis.c.b = static_cast< float32_c >( in_basis[ 2 ][ 1 ] );
		out_basis.c.c = static_cast< float32_c >( in_basis[ 2 ][ 2 ] );
		out_transform.rotation = ops::make_quaternion32_from_matrix32x3x3_basis( out_basis );
		vector64x3_c out_position( in_position[ 0 ], in_position[ 1 ], in_position[ 2 ] );
		out_transform.position = out_position;
		out_transform.scale.a = 1.0f;
		out_transform.scale.b = 1.0f;
		out_transform.scale.c = 1.0f;
	}

}
