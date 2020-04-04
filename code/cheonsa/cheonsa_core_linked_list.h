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
			assert( node != nullptr );
			assert( node->_previous == nullptr && node->_next == nullptr );
			if ( _first != nullptr )
			{
				if ( _first != _last )
				{
					node->next = _first;
					_first->previous = node;
					_first = node;
				}
				else
				{
					_first = node;
					_first->next = _last;
					_last->previous = _first;
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
			assert( node != nullptr );
			assert( node->_previous == nullptr && node->_next == nullptr );
			if ( _last != nullptr )
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

		void insert_before( node_c * node, node_c * before )
		{
			assert( _first != nullptr && _last != nullptr );
			assert( node != nullptr && before != nullptr );
			if ( _first != _last )
			{
				node->_next = before;
				node->_previous = before->_previous;
				if ( before->_previous != nullptr )
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

		void_c insert_after( node_c * value, node_c * after )
		{
			assert( _first != nullptr && _last != nullptr );
			assert( node != nullptr && after != nullptr );
			if ( _first != _last )
			{
				node->_previous = after;
				node->_next = after->_next;
				if ( after->_next != nullptr )
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
			assert( node != nullptr );
			if ( _first == node )
			{
				_first = node->_next;
			}
			if ( _last == node )
			{
				_last = node->_previous;
			}
			if ( node->_next != nullptr )
			{
				node->_next->_previous = node->_previous;
			}
			if ( node->_previous != nullptr )
			{
				node->_previous->_next = node->_next;
			}
			node->_next = nullptr;
			node->_previous = nullptr;
			_length--;
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
