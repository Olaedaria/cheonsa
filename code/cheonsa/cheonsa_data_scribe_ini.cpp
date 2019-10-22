#include "cheonsa_data_scribe_ini.h"
#include "cheonsa_ops.h"
#include "cheonsa_debug.h"

namespace cheonsa
{

	data_scribe_ini_c::data_scribe_ini_c()
		: _document()
		, _item_list()
	{
	}

	void_c data_scribe_ini_c::reset()
	{
		_item_list.remove_all();
		_document = string8_c();
	}

	sint32_c count_spaces_at_start( char8_c * at )
	{
		char8_c * original = at;
		while ( *at != 0 && ( *at == ' ' || *at == '\t' ) )
		{
			at++;
		}
		return static_cast< sint32_c >( at - original );
	}

	sint32_c count_and_zero_spaces_at_end( char8_c * at, char8_c const * at_start )
	{
		char8_c * original = at;
		while ( at >= at_start && ops::char16_is_space( *at ) )
		{
			*at = 0;
			at--;
		}
		return static_cast< sint32_c >( original - at );
	}

	sint32_c count_line_length( char8_c * at )
	{
		char8_c * original = at;
		while ( *at != 0 && *at != '\n' )
		{
			at++;
		}
		return static_cast< sint32_c >( at - original );
	}

	sint32_c count_string_length( char8_c * at )
	{
		char8_c * original = at;
		while ( *at != 0 )
		{
			at++;
		}
		return static_cast< sint32_c >( at - original );
	}

	char8_c * find_first_instance( char8_c * at, char8_c character )
	{
		while ( *at != 0 && *at != character )
		{
			at++;
		}
		return *at == character ? at : nullptr;
	}

	boolean_c data_scribe_ini_c::load( data_stream_c * stream )
	{
		cheonsa_assert( stream->get_position() == 0 );
		sint32_c size = stream->get_size();

		_item_list.remove_all();

		_document.character_list.construct_mode_dynamic( size + 1 );
		_document.character_list[ size ] = 0;
		if ( !stream->load( _document.character_list.get_internal_array(), size ) )
		{
			_document = string8_c();
			return false;
		}

		char8_c * c = _document.character_list.get_internal_array();

		sint32_c line_number = 0;
		while ( sint32_c line_length = count_line_length( c ) > 0 )
		{
			sint32_c spaces_before = count_spaces_at_start( c );
			sint32_c spaces_after = count_and_zero_spaces_at_end( c + line_length, c );
			char8_c * c_next = c + line_length;
			line_length -= spaces_before + spaces_after;

			if ( line_length > 0 )
			{
				c += spaces_before;
				if ( *c == ';' || *c == '#' )
				{
					// current line is a comment, do nothing to ignore it.
				}
				else if ( *c == '[' )
				{
					// current line is a section.
					char8_c * end = find_first_instance( c, ']' );
					if ( end != nullptr )
					{
						end = 0; // zero the ']'.
						item_c * item = _item_list.emplace_at_end();
						item->type = item_c::type_e_section;
						item->name = c + 1;
						item->value = nullptr;
					}
					else
					{
						// error
						_error_line_number = line_number;
						return false;
					}
				}
				else
				{
					// current line is a property.
					char8_c * equal_sign = find_first_instance( c, '=' );
					if ( equal_sign != nullptr && equal_sign > c )
					{
						count_and_zero_spaces_at_end( equal_sign - 1, c );
						equal_sign++;
						spaces_after = count_spaces_at_start( equal_sign );
						equal_sign += spaces_after;

						item_c * item = _item_list.emplace_at_end();
						item->type = item_c::type_e_property;
						item->name = c;
						item->value = equal_sign; // the end of value should be zeroed out during next iteration, when count_and_zero_spaces_at_end() is called.
					}
					else
					{
						_error_line_number = line_number;
						return false;
					}
				}
			}

			line_number++;

			c = c_next;
		}

		return true;
	}

	boolean_c data_scribe_ini_c::find_property_value( string8_c & section_name, string8_c const & property_name, string8_c & result )
	{
		sint32_c phase = 1; // 1 is scan for property, 0 is scan for section.
		for ( sint32_c i = 0; i < _item_list.get_length(); i++ )
		{
			item_c & item = _item_list[ i ];
			if ( phase == 1 )
			{
				if ( item.type == item_c::type_e_property && property_name == item.name )
				{
					result = string8_c( mode_e_static, item.value );
					return true;
				}
				else if ( item.type == item_c::type_e_section )
				{
					phase = 0;
				}
			}
			if ( phase == 0 )
			{
				if ( item.type == item_c::type_e_section && section_name == item.name )
				{
					phase = 1;
				}
			}
		}
		return false;
	}

	void_c data_scribe_ini_c::add_section( string8_c const & section_name )
	{
		cheonsa_assert( section_name.get_length() > 0 );
		if ( _document.get_length() > 0 )
		{
			_document += "\n";
		}
		_document += "[";
		_document += section_name;
		_document += "]\n";
	}

	void_c data_scribe_ini_c::add_property( string8_c const & property_name, string8_c const & property_value )
	{
		_document += property_name;
		_document += "=";
		_document += property_value;
		_document += "\n";
	}

	boolean_c data_scribe_ini_c::save( data_stream_c * stream )
	{
		stream->save( _document.character_list.get_internal_array(), _document.character_list.get_length() - 1 );
		return true;
	}

}
