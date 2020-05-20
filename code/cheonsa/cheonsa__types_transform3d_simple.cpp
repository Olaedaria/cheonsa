#include "cheonsa__types_transform3d_simple.h"
#include "cheonsa__ops_vector.h"

namespace cheonsa
{

	transform3d_simple_c operator * ( transform3d_simple_c const & a, transform3d_simple_c const & b )
	{
		transform3d_simple_c result;
		result.basis = a.basis * b.basis;
		result.position = a.position + ops::rotate_and_scale_vector64x3( b.position, a.basis );
		return result;
	}

}
