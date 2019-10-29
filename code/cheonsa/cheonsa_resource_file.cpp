#include "cheonsa_resource_file.h"
#include "cheonsa_engine.h"
#include "cheonsa_ops.h"

namespace cheonsa
{

	resource_file_c::resource_file_c()
		: _is_loaded( false )
		, _file_path_relative()
		, _file_path_absolute()
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

	string16_c const & resource_file_c::get_file_path_relative() const
	{
		return _file_path_relative;
	}

	string16_c const & resource_file_c::get_file_path_absolute() const
	{
		return _file_path_absolute;
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
