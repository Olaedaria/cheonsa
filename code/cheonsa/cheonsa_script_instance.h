#pragma once

#include "cheonsa_script_types.h"
#include "cheonsa_script_scope.h"
#include "cheonsa_script_value.h"

namespace cheonsa
{

	class script_instance_c
	{
	public:
		enum status_e
		{
			status_e_error, // script has encountered an error.
			status_e_okay, // script is executing.
			status_e_wait, // script is executing but it wants to continue from the same line next frame.
			status_e_done // script has finished executing.
		};

		class token_c
		{
		public:
			enum type_e
			{
				type_e_none, // default value.

				type_e_keyword_entry, // entry
				type_e_keyword_finish, // finish
				type_e_keyword_goto, // goto
				type_e_keyword_label, // label

				type_e_keyword_if, // if
				type_e_keyword_else_if, // else if
				type_e_keyword_else, // else

				type_e_value_name, // name of function or variable
				type_e_value_none, // "none" constant
				type_e_value_boolean, // "true" or "false" constant
				type_e_value_number, // number constant
				type_e_value_string, // double quote enclosed string constant

				type_e_operator_times, // *
				type_e_operator_divide, // /
				type_e_operator_modulo, // %
				type_e_operator_plus, // +
				type_e_operator_minus, // -
				type_e_operator_is_equal_to, // ==
				type_e_operator_is_not_equal_to, // !=
				type_e_operator_is_less_than, // <
				type_e_operator_is_less_than_or_equal_to, // <=
				type_e_operator_is_greater_than, // >
				type_e_operator_is_greater_than_or_equal_to, // >=
				type_e_operator_not, // not
				type_e_operator_or, // or
				type_e_operator_and, // and
				type_e_operator_assign, // =
				type_e_operator_remove, // "remove", used to remove a named variable from a scope

				type_e_expression_begin, // (
				type_e_expression_end, // )
				type_e_expression_comma, // ,

				type_e_resolved_value, // resolvedvalue is valid and set
			};

		public:
			uint8_c type; // this token's type.
			string8_c string; // will only be set for type_e_value_* types.
			sint32_c original_line; // this token's line position in the source code.
			sint32_c original_column; // this token's column position in the source code.
			script_value_c resolved_value; // used at script execution, resolved static value of ValueString if any.

			inline token_c()
				: type( type_e_none )
				, string()
				, original_line( 0 )
				, original_column( 0 )
				, resolved_value()
			{
			}

			inline token_c & operator = ( token_c const & other )
			{
				type = other.type;
				string = other.string;
				original_line = other.original_line;
				original_column = other.original_column;
				resolved_value = other.resolved_value;
				return *this;
			}

		};

		class line_c
		{
		public:
			sint32_c indentation;
			token_c * token_array;
			sint32_c token_start;
			sint32_c token_count;
			boolean_c resolved_if_group; // used at script execution, will be marked true for each if/else if/else line within a block once one of the braches was evaluated to true.

			inline line_c()
				: indentation( 0 )
				, token_array( nullptr )
				, token_start( 0 )
				, token_count( 0 )
				, resolved_if_group( false )
			{
			}

			inline line_c & operator = ( line_c const & other )
			{
				indentation = other.indentation;
				token_array = other.token_array;
				token_start = other.token_start;
				token_count = other.token_count;
				resolved_if_group = other.resolved_if_group;
				return *this;
			}

		};

		class label_c
		{
		public:
			string8_c name;
			sint32_c line;

			inline label_c()
				: name()
				, line( 0 )
			{
			}

			inline label_c & operator = ( label_c const & other )
			{
				name = other.name;
				line = other.line;
				return *this;
			}

		};

	public:
		core_list_c< token_c > _token_list;
		core_list_c< line_c > _line_list;
		core_list_c< label_c > _label_list;
		script_scope_c _scope; // local scope for script execution.
		uint8_c _status; // current executing satus.
		sint32_c _line_index; // index of currently executing line.
		sint32_c _indentation; // current expected indentation.
		float64_c _time_step; // update time step in seconds.
		string16_c _error_message; // if an execution error is encountered then a message will be stored here.

		string16_c _file_path; // file path that identifies where this resource was loaded from.
		boolean_c _loaded; // tracks if this resource is loaded or not.

		sint32_c _serial_index; // the index of this resource within the context, so that this resource can be saved/loaded while the game is running.

		sint32_c _master_serial_index;
		void_c * _master; // the scene object that spawned this script.

		script_context_c * _context; // the context that this resource belongs to.

		boolean_c _evaluate_expression( core_list_c< script_value_c > & result, token_c * token_array, sint32_c token_count ); // evaluates and resolves an expression to result. the expression will have access to this executing entry's scopes.
		boolean_c _evaluate_expression( core_list_c< script_value_c > & result, token_c * * token_array, sint32_c token_count ); // evaluates and resolves an expression to result. the expression will have access to this executing entry's scopes.

		boolean_c _resolve_l_r_operands( core_list_c< token_c * > & resolved_token_list, core_list_c< token_c * > & token_list, sint32_c operator_token_index, script_value_c * & l_operand, script_value_c * & r_operand );
		boolean_c _resolve_r_operand( core_list_c< token_c * > & token_list, sint32_c operator_token_index, script_value_c * & r_operand );
		script_scope_c * _resolve_scope( string8_c const & full_name, string8_c & name );
		script_value_c * _get_variable( string8_c const & full_name ); // used for reading values from variables within any scope.
		script_value_c * _add_or_get_variable( string8_c const & full_name ); // used for writing values to variables within any scope.
		void_c _remove_variable( string8_c const & full_name ); // used for deleting variables within any scope.

		void_c _set_error( string16_c const & message, sint32_c const line, sint32_c const column );
		void_c _set_error( string16_c const & message );

	public:
		script_instance_c( script_context_c * Context );
		~script_instance_c();

		boolean_c load( string16_c const & file_path, data_stream_c * file_stream ); // loads this script from a stream. returns true if the load succeeded, returns false if otherwise.

		boolean_c update( float32_c time_step ); // updates execution of this script.

		status_e get_status(); // gets execution status of this script.
		void_c set_status( status_e value ); // sets execution status of this script, for use by custom functions only.

		string16_c const & get_file_path() const; // gets the file path of the script file that this script instance was loaded from.

		string16_c const & get_error_message() const; // if any errors were encountered during load or update then an error message will be found here.

		sint32_c get_serial_index(); // gets the serial index of this script instance so that this script instance can be found again from a suspended execution state.

		sint32_c get_master_serial_index();
		void_c set_master_serial_index( sint32_c value );
		void_c * get_master();
		void_c set_master( void_c * value );

		script_context_c * get_context(); // gets the context that this script belongs to.

		void_c serial_save( data_scribe_binary_property_c & scribe ); // saves this script in its current execution state to a stream.
		void_c serial_load( data_scribe_binary_property_c & scribe ); // loads this script from a suspended execution state from a stream.

	};

}
