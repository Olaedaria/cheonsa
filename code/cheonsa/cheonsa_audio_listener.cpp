#include "cheonsa_audio_listener.h"
#include "cheonsa_ops.h"

namespace cheonsa
{

	audio_listener_c::audio_listener_c()
		: _world_space_transform( nullptr )
		//, _world_space_position()
		//, _world_space_forward( 0.0f, 1.0f, 0.0f )
		//, _world_space_up( 0.0f, 0.0f, 1.0f )
		, _damping( 0.71828f )
		, _volume( 1.0f )
	{
	}

	//vector64x3_c const & audio_listener_c::get_world_space_position() const
	//{
	//	return _world_space_position;
	//}

	//void_c audio_listener_c::set_world_space_position( vector64x3_c const & value )
	//{
	//	_world_space_position = value;
	//}

	//vector32x3_c const & audio_listener_c::get_world_space_forward() const
	//{
	//	return _world_space_forward;
	//}

	//void_c audio_listener_c::set_world_space_forward( vector32x3_c const & value )
	//{
	//	_world_space_forward = value;
	//}

	//vector32x3_c const & audio_listener_c::get_world_space_up() const
	//{
	//	return _world_space_up;
	//}

	//void_c audio_listener_c::set_world_space_up( vector32x3_c const & value )
	//{
	//	_world_space_up = value;
	//}

	float32_c audio_listener_c::get_damping() const
	{
		return _damping;
	}

	void_c audio_listener_c::set_damping( float32_c value )
	{
		_damping = ops::math_clamp( value, 0.0f, 1000.0f );
	}

	float32_c audio_listener_c::get_volume() const
	{
		return _volume;
	}

	void_c audio_listener_c::set_volume( float32_c value )
	{
		_volume = ops::math_clamp( value, 0.0f, 10.0f );
	}

	void_c audio_listener_c::set_world_space_transform( space_transform_c const * value )
	{
		_world_space_transform = value;
	}

}
