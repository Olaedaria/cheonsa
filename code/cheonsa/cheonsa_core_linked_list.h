#pragma once

#include "cheonsa__types.h"
#include <cassert>

namespace cheonsa
{

	// doubley linked list.
	// value_type_c should always be a pointer type to a class type of object, that holds an encapsulated instance member of core_linked_list_c< value_type_c >::node_c.
	// it takes some planning and it's a little bit pointer-soup.
	template< typename value_type_c >
	class core_linked_list_c
	{
	public:
		struct node_c
		{
		protected:
			friend class core_linked_list_c;
			value_type_c _value;
			node_c * _previous;
			node_c * _next;

		public:
			node_c( value_type_c value )
				: _value( value )
				, _previous( nullptr )
				, _next( nullptr )
			{
			}

			value_type_c get_value() const
			{
				return _value;
			}

			node_c const * get_previous() const
			{
				return _previous;
			}

			node_c * get_previous()
			{
				return _previous;
			}

			node_c const * get_next() const
			{
				return _next;
			}

			node_c * get_next()
			{
				return _next;
			}

		};

	private:
		node_c * _first;
		node_c * _last;
		sint32_c _length;

	public:
		core_linked_list_c()
			: _first( nullptr )
			, _last( nullptr )
			, _length( 0 )
		{
		}

		void_c insert_at_front( node_c * node )
		{
			assert( node );
			assert( node->_previous == nullptr && node->_next == nullptr );
			if ( _first )
			{
				if ( _first != _last )
				{
					node->_next = _first;
					_first->_previous = node;
					_first = node;
				}
				else
				{
					_first = node;
					_first->_next = _last;
					_last->_previous = _first;
				}
			}
			else
			{
				_first = node;
				_last = node;
			}
			_length++;
		}

		void_c insert_at_end( node_c * node )
		{
			assert( node );
			assert( node->_previous == nullptr && node->_next == nullptr );
			if ( _last )
			{
				if ( _last != _first )
				{
					node->_previous = _last;
					_last->_next = node;
					_last = node;
				}
				else
				{
					_last = node;
					_last->_previous = _first;
					_first->_next = _last;
				}
			}
			else
			{
				_first = node;
				_last = node;
			}
			_length++;
		}

		void_c insert_at_index( node_c * node, sint32_c index )
		{
			assert( node );
			assert( node->_previous == nullptr && node->_next == nullptr );
			assert( index >= 0 && index <= _length );
			if ( index == 0 )
			{
				insert_at_front( node );
			}
			else if ( index == _length )
			{
				insert_at_end( node );
			}
			else
			{
				node_c * after_node = _first;
				for ( sint32_c i = 0; i < index - 1; i++ )
				{
					after_node = after_node->get_next();
				}
				insert_after( node, after_node );
			}
		}

		void_c insert_before( node_c * node, node_c * before )
		{
			assert( _first && _last );
			assert( node && before );
			if ( _first != _last )
			{
				node->_next = before;
				node->_previous = before->_previous;
				if ( before->_previous )
				{
					before->_previous->_next = node;
				}
				else
				{
					assert( before == _first );
					_first = node;
				}
				before->_previous = node;
			}
			else
			{
				assert( before == _first );
				_first = node;
				_first->_next = _last;
				_last->_previous = _first;
			}
			_length++;
		}

		void_c insert_after( node_c * node, node_c * after )
		{
			assert( _first && _last );
			assert( node && after );
			assert( node != after );
			if ( _first != _last )
			{
				node->_previous = after;
				node->_next = after->_next;
				if ( after->_next )
				{
					after->_next->_previous = node;
				}
				else
				{
					assert( after == _last );
					_last = node;
				}
				after->_next = node;
			}
			else
			{
				assert( after == _last );
				_last = node;
				_last->_previous = _first;
				_first->_next = _last;
			}
			_length++;
		}

		// empties this linked list in a quick and dirty way, doesn't try to delete values or reset next and previous pointers in nodes.
		// use this if you deleted all of the values already.
		void_c remove_all_quick_and_dirty()
		{
			_first = nullptr;
			_last = nullptr;
			_length = 0;
		}

		// removes a single node from this list.
		void_c remove( node_c * node )
		{
			assert( node );
			if ( _first == node )
			{
				_first = node->_next;
			}
			if ( _last == node )
			{
				_last = node->_previous;
			}
			if ( node->_next )
			{
				node->_next->_previous = node->_previous;
			}
			if ( node->_previous )
			{
				node->_previous->_next = node->_next;
			}
			node->_next = nullptr;
			node->_previous = nullptr;
			_length--;
		}

		void_c remove_at_index( sint32_c index )
		{
			assert( index >= 0 && index < _length );
			node_c * node = _first;
			for ( sint32_c i = 0; i < index; i++ )
			{
				node = node->get_next();
			}
			remove( node );
		}

		node_c * get_at_index( sint32_c index ) const
		{
			assert( index >= 0 && index < _length );
			node_c * node = _first;
			for ( sint32_c i = 0; i < index; i++ )
			{
				node = node->get_next();
			}
			return node;
		}

		node_c * get_first() const
		{
			return _first;
		}

		node_c * get_last() const
		{
			return _last;
		}

		sint32_c get_length() const
		{
			return _length;
		}

	};

}
