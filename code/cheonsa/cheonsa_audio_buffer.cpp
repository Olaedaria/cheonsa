#include "cheonsa_audio_buffer.h"
#include "cheonsa_debug.h"

namespace cheonsa
{

	audio_buffer_c::audio_buffer_c()
		: _reference_count( 1 )
		, _is_loaded( false )
	{
	}

	audio_buffer_c::~audio_buffer_c()
	{
		cheonsa_assert( _reference_count == 0 );
	}

	boolean_c audio_buffer_c::get_is_loaded() const
	{
		return _is_loaded;
	}

	void_c audio_buffer_c::add_reference() const
	{
		const_cast< audio_buffer_c * >( this )->_reference_count++;
	}

	void_c audio_buffer_c::remove_reference() const
	{
		cheonsa_assert( _reference_count > 0 );
		const_cast< audio_buffer_c * >( this )->_reference_count--;
		if ( _reference_count == 0 )
		{
			delete this;
		}
	}

}
