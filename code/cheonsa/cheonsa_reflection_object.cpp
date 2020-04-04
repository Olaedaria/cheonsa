#include "cheonsa_reflection_object.h"
#include "cheonsa_reflection_class.h"
#include <cassert>

namespace cheonsa
{

	reflection_object_c::reflection_object_c( reflection_object_c * this_in, reflection_class_c const * reflection_class )
		: _this( this_in )
		, _reflection_class( reflection_class )
	{
		assert( _reflection_class != nullptr );
	}

	string16_c reflection_object_c::get_reflection_display_name() const
	{
		string16_c result;
		result += _reflection_class->get_name();
		result += L"::get_reflection_display_name() { \"is not implemented\"; }";
		return result;
	}

	reflection_class_c const * reflection_object_c::get_reflection_class() const
	{
		return _reflection_class;
	}

}
