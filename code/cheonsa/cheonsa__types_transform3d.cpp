#include "cheonsa__types_transform3d.h"
#include "cheonsa__ops_vector.h"

namespace cheonsa
{

	transform3d_c::transform3d_c()
		: position()
		, rotation()
		, scale( 1.0f, 1.0f, 1.0f )
	{
	}

	transform3d_c::transform3d_c( vector64x3_c const & position, quaternion32_c const & rotation, float32_c uniform_scale )
		: position( position )
		, rotation( rotation )
		, scale( uniform_scale, uniform_scale, uniform_scale )
	{
	}

	transform3d_c::transform3d_c( vector64x3_c const & position, quaternion32_c const & rotation, vector32x3_c const & scale )
		: position( position )
		, rotation( rotation )
		, scale( scale )
	{
	}

	void_c transform3d_c::reset()
	{
		position.a = 0.0;
		position.b = 0.0;
		position.c = 0.0;
		rotation.a = 0.0f;
		rotation.b = 0.0f;
		rotation.c = 0.0f;
		rotation.d = 1.0f;
		scale.a = 1.0f;
		scale.b = 1.0f;
		scale.c = 1.0f;
	}

	matrix32x3x3_c transform3d_c::get_scaled_basis() const
	{
		matrix32x3x3_c result = ops::basis_matrix32x3x3_from_rotation_quaternion32( rotation );
		result.a *= scale.a;
		result.b *= scale.b;
		result.c *= scale.c;
		return result;
	}

	vector32x3_c transform3d_c::get_scaled_basis_x() const
	{
		return ops::basis_vector32x3_x_from_rotation_quaternion32( rotation ) * scale;
	}

	vector32x3_c transform3d_c::get_scaled_basis_y() const
	{
		return ops::basis_vector32x3_y_from_rotation_quaternion32( rotation ) * scale;
	}

	vector32x3_c transform3d_c::get_scaled_basis_z() const
	{
		return ops::basis_vector32x3_z_from_rotation_quaternion32( rotation ) * scale;
	}

	matrix32x3x3_c transform3d_c::get_unscaled_basis() const
	{
		return ops::basis_matrix32x3x3_from_rotation_quaternion32( rotation );
	}

	vector32x3_c transform3d_c::get_unscaled_basis_x() const
	{
		return ops::basis_vector32x3_x_from_rotation_quaternion32( rotation );
	}

	vector32x3_c transform3d_c::get_unscaled_basis_y() const
	{
		return ops::basis_vector32x3_y_from_rotation_quaternion32( rotation );
	}

	vector32x3_c transform3d_c::get_unscaled_basis_z() const
	{
		return ops::basis_vector32x3_z_from_rotation_quaternion32( rotation );
	}

	void_c transform3d_c::set_rotation_from_unscaled_basis( matrix32x3x3_c const & value )
	{
		rotation = ops::rotation_quaternion32_from_basis_matrix32x3x3( value );
	}

	void_c transform3d_c::set_uniform_scale( float32_c value )
	{
		scale.a = value;
		scale.b = value;
		scale.c = value;
	}

	transform3d_c transform3d_c::get_inverted() const
	{
		transform3d_c result;
		result.rotation.a = -rotation.a;
		result.rotation.b = -rotation.b;
		result.rotation.c = -rotation.c;
		result.rotation.d =  rotation.d;
		result.scale.a = 1.0f / scale.a;
		result.scale.b = 1.0f / scale.b;
		result.scale.c = 1.0f / scale.c;
		result.position = ops::rotate_vector64x3( position, result.rotation ) * -result.scale;
		return result;
	}

	void_c transform3d_c::invert()
	{
		rotation.a = -rotation.a;
		rotation.b = -rotation.b;
		rotation.c = -rotation.c;
		scale.a = 1.0f / scale.a;
		scale.b = 1.0f / scale.b;
		scale.c = 1.0f / scale.c;
		position = ops::rotate_vector64x3( position, rotation ) * -scale;
	}

	transform3d_c & transform3d_c::operator = ( transform3d_c const & other )
	{
		position = other.position;
		rotation = other.rotation;
		scale = other.scale;
		return *this;
	}

	transform3d_c & transform3d_c::operator *= ( transform3d_c const & b )
	{
		this->rotation = this->rotation * b.rotation;
		this->scale = this->scale * b.scale;
		this->position = this->position + ops::rotate_vector64x3( b.position, this->rotation ) * this->scale;
		return *this;
	}

	boolean_c operator == ( transform3d_c const & a, transform3d_c const & b )
	{
		return ( a.position == b.position ) && ( a.rotation == b.rotation ) && ( a.scale == b.scale );
	}

	boolean_c operator != ( transform3d_c const & a, transform3d_c const & b )
	{
		return ( a.position != b.position ) || ( a.rotation != b.rotation ) || ( a.scale != b.scale );
	}

	transform3d_c operator * ( transform3d_c a, transform3d_c const & b )
	{
		return a *= b;
	}

}
