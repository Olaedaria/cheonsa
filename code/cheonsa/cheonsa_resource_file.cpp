#include "cheonsa_resource_file.h"
#include "cheonsa_engine.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	resource_file_c::resource_file_c()
		: _is_loaded( false )
		, _relative_file_path()
		, _absolute_file_path()
		, _file_modified_time( 0 )
		, _reference_count( 0 )
	{
	}

	resource_file_c::~resource_file_c()
	{
	}

	boolean_c resource_file_c::get_is_loaded() const
	{
		return _is_loaded;
	}

	string16_c const & resource_file_c::get_relative_file_path() const
	{
		return _relative_file_path;
	}

	string16_c const & resource_file_c::get_absolute_file_path() const
	{
		return _absolute_file_path;
	}

	sint64_c resource_file_c::get_file_modified_time() const
	{
		return _file_modified_time;
	}

	sint32_c resource_file_c::get_reference_count() const
	{
		return _reference_count;
	}

}
