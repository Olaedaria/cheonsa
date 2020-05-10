#include "cheonsa_database_stack.h"
#include "cheonsa_database_stack_record.h"
#include "cheonsa_database.h"
#include "cheonsa_database_table.h"
#include "cheonsa_database_record.h"
#include <cassert>

namespace cheonsa
{

	database_stack_c::database_stack_c()
		: _database_stack()
	{
	}

	void_c database_stack_c::add_database( database_c * database )
	{
		assert( database );
		assert( database->_database_stack == nullptr );
		database->_database_stack = this;
		_database_stack.insert( -1, database );
	}

	boolean_c database_stack_c::get_record_stack( database_record_address_c const & record_address, database_stack_record_c & record_stack )
	{
		record_stack._record_stack.remove_all();
		record_stack._record_schema = nullptr;
		assert( _database_stack.get_length() > 0 );
		boolean_c result = false;
		boolean_c result_is_hidden = false;
		database_table_c const * table = _database_stack[ 0 ]->get_table( record_address.table_id );
		assert( table );
		record_stack._record_schema = &table->get_records_schema();
		for ( sint32_c i = 0; i < _database_stack.get_length(); i++ )
		{
			database_c * database = _database_stack[ i ];
			database_record_c const * record = database->get_record( record_address );
			record_stack._record_stack.insert( -1, record );
			if ( record )
			{
				result = true;
				if ( record->get_is_hidden() )
				{
					result_is_hidden = true;
				}
			}
		}
		return result && !result_is_hidden;
	}

}
