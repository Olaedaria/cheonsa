#include "cheonsa_memory_allocator_pool.h"
#include "cheonsa_ops.h"
#include <assert.h>
#include <cstring> // cstring for memcpy and memset... makes a lot of sense... just kidding, it doesn't, it's stupid.

namespace cheonsa
{

	memory_allocator_pool_c::stack_linked_list_c::stack_linked_list_c()
		: head( nullptr )
	{
	}

	void_c memory_allocator_pool_c::stack_linked_list_c::push( node_c * node )
	{
		node->next = head;
		head = node;
	}

	memory_allocator_pool_c::stack_linked_list_c::node_c * memory_allocator_pool_c::stack_linked_list_c::pop()
	{
		node_c * top = head;
		head = head->next;
		return top;
	}

	memory_allocator_pool_c::memory_allocator_pool_c( sint32_c item_size, sint32_c item_count )
		: _buffer( nullptr )
		, _item_size( 0 )
		, _item_count( 0 )
	{
		assert( item_size >= 16 && "item_size must be greater or equal to 16, since that overhead is used by free slots to track the link to the next free slot." );
		assert( item_count > 0 && "item_count must be greater than 0." );

		// initialize the buffer, and initialize the linked-list with all free positions
		_item_size = item_size;
		_item_count = item_count;
		_buffer = reinterpret_cast< uint8_c * >( ops::memory_aligned_allocate( item_size * item_count, 16 ) );
		ops::memory_zero( _buffer, item_size * item_count );
		reset();
	}

	memory_allocator_pool_c::~memory_allocator_pool_c()
	{
		_free_list.head = nullptr;
		ops::memory_aligned_deallocate( _buffer );
		_buffer = nullptr;
	}

	void_c memory_allocator_pool_c::reset()
	{
		_free_list.head = nullptr;
		for ( sint32_c i = 0; i < _item_count; i++ )
		{
			stack_linked_list_c::node_c * node = reinterpret_cast< stack_linked_list_c::node_c * >( _buffer + ( i * _item_size ) );
			_free_list.push( node );
		}
	}

	void_c * memory_allocator_pool_c::allocate()
	{
		stack_linked_list_c::node_c * free_position = _free_list.pop();
		assert( free_position != nullptr && "pool allocator is full." );
		return reinterpret_cast< void_c * >( free_position );
	}

	void_c memory_allocator_pool_c::free( void_c const * pointer )
	{
		void_c * p = const_cast< void_c * >( pointer );
		assert( p >= _buffer && p < _buffer + ( _item_size * _item_count ) ); // make sure pointer is in range of our pool.
		assert( ( ( reinterpret_cast< uint_native_c >( p ) - reinterpret_cast< uint_native_c >( _buffer ) ) % _item_size ) == 0 ); // make sure pointer is at an even multiple of item size.
		_free_list.push( reinterpret_cast< stack_linked_list_c::node_c * >( p ) );
	}

}
