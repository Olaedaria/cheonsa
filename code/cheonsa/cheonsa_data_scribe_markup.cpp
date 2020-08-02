#include "cheonsa_data_scribe_markup.h"
#include "cheonsa_string16.h"
#include "cheonsa__ops.h"
#include <cassert>

namespace cheonsa
{

	boolean_c data_scribe_markup_c::_is_mark_up_character( char16_c character )
	{
		return ( character == _bracket_open ) || ( character == _bracket_close ) || ( character == '/' ) || ( character == '=' ) || ( character == '!' );
	}

	boolean_c data_scribe_markup_c::_is_name_character( char16_c character )
	{
		return !_is_mark_up_character( character );
	}

	boolean_c data_scribe_markup_c::_is_space_character( char16_c character )
	{
		return ( character == ' ' ) || ( character == '\t' ) || ( character == '\r' ) || ( character == '\n' );
	}

	data_scribe_markup_c::attribute_c::attribute_c()
		: _name()
		, _value()
	{
	}

	string8_c const & data_scribe_markup_c::attribute_c::get_name() const
	{
		return _name;
	}

	string8_c const & data_scribe_markup_c::attribute_c::get_value() const
	{
		return _value;
	}

	data_scribe_markup_c::attribute_c & data_scribe_markup_c::attribute_c::operator = ( attribute_c const & )
	{
		assert( false );
		return *this;
	}

	void_c data_scribe_markup_c::attribute_c::move_list_item( attribute_c & from, attribute_c & to )
	{
		from._name.character_list.transfer_to( to._name.character_list );
		from._value.character_list.transfer_to( to._value.character_list );
	}

	data_scribe_markup_c::node_c & data_scribe_markup_c::node_c::operator = ( node_c const & )
	{
		assert( false );
		return *this;
	}

	data_scribe_markup_c::node_c::node_c()
		: _type( type_e_none )
		, _tag_type( tag_type_e_none )
		, _value()
		, _attribute_start( -1 )
		, _attribute_count( 0 )
		, _depth( -1 )
		, _mother( -1 )
		, _first_daughter( -1 )
		, _daughter_count( 0 )
		, _next_sister( -1 )
		, _markup( nullptr )
		, _is_open( false )
	{
	}

	void_c data_scribe_markup_c::node_c::reset( data_scribe_markup_c * markup )
	{
		_type = type_e_none;
		_tag_type = tag_type_e_none;
		_value = string8_c();
		_attribute_start = -1;
		_attribute_count = 0;
		_depth = -1;
		_mother = -1;
		_first_daughter = -1;
		_daughter_count = 0;
		_next_sister = -1;
		_markup = markup;
	}

	data_scribe_markup_c::node_c const * data_scribe_markup_c::node_c::get_node( sint32_c index ) const
	{
		assert( index >= 0 && index < _daughter_count );
		sint32_c current_index = 0;
		sint32_c node_index = _first_daughter;
		while ( current_index <= index )
		{
			node_c * const node = &_markup->_node_heap[ node_index ];
			if ( current_index == index )
			{
				return node;
			}
			current_index++;
			node_index = node->_next_sister;
		}
		return nullptr;
	}

	data_scribe_markup_c::attribute_c const * data_scribe_markup_c::node_c::find_attribute( char8_c const * name ) const
	{
		for ( sint32_c i = 0; i < _attribute_count; i++ )
		{
			attribute_c const * attribute = &_markup->_attribute_heap[ _attribute_start + i ];
			if ( attribute->_name == name )
			{
				return attribute;
			}
		}
		return nullptr;
	}

	sint32_c data_scribe_markup_c::node_c::get_attribute_count() const
	{
		return _attribute_count;
	}

	data_scribe_markup_c::attribute_c const * data_scribe_markup_c::node_c::get_attribute( sint32_c index ) const
	{
		assert( index >= 0 && index < _attribute_count );
		return &_markup->_attribute_heap[ _attribute_start + index ];
	}

	data_scribe_markup_c::node_c const * data_scribe_markup_c::node_c::find_tag( char8_c const * name ) const
	{
		sint32_c node_index = _first_daughter;
		while ( node_index > 0 )
		{
			node_c const * node = &_markup->_node_heap[ node_index ];
			if ( node->_type == node_c::type_e_tag )
			{
				if ( node->_value == name )
				{
					return node;
				}
			}
			node_index = node->_next_sister;
		}
		return nullptr;
	}

	void_c data_scribe_markup_c::node_c::find_tags( char8_c const * name, core_list_c< node_c const * > & result ) const
	{
		result.construct_mode_dynamic( 0 );
		sint32_c node_index = _first_daughter;
		while ( node_index > 0 )
		{
			node_c const * node = &_markup->_node_heap[ node_index ];
			if ( node->_type == node_c::type_e_tag )
			{
				if ( node->_value == name )
				{
					result.insert( -1, node );
				}
			}
			node_index = node->_next_sister;
		}
	}

	data_scribe_markup_c::node_c::type_e data_scribe_markup_c::node_c::get_type() const
	{
		return _type;
	}

	data_scribe_markup_c::node_c::tag_type_e data_scribe_markup_c::node_c::get_tag_type() const
	{
		return _tag_type;
	}

	string8_c const & data_scribe_markup_c::node_c::get_value() const
	{
		return _value;
	}

	sint32_c data_scribe_markup_c::node_c::get_depth() const
	{
		return _depth;
	}

	data_scribe_markup_c::node_c const * data_scribe_markup_c::node_c::get_mother() const
	{
		return _markup->get_node( _mother );
	}

	data_scribe_markup_c::node_c const * data_scribe_markup_c::node_c::get_first_daughter() const
	{
		if ( _first_daughter > 0 )
		{
			return _markup->get_node( _first_daughter );
		}
		return nullptr;
	}

	sint32_c data_scribe_markup_c::node_c::get_daughter_count() const
	{
		return _daughter_count;
	}

	data_scribe_markup_c::node_c const * data_scribe_markup_c::node_c::get_daughter_at_index( sint32_c index ) const
	{
		assert( index < _daughter_count );
		node_c const * result = _markup->get_node( _first_daughter );
		for ( sint32_c i = 0; i <= index; i++ )
		{
			result = _markup->get_node( result->_next_sister );
		}
		return result;
	}

	data_scribe_markup_c::node_c const * data_scribe_markup_c::node_c::get_next_sister() const
	{
		if ( _next_sister > 0 )
		{
			return _markup->get_node( _next_sister );
		}
		return nullptr;
	}

	data_scribe_markup_c const * data_scribe_markup_c::node_c::get_markup() const
	{
		return _markup;
	}

	void_c data_scribe_markup_c::node_c::move_list_item( node_c & from, node_c & to )
	{
		to._type = from._type;
		to._tag_type = from._tag_type;
		from._value.character_list.transfer_to( to._value.character_list );
		to._attribute_start = from._attribute_start;
		to._attribute_count = from._attribute_count;
		to._depth = from._depth;
		to._mother = from._mother;
		to._first_daughter = from._first_daughter;
		to._daughter_count = from._daughter_count;
		to._next_sister = from._next_sister;
		to._markup = from._markup;
		to._is_open = from._is_open;
	}

	void_c data_scribe_markup_c::_advance( sint32_c amount )
	{
		for ( sint32_c i = 0; i < amount; i++ )
		{
			char16_c character = _current[ 0 ];
			if ( character == '\r' )
			{
			}
			else if ( character == '\n' )
			{
				_current_line++;
				_current_character = 0;
				_current_column = 0;
			}
			else if ( character == '\t' )
			{
				_current_character++;
				_current_column += 4 - ( _current_column % 4 );
			}
			else
			{
				_current_character++;
			}
			_current++;
		}
	}

	void_c data_scribe_markup_c::_skip_space()
	{
		while ( _is_space_character( _current[ 0 ] ) )
		{
			_advance( 1 );
		}
	}

	boolean_c data_scribe_markup_c::_parse()
	{
		assert( _node_heap.get_length() == 0 );

		// open virtual root tag.
		node_c * virtual_root_tag = _node_heap.emplace( -1, 1 );
		virtual_root_tag->reset( this );
		virtual_root_tag->_type = node_c::type_e_tag;
		virtual_root_tag->_tag_type = node_c::tag_type_e_open;
		virtual_root_tag->_value = "[virtual_root]";
		virtual_root_tag->_index = 0;
		virtual_root_tag->_depth = 0;
		virtual_root_tag->_is_open = true;

		// parse nodes until end of document.
		core_list_c< sint32_c > node_stack; // tracks our stack state with indices to nodes in the _node_heap.
		node_stack.insert( -1, 0 ); // psuh the virtual root node onto the stack.

		_current = _text_with_mark_up.get_internal_array();

		// check for XML header, it doesn't need to be present, but if it is then we just want to skip past it.
		// we're not programmed to parse it.
		if ( _current[ 0 ] == _bracket_open && _current[ 1 ] == '?' )
		{
			_advance( 2 );
			while ( true )
			{
				if ( _current[ 0 ] == 0 )
				{
					return false;
				}
				else if ( _current[ 0 ] == '?' && _current[ 1 ] == _bracket_close )
				{
					_advance( 2 );
					break;
				}
				else
				{
					_advance( 1 );
				}
			}
		}

		// parse the contents of the document.
		_skip_space();
		while ( _current[ 0 ] != 0 )
		{
			sint32_c new_node_index = _node_heap.get_length();

			node_c * new_node = _node_heap.emplace( -1, 1 );
			new_node->reset( this );
			new_node->_index = _node_heap.get_length() - 1;
			new_node->_depth = node_stack.get_length();

			if ( _current[ 0 ] == _bracket_open )
			{
				if ( _current[ 1 ] == '!' )
				{
					if ( !_parse_comment( new_node ) )
					{
						return false;
					}
				}
				else
				{
					_current[ 0 ] = 0; // in case the previous node was text and needs to be null terminated.
					_advance( 1 ); // skip past '<'.
					if ( !_parse_tag( new_node ) )
					{
						return false;
					}
				}
			}
			else
			{
				if ( !_parse_text( new_node ) )
				{
					return false;
				}
			}

			sint32_c top_node_index = node_stack[ node_stack.get_length() - 1 ];
			node_c * top_node = &_node_heap[ top_node_index ];
			if ( new_node->_tag_type != node_c::tag_type_e_close )
			{
				if ( top_node->_is_open )
				{
					// new_node is daughter of top_node.
					assert( top_node->_first_daughter == -1 );
					top_node->_first_daughter = new_node_index;
					top_node->_daughter_count = 1;
					new_node->_mother = top_node_index;
					node_stack.insert( -1, new_node_index );
					top_node->_is_open = false;
				}
				else
				{
					// new_node is sister of top_node.
					// replace node on top of node stack.
					top_node->_next_sister = new_node_index;
					assert( top_node->_mother >= 0 );
					new_node->_mother = top_node->_mother;
					node_c * node_mother = &_node_heap[ top_node->_mother ];
					node_mother->_daughter_count++;
					node_stack[ node_stack.get_length() - 1 ] = new_node_index;
				}
			}
			else
			{
				assert( new_node->_tag_type == node_c::tag_type_e_close );

				if ( top_node->_is_open )
				{
					// keep current top node, just close it.
					top_node->_is_open = false;
				}
				else
				{
					// pop current top node.
					// we could validate that the name of the closing tag matches the name of the tag that it is closing, but we won't.
					if ( node_stack.get_length() == 1 )
					{
						// encountered too many close tags.
						return false;
					}
					node_stack.remove( -1, 1 );
					_node_heap.remove( -1, 1 ); // recycle, reuse.
				}
			}

			_skip_space();
		}

		return true;
	}

	boolean_c data_scribe_markup_c::_parse_text( node_c * node )
	{
		node->_type = node_c::type_e_text;
		node->_tag_type = node_c::tag_type_e_open_close;

		// skip to end of text.
		char8_c * start = _current;
		while ( _current[ 0 ] != 0 && _current[ 0 ] != _bracket_open )
		{
			_advance( 1 );
		}
		if ( _current == start )
		{
			// zero length text value.
			return true;
		}
		sint32_c text_with_mark_up_start = static_cast< sint32_c >( start - _text_with_mark_up.get_internal_array() );
		sint32_c text_with_mark_up_count = static_cast< sint32_c >( _current - start );

		char8_c * end = _current;
		_process_string( start, end );

		// extract string.
		node->_value.character_list.construct_mode_static_volatile_from_array( start, static_cast< sint32_c >( end - start ) + 1 ); // + 1 for null terminator, which will be set later.
		//out[ 0 ] = 0; // this will be set to null later by _parse_tag() or _parse_comment(), otherwise we risk overwriting a '<' which will throw off the parser when it scans for the next tag.
		return true;
	}

	boolean_c data_scribe_markup_c::_parse_tag( node_c * node )
	{
		node->_type = node_c::type_e_tag;

		// determine if closing tag.
		if ( _current[ 0 ] == '/' )
		{
			node->_tag_type = node_c::tag_type_e_close;
			_advance( 1 );
		}

		// parse tag name.
		if ( !_parse_generic_name( node->_value ) )
		{
			_error_message = "expected element name.";
			return false;
		}

		//if ( node->tag_type == node_c::tag_type_e_close )
		//{
		//	if ( _current[0] == 0 )
		//	{
		//		//_error_message = "expected end of tag, but encountered end of document.";
		//		return false;
		//	}
		//	else if ( _current[0] != '>' )
		//	{
		//		//_error_message = "expected end of tag.";
		//		return false;
		//	}
		//	_text_with_mark_up[node->value_start + node->value_count] = 0;
		//	return true;
		//}

		// parse attributes until end of tag.
		while ( true )
		{
			_skip_space(); // skip past any space that might follow the '<'.

			if ( _current[ 0 ] == 0 )
			{
				// encountered end of document.
				_error_message = "expected end of tag, but encountered end of document.";
				return false;
			}
			else if ( _current[ 0 ] == _bracket_close )
			{
				// encountered end of tag.
				node->_value.character_list[ node->_value.character_list.get_length() - 1 ] = 0; // replace character after name with null terminator.
				_advance( 1 );
				if ( node->_tag_type == node_c::tag_type_e_none )
				{
					node->_tag_type = node_c::tag_type_e_open;
					node->_is_open = true;
				}
				break;
			}
			else if ( _current[ 0 ] == '/' && _current[ 1 ] == _bracket_close )
			{
				// encountered end of self-closing tag.
				node->_value.character_list[ node->_value.character_list.get_length() - 1 ] = 0; // replace character after name with null terminator.
				_advance( 2 );
				if ( node->_tag_type != node_c::tag_type_e_none )
				{
					_error_message = "found '/' at start of tag so expected closing tag, but encountered another '/' at end of tag to indicate self-closing tag.";
					return false;
				}
				node->_tag_type = node_c::tag_type_e_open_close;
				break;
			}

			attribute_c * new_attribute = _attribute_heap.emplace( -1, 1 ); // create new attribute.
			if ( !_parse_generic_name( new_attribute->_name ) ) // parse attribute name.
			{
				_error_message = "expected attribute name.";
				return false;
			}
			_skip_space(); // skip past any space that might be after the name.
			if ( _current[ 0 ] == 0 || _current[ 0 ] != '=' ) // check for end of file or '='.
			{
				_error_message = "expected equal sign so to follow attribute name, but did not find one.";
				return false;
			}
			_advance( 1 ); // skip past '='.
			_skip_space(); // skip past any space that might follow the '='.
			new_attribute->_name.character_list[ new_attribute->_name.character_list.get_length() - 1 ] = 0; // terminate attribute name, this might overwrite the '='.
			if ( _current[ 0 ] == 0 || _current[ 0 ] != '"' ) // check for end of file or missing '"'.
			{
				_error_message = "expected double quote after equal sign to begin attribute value, but did not find one.";
				return false;
			}
			_advance( 1 ); // skip past '"'.
			if ( !_parse_generic_quoted_value( new_attribute->_value ) )
			{
				_error_message = "expected a second double quote to enclose attribute value, but did not find one.";
				return false;
			}
			new_attribute->_value.character_list[ new_attribute->_value.character_list.get_length() - 1 ] = 0; // terminate the attribute value, this will overwrite the '"'.
			if ( node->_attribute_start == -1 )
			{
				node->_attribute_start = _attribute_heap.get_length() - 1;
			}
			node->_attribute_count++;
		}

		return true;
	}

	boolean_c data_scribe_markup_c::_parse_comment( node_c * node )
	{
		node->_type = node_c::type_e_comment;
		node->_tag_type = node_c::tag_type_e_open_close;

		sint32_c comment_depth = 0;
		char8_c const * comment_start = nullptr;
		while ( true )
		{
			sint32_c open_comment = -1;
			sint32_c close_comment = -1;
			if ( _current[ 0 ] == 0 )
			{
				return false; // reached end of document before finding end of comment.
			}
			else if ( _current[ 0 ] == _bracket_open && _current[ 1 ] == '!' && _current[ 2 ] == '-' && _current[ 3 ] == '-' )
			{
				open_comment = 4;
			}
			else if ( _current[ 0 ] == _bracket_open && _current[ 1 ] == '!' )
			{
				open_comment = 2;
			}
			else if ( _current[ 0 ] == '-' && _current[ 1 ] == '-' && _current[ 2 ] == _bracket_close )
			{
				close_comment = 3;
			}
			else if ( _current[ 0 ] == '!' && _current[ 1 ] == _bracket_close )
			{
				close_comment = 2;
			}
			else
			{
				_advance( 1 );
				continue;
			}
			if ( open_comment > 0 )
			{
				if ( comment_depth == 0 )
				{
					_current[ 0 ] = 0; // in case the previous node was text and needs to be null terminated.
					_advance( open_comment );
					comment_start = _current;
				}
				comment_depth++;
			}
			else if ( close_comment > 0 )
			{
				comment_depth--;
				if ( comment_depth == 0 )
				{
					assert( _current - comment_start + 1 < static_cast< sint_native_c >( constants< sint32_c >::maximum() ) );
					node->_value.character_list.construct_mode_static_volatile_from_array( comment_start, static_cast< sint32_c >( _current - comment_start ) );
					_current[ 0 ] = 0; // null terminate value of comment text.
					_advance( close_comment );
					return true;
				}
				_advance( close_comment );
			}
		}

		assert( false ); // it shouldn't be possible to reach here.
		return false;
	}

	boolean_c data_scribe_markup_c::_parse_generic_name( string8_c & out )
	{
		char8_c * start = _current;
		while ( _current[ 0 ] != 0 && !_is_space_character( _current[ 0 ] ) && !_is_mark_up_character( _current[ 0 ] ) )
		{
			_advance( 1 );
		}
		if ( _current == start )
		{
			return false;
		}
		assert( _current - start + 1 < static_cast< sint_native_c >( constants< sint32_c >::maximum() ) );
		out.character_list.construct_mode_static_volatile_from_array( start, static_cast< sint32_c >( _current - start + 1 ) ); // null terminator will be set later.
		return true;
	}

	boolean_c data_scribe_markup_c::_parse_generic_quoted_value( string8_c & out )
	{
		// skip to next double quote.
		char8_c * start = _current;
		while ( _current[ 0 ] != 0 && _current[ 0 ] != '"' )
		{
			_advance( 1 );
		}
		if ( _current[ 0 ] == 0 )
		{
			//_error_message = UnexpectedEndOfFile; // error message will be assigned after returning, by caller.
			return false;
		}
		char8_c * end = _current;
		_process_string( start, end );
		assert( end - start + 1 < static_cast< sint_native_c >( constants< sint32_c >::maximum() ) );
		out.character_list.construct_mode_static_volatile_from_array( start, static_cast< sint32_c >( end - start + 1 ) );
		end[ 0 ] = 0; // replace byte past end of string value with null character terminator.
		_advance( 1 ); // skip closing double quote.
		return true;
	}

	void_c data_scribe_markup_c::_process_string( char8_c * start, char8_c * & end )
	{
		assert( start && end );
		assert( start <= end );

		// process text.
		// replace character entity references with actual characters.
		// replace whitespace sequences to a single space characters.
		// out and in are initially the same, but they may differentiate once symbol entities are translated and spaces are condensed.
		char8_c * out = start;
		char8_c * in = start;

		//// insert leading space if needed, since _skip_space() would have skipped over it in order to detect the next node's type.
		//if ( _current > _text_with_mark_up.get_internal_array() && _is_space_character( _current[ -1 ] ) )
		//{
		//	out[ 0 ] = ' ';
		//	out++;
		//}

		// use translation skip.
		sint_native_c length = end - start;
		for ( sint_native_c i = 0; i < length; i++ )
		{
			// test if replacement is needed.
			if ( in[ 0 ] == '&' )
			{
				switch ( in[ 1 ] )
				{
					case 'a':
						// &amp; &apos;
						if ( in[ 2 ] == 'm' && in[ 3 ] == 'p' && in[ 4 ] == ';' )
						{
							out[ 0 ] = '&';
							out++;
							in += 5;
							continue;
						}
						if ( in[ 2 ] == 'p' && in[ 3 ] == 'o' && in[ 4 ] == 's' && in[ 5 ] == ';' )
						{
							out[ 0 ] = '\'';
							out++;
							in += 6;
							continue;
						}
						break;

					case 'q':
						// &quot;
						if ( in[ 2 ] == 'u' && in[ 3 ] == 'o' && in[ 4 ] == 't' && in[ 5 ] == ';' )
						{
							out[ 0 ] = '"';
							out++;
							in += 6;
							continue;
						}
						break;

					case 'g':
						// &gt;
						if ( in[ 2 ] == 't' && in[ 3 ] == ';' )
						{
							out[ 0 ] = _bracket_close;
							out++;
							in += 4;
							continue;
						}
						break;

					case 'l':
						// &lt;
						if ( in[ 2 ] == 't' && in[ 3 ] == ';' )
						{
							out[ 0 ] = _bracket_open;
							out++;
							in += 4;
							continue;
						}
						break;

					case '#':
						// &#...; - assumes ASCII
						if ( in[ 2 ] == 'x' )
						{
							uint32_c code = 0;
							in += 3; // Skip &#x
							while ( 1 )
							{
								sint8_c digit = ops::convert_char8_to_digit( static_cast< char8_c >( in[ 0 ] ) );
								if ( digit < 0 || digit > 15 )
								{
									break;
								}
								code = code * 16 + digit;
								in++;
							}
							ops::convert_char16_to_utf8( static_cast< char16_c >( code ), out ); // advances out.
						}
						else
						{
							uint32_c code = 0;
							in += 2; // Skip &#
							while ( 1 )
							{
								sint8_c digit = ops::convert_char8_to_digit( static_cast< char8_c >( in[ 0 ] ) );
								if ( digit < 0 || digit > 9 )
								{
									break;
								}
								code = code * 10 + digit;
								in++;
							}
							ops::convert_char16_to_utf8( static_cast< char16_c >( code ), out ); // advances out.
						}
						if ( in[ 0 ] == ';' )
						{
							in++;
						}
						else
						{
							// error or something
							//_error_message = UnrecognizedCharacterEntityReference;
							return;
						}
						continue;

					default:
						// something else
						// ignore, just copy '&' verbatim
						break;

				}
			}
			else if ( in[ 0 ] == ' ' || in[ 0 ] == '\t' )
			{
				// condense spaces if needed.
				// put single space in out.
				out[ 0 ] = ' ';
				out++;
				in++;
				// skip remaining space characters.
				while ( in[ 0 ] == ' ' || in[ 0 ] == '\t' )
				{
					in++;
				}
				continue;
			}
			else if ( in[ 0 ] == '\n' || in[ 0 ] == '\r' )
			{
				in++;
				continue;
			}
			// no translation needed, copy the character as it is.
			out[ 0 ] = in[ 0 ];
			out++;
			in++;
		}

		// remove trailing space.
		if ( out > start )
		{
			if ( out[ -1 ] == ' ' )
			{
				out--;
			}
		}

		// result.
		end = out;
	}

	data_scribe_markup_c::data_scribe_markup_c( char8_c bracket_type )
		: _bracket_open( 0 )
		, _bracket_close( 0 )
		, _text_with_mark_up()
		, _current( nullptr )
		, _current_line( 0 )
		, _current_column( 0 )
		, _current_character( 0 )
		, _tab_size( 4 )
		, _node_heap()
		, _attribute_heap()
	{
		assert( bracket_type == '<' || bracket_type == '[' );

		_node_heap.set_list_item_move_function( &node_c::move_list_item );
		_attribute_heap.set_list_item_move_function( &attribute_c::move_list_item );

		if ( bracket_type == '<' )
		{
			_bracket_open = '<';
			_bracket_close = '>';
		}
		else if ( bracket_type == '[' )
		{
			_bracket_open = '[';
			_bracket_close = ']';
		}
	}

	data_scribe_markup_c::~data_scribe_markup_c()
	{
	}

	void_c data_scribe_markup_c::reset()
	{
		_text_with_mark_up.remove_all();
		_current = nullptr;
		_current_line = 0;
		_current_column = 0;
		_current_character = 0;
		_tab_size = 4;
		_node_heap.remove_all();
		_attribute_heap.remove_all();
	}

	boolean_c data_scribe_markup_c::parse( string8_c const & document_string )
	{
		reset();
		_text_with_mark_up.insert( -1, document_string.character_list.get_internal_array(), document_string.character_list.get_length() );
		// add a bunch of terminating zeros so that the parser has room to scan ahead safely without worrying about checking if it will go out of bounds.
		char8_c values[] = { '\0', '\0', '\0', '\0', '\0' };
		_text_with_mark_up.insert( -1, values, 5 );
		return _parse();
	}

	boolean_c data_scribe_markup_c::parse( data_stream_c * document_stream )
	{
		assert( document_stream );
		assert( document_stream->get_position() == 0 );
		reset();
		sint32_c size = document_stream->get_size();
		sint32_c padding = 5; // allocate some padding so that the parser can scan ahead without needing to check if it will go out of bounds.
		_text_with_mark_up.construct_mode_dynamic( size + padding, size + padding );
		document_stream->load( _text_with_mark_up.get_internal_array(), size );
		for ( sint32_c i = size; i < size + padding; i++ ) // zero the padding.
		{
			_text_with_mark_up[ i ] = 0;
		}
		return _parse();
	}

	data_scribe_markup_c::node_c const * data_scribe_markup_c::get_node( sint32_c index ) const
	{
		if ( index >= 0 && index < _node_heap.get_length() )
		{
			return &_node_heap[ index ];
		}
		return nullptr;
	}

	string16_c data_scribe_markup_c::get_error_message()
	{
		return _error_message;
	}

	string16_c const & data_scribe_markup_c::get_error_message( sint32_c & line, sint32_c & column, sint32_c & character ) const
	{
		line = _current_line;
		column = _current_column;
		character = _current_character;
		return _error_message;
	}

	boolean_c data_scribe_markup_c::parse_first_tag( data_stream_c * stream )
	{
		reset();

		// read at most 64 bytes at a time from the stream until we find a tag.
		// 0 is scanning for open bracket.
		// 1 is scanning for close bracket.
		// 2 is done.
		sint32_c iteration = 0;
		sint32_c state = 0;
		sint32_c stream_size = stream->get_size();
		sint32_c line_start = -1;
		sint32_c line_end = -1;
		while ( state < 2 )
		{
			sint32_c bytes_to_read = stream_size - stream->get_position();
			if ( bytes_to_read > 64 )
			{
				bytes_to_read = 64;
			}
			else if ( bytes_to_read == 0 )
			{
				return false;
			}
			sint32_c insert_at = _text_with_mark_up.get_length();
			_text_with_mark_up.set_length( _text_with_mark_up.get_length() + bytes_to_read );
			stream->load( &_text_with_mark_up.get_internal_array()[ insert_at ], bytes_to_read );

			// scan the bytes that were read.
			sint32_c end = insert_at + bytes_to_read;
			sint32_c i = insert_at;
			if ( state == 0 )
			{
				for ( ; i < end; i++ )
				{
					if ( _text_with_mark_up[ i ] == _bracket_open )
					{
						line_start = i;
						state = 1;
						break;
					}
				}
			}
			if ( state == 1 )
			{
				for ( ; i < end; i++ )
				{
					if ( _text_with_mark_up[ i ] == _bracket_close )
					{
						line_end = i + 1;
						state = 2;
						break;
					}
				}
			}

			if ( iteration == 0 && state == 0 )
			{
				// no tag found in the first bytes, we'll assume that this file is not relevant or correct format, give up early.
				return false;
			}

			iteration++;
		}

		assert( line_start != -1 && line_end != -1 );

		_current = &_text_with_mark_up.get_internal_array()[ line_start ];

		assert( _current[ 0 ] == _bracket_open );

		node_c * new_node = _node_heap.emplace( -1, 1 );
		new_node->reset( this );

		_current[ 0 ] = 0; // we don't need to do this here but it's not a big deal.
		_advance( 1 ); // skip past '<'.

		return _parse_tag( new_node );
	}

}
