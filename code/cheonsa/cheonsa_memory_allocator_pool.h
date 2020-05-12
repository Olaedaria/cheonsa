#pragma once

#include "cheonsa_types.h"

namespace cheonsa
{

	// used to allocate and deallocate fixed chunk_size size pieces of memory from a total_size size pool of memory.
	// so maximum number of items that can be allocated in the pool is limited by total_size / chunk_size.
	// uses a stack to track free allocations.
	// has very good performance for lots of random allocation and deallocation.
	// ideal for glyph storage and particle systems where particles can be created and freed as needed.
	class memory_allocator_pool_c
	{
	private:
		struct free_header_c
		{
		};

		class stack_linked_list_c
		{
		public:
			struct node_c
			{
				free_header_c data;
				node_c * next;
			};

			node_c * head;

		public:
			stack_linked_list_c();
			void_c push( node_c * node );
			node_c * pop();

		};

		stack_linked_list_c _free_list;

		uint8_c * _buffer; // heap allocated buffer of _total_size size in bytes.
		sint32_c _item_size; // size in bytes of each item allocation.
		sint32_c _item_count; // determines size that _buffer is allocated to, and the maximum number of items that can be allocated in this pool.

	public:
		// item_size is size in bytes of each item.
		// item_count is how much memory to reserve on the heap and is the limit of how many items can be allocated by the pool.
		memory_allocator_pool_c( sint32_c item_size, sint32_c item_count );
		~memory_allocator_pool_c();
		
		void_c reset(); // frees everything.

		void_c * allocate(); // returns the next free slot in this allocator.
		void_c free( void_c const * pointer ); // releases a previously allocated free slot in this allocator.

	};

}
