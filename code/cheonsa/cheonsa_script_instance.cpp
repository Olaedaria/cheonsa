#include "cheonsa_script_instance.h"
#include "cheonsa_script_context.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	class script_line_scanner_c // extracts lines from source code.
	{
	public:
		string8_c const & source;
		sint32_c index;
		sint32_c line;

		script_line_scanner_c( string8_c const & source_in )
			: source( source_in )
			, index( 0 )
			, line( 0 )
		{
		}

		boolean_c get_next_line( string8_c & line_out, sint32_c & line_indentation_out )
		{
			sint32_c length = source.get_length();

			if ( index >= length )
			{
				return false;
			}

			sint32_c start = index; // index of first character in line.
			sint32_c end = start; // index of last character in line.
			sint32_c end_bias = 0; // used to exclude \r and \n characters when converting the line to a string.

			sint32_c line_indentation = 0;
			boolean_c find_line_indentation = true;
			while ( end < length )
			{
				char8_c character = source.character_list[ end ];
				end++;

				if ( character == '\r' )
				{
					end_bias = 1;
					if ( end < length )
					{
						character = source.character_list[ end ];
						if ( character == '\n' )
						{
							end_bias = 2;
							end++;
						}
					}
					break;
				}
				else if ( character == '\n' )
				{
					end_bias = 1;
					break;
				}

				if ( find_line_indentation )
				{
					if ( character == ' ' )
					{
						line_indentation += 1;
					}
					else if ( character == '\t' )
					{
						line_indentation += 4 - ( line_indentation % 4 ); // tabs stick to columns that measure multiples of 4
					}
					else
					{
						find_line_indentation = false;
					}
				}
			}

			line_indentation_out = line_indentation;
			line_out = ops::string8_sub_string( source, start, end - end_bias - start );
			index = end;

			return true;
		}

	};

	class script_token_scanner_c // extracts tokens from lines.
	{
	public:
		string8_c source;
		sint32_c index;
		sint32_c column;
		string16_c error_message;

		script_token_scanner_c()
			: source()
			, index( 0 )
			, column( 0 )
			, error_message()
		{
		}

		boolean_c skip_white_space()
		{
			char16_c character = 0;
			sint32_c length = source.get_length();
			for ( ; index < length; index++ )
			{
				character = source.character_list[ index ];
				if ( character == ' ' )
				{
					column += 1;
				}
				else if ( character == '\t' )
				{
					column += 4 - ( column % 4 );
				}
				else
				{
					break;
				}
			}
			return true;
		}

		// removes quotes from input string and resolves inline escaped characters to their actual character values.
		static void_c format_string( string8_c & in, string8_c & out )
		{
			out = L"";
			char16_c last_character = 0;
			sint32_c end = in.get_length() - 1;
			for ( sint32_c i = 1; i < end; i++ )
			{
				char16_c character = in.character_list[ i ];
				if ( last_character == '\\' )
				{
					if ( character == 'n' )
					{
						out += '\n';
					}
					else if ( character == 't' )
					{
						out += '\t';
					}
					else if ( character == '"' )
					{
						out += '"';
					}
					else if ( character == '\\' )
					{
						out += '\\';
					}
				}
				else
				{
					out += character;
				}
				last_character = character;
			}
			out += '\0';
		}

		script_instance_c::status_e get_next_token( string8_c & value_out, script_instance_c::token_c::type_e & type_out )
		{
			sint32_c length = source.get_length();
			if ( index >= length )
			{
				return script_instance_c::status_e_done;
			}

			type_out = script_instance_c::token_c::type_e_none;

			sint32_c start = index;
			sint32_c end = start;

			char16_c character = source.character_list[ end ];
			if ( character == '#' )
			{
				return script_instance_c::status_e_done;
			}
			else if ( ( character == '_' ) || ( character >= 'a' && character <= 'z' ) || ( character >= 'a' && character <= 'Z' ) )
			{
				do
				{
					end++;
					column++;
					if ( end >= length )
					{
						break;
					}
					character = source.character_list[ end ];
				} while ( ( character == '_' ) || ( character >= 'a' && character <= 'z' ) || ( character >= 'a' && character <= 'Z' ) || ( character >= '0' && character <= '9' ) || ( character == '.' ) );
				type_out = script_instance_c::token_c::type_e_value_name;
				value_out = ops::string8_sub_string( source, start, end - start );
				if ( value_out == "entry" )
				{
					type_out = script_instance_c::token_c::type_e_keyword_entry;
				}
				else if ( value_out == "finish" )
				{
					type_out = script_instance_c::token_c::type_e_keyword_finish;
				}
				else if ( value_out == "goto" )
				{
					type_out = script_instance_c::token_c::type_e_keyword_goto;
				}
				else if ( value_out == "label" )
				{
					type_out = script_instance_c::token_c::type_e_keyword_label;
				}
				else if ( value_out == "if" )
				{
					type_out = script_instance_c::token_c::type_e_keyword_if;
				}
				else if ( value_out == "else" )
				{
					type_out = script_instance_c::token_c::type_e_keyword_else;
				}
				else if ( value_out == "true" || value_out == "false" )
				{
					type_out = script_instance_c::token_c::type_e_value_boolean;
				}
				else if ( value_out == "remove" )
				{
					type_out = script_instance_c::token_c::type_e_operator_remove;
				}
			}
			else if ( ( character >= '0' && character <= '9' ) || character == '-' )
			{
				if ( character == '-' && end + 1 < source.get_length() ) // if we encountered a minus sign then check if the next character is a digit
				{
					char16_c next_character = source.character_list[ end + 1 ];
					if ( next_character >= '0' && next_character <= '9' ) // the next character is a digit so we can continue parsing this number as normal
					{
					}
					else // the next character is not a digit so this minus sign is just a minus sign
					{
						value_out = "-";
						type_out = script_instance_c::token_c::type_e_operator_minus;
					}
				}

				if ( type_out != script_instance_c::token_c::type_e_operator_minus )
				{
					boolean_c has_decimal_point = false;
					do
					{
						if ( character == '.' )
						{
							if ( has_decimal_point )
							{
								error_message = "Encountered more than one decimal point in a number.";
								return script_instance_c::status_e_error;
							}
							has_decimal_point = true;
						}
						end++;
						column++;
						if ( end >= length )
						{
							break;
						}
						character = source.character_list[ end ];
					} while ( ( character >= '0' && character <= '9' ) || ( character == '.' ) );
					type_out = script_instance_c::token_c::type_e_value_number;
				}
			}
			else if ( character == '"' )
			{
				boolean_c has_escape = false;
				do
				{
					has_escape = character == '\\';
					end++;
					column++;
					if ( end >= length )
					{
						error_message = "Encountered end of line before end of string.";
						return script_instance_c::status_e_error;
					}
					character = source.character_list[ end ];
				} while ( ( character != '"' ) || ( character == '"' && has_escape ) );
				end++;
				column++;
				type_out = script_instance_c::token_c::type_e_value_string;
				string8_c value_out_raw = ops::string8_sub_string( source, start, end - start );
				format_string( value_out_raw, value_out );
			}
			else if ( character == '(' || character == ',' || character == ')' || character == '+' || character == '-' || character == '*' || character == '/' || character == '%' || character == '=' || character == '!' || character == '<' || character == '>' || character == ':' )
			{
				end++;
				column++;
				if ( end < length )
				{
					if ( character == '=' || character == '!' || character == '<' || character == '>' )
					{
						character = source.character_list[ end ];
						if ( character == '=' )
						{
							end++;
							column++;
						}
					}
				}
				value_out = ops::string8_sub_string( source, start, end - start );
				if ( value_out == "=" )
				{
					type_out = script_instance_c::token_c::type_e_operator_assign;
				}
				else if ( value_out == "+" )
				{
					type_out = script_instance_c::token_c::type_e_operator_plus;
				}
				else if ( value_out == "-" )
				{
					type_out = script_instance_c::token_c::type_e_operator_minus;
				}
				else if ( value_out == "*" )
				{
					type_out = script_instance_c::token_c::type_e_operator_times;
				}
				else if ( value_out == "/" )
				{
					type_out = script_instance_c::token_c::type_e_operator_divide;
				}
				else if ( value_out == "%" )
				{
					type_out = script_instance_c::token_c::type_e_operator_modulo;
				}
				else if ( value_out == "==" )
				{
					type_out = script_instance_c::token_c::type_e_operator_is_equal_to;
				}
				else if ( value_out == "!=" )
				{
					type_out = script_instance_c::token_c::type_e_operator_is_not_equal_to;
				}
				else if ( value_out == "<" )
				{
					type_out = script_instance_c::token_c::type_e_operator_is_less_than;
				}
				else if ( value_out == "<=" )
				{
					type_out = script_instance_c::token_c::type_e_operator_is_less_than_or_equal_to;
				}
				else if ( value_out == ">" )
				{
					type_out = script_instance_c::token_c::type_e_operator_is_greater_than;
				}
				else if ( value_out == ">=" )
				{
					type_out = script_instance_c::token_c::type_e_operator_is_greater_than_or_equal_to;
				}
				else if ( value_out == "!" )
				{
					type_out = script_instance_c::token_c::type_e_operator_not;
				}
				else if ( value_out == "||" )
				{
					type_out = script_instance_c::token_c::type_e_operator_or;
				}
				else if ( value_out == "&&" )
				{
					type_out = script_instance_c::token_c::type_e_operator_and;
				}
				else if ( value_out == "(" )
				{
					type_out = script_instance_c::token_c::type_e_expression_begin;
				}
				else if ( value_out == ")" )
				{
					type_out = script_instance_c::token_c::type_e_expression_end;
				}
				else if ( value_out == "," )
				{
					type_out = script_instance_c::token_c::type_e_expression_comma;
				}
			}

			if ( type_out == script_instance_c::token_c::type_e_none )
			{
				error_message = "Encountered unrecognized token.";
				return script_instance_c::status_e_error;
			}

			index = end;
			return script_instance_c::status_e_wait;
		}

	};

	void_c script_reset_if_block_resolution( script_instance_c::line_c * line_array, sint32_c start, sint32_c end, boolean_c resolved )
	{
		sint32_c indentation = line_array[start].indentation;
		for ( sint32_c i = start; i < end; i++ )
		{
			script_instance_c::line_c & line = line_array[i];
			if ( line.indentation == indentation )
			{
				script_instance_c::token_c & token = line.token_array[0];
				if ( token.type == script_instance_c::token_c::type_e_keyword_else_if || token.type == script_instance_c::token_c::type_e_keyword_else || token.type == script_instance_c::token_c::type_e_keyword_if )
				{
					line.resolved_if_group = resolved;
				}
				else
				{
					break;
				}
			}
		}
	}

	boolean_c script_instance_c::_evaluate_expression( core_list_c< script_value_c > & result, token_c * token_array, sint32_c token_count )
	{
		token_c * CopyTokenArray = new token_c[token_count];
		token_c * * CopyTokenPointerArray = new token_c *[token_count];
		for ( sint32_c i = 0; i < token_count; i++ )
		{
			CopyTokenArray[i] = token_array[i];
			CopyTokenPointerArray[i] = &CopyTokenArray[i];
		}
		boolean_c StatusResult = _evaluate_expression( result, CopyTokenPointerArray, token_count );
		delete[] CopyTokenPointerArray;
		delete[] CopyTokenArray;
		return StatusResult;
	}

	boolean_c script_instance_c::_evaluate_expression( core_list_c< script_value_c > & result, token_c * * token_array, sint32_c token_count )
	{
		// the following token types are illegal within expressions
		// type_e_none
		// type_e_keyword_entry
		// type_e_keyword_finish
		// type_e_keyword_goto
		// type_e_keyword_label
		// type_e_keyword_if
		// type_e_keyword_else_if
		// type_e_keyword_else

		core_list_c< token_c * > token_list_a;
		token_list_a.insert_at_end( token_array, token_count );

		// if there is only one token in the input and if it is a resolved value, then it is our final result and we are done
		if ( token_list_a.get_length() == 1 )
		{
			token_c * token = token_list_a[ 0 ];
			if ( token->type == script_instance_c::token_c::type_e_resolved_value )
			{
				result.insert_at_end( token->resolved_value );
				return true;
			}
		}

		core_list_c< token_c * > token_list_b;
		token_list_b.construct_mode_dynamic( token_count, token_count );

		script_value_c * l_operand = nullptr;
		script_value_c * r_operand = nullptr;

		// 0, resolve constant values
		for ( sint32_c i = 0; i < token_count; i++ )
		{
			token_c * token = token_list_a[ i ];
			if ( token->type == script_instance_c::token_c::type_e_value_none )
			{
				token->type = script_instance_c::token_c::type_e_resolved_value;
				token->resolved_value.set_none_value();
			}
			else if ( token->type == script_instance_c::token_c::type_e_value_boolean )
			{
				token->type = script_instance_c::token_c::type_e_resolved_value;
				token->resolved_value.set_number_value( token->string == "true" ? 1.0f : 0.0f );
			}
			else if ( token->type == script_instance_c::token_c::type_e_value_number )
			{
				token->type = script_instance_c::token_c::type_e_resolved_value;
				float32_c float_value = 0.0f;
				ops::convert_string8_to_float32( token->string, float_value );
				token->resolved_value.set_number_value( float_value );
			}
			else if ( token->type == script_instance_c::token_c::type_e_value_string )
			{
				token->type = script_instance_c::token_c::type_e_resolved_value;
				token->resolved_value.set_string_value( token->string );
			}
		}

		// 1, resolve sub expressions and function calls
		boolean_c last_token_was_name = false;
		for ( sint32_c i = 0; i < token_list_a.get_length(); i++ )
		{
			token_c * token = token_list_a[ i ];
			if ( token->type == script_instance_c::token_c::type_e_expression_begin )
			{
				core_list_c< script_value_c > sub_expression_result;
				sint32_c level = 1;
				i++;
				sint32_c sub_expression_start = i;
				for ( ; i < token_list_a.get_length(); i++ )
				{
					if ( token_list_a[ i ]->type == script_instance_c::token_c::type_e_expression_begin )
					{
						level++;
					}
					else if ( token_list_a[ i ]->type == script_instance_c::token_c::type_e_expression_end )
					{
						level--;
						if ( level == 0 ) // we found the end of the sub expression, evaluate it
						{
							sint32_c sub_expression_end = i;
							if ( sub_expression_end > sub_expression_start )
							{
								core_list_c< script_value_c > sub_expression_result_2;
								_evaluate_expression( sub_expression_result_2, &token_array[ sub_expression_start ], sub_expression_end - sub_expression_start );
								for ( sint32_c j = 0; j < sub_expression_result_2.get_length(); j++ )
								{
									sub_expression_result.insert_at_end( sub_expression_result_2[ j ] );
								}
							}
						}
					}
					else if ( token_list_a[ i ]->type == script_instance_c::token_c::type_e_expression_comma )
					{
						if ( level == 1 ) // we found a coma in the sub expression, evaluate it
						{
							sint32_c sub_expression_end = i;
							if ( sub_expression_end > sub_expression_start )
							{
								core_list_c< script_value_c > sub_expression_result_2;
								_evaluate_expression( sub_expression_result_2, &token_array[ sub_expression_start ], sub_expression_end - sub_expression_start );
								for ( sint32_c j = 0; j < sub_expression_result_2.get_length(); j++ )
								{
									sub_expression_result.insert_at_end( sub_expression_result_2[ j ] );
								}
								sub_expression_start = sub_expression_end + 1;
							}
						}
					}
				}
				if ( last_token_was_name ) // making a function call
				{
					token_c * last_token = token_list_b[ token_list_b.get_length() - 1 ];
					script_value_c function_result;
					if ( !engine_c::get_instance()->get_script_manager()->call_function( this, last_token->string, sub_expression_result, function_result ) )
					{
						return false;
					}
					if ( _status == status_e_error )
					{
						return false;
					}
					else if ( _status == status_e_wait )
					{
						return true;
					}
					else if ( _status == status_e_done )
					{
						return true;
					}
					last_token->type = token_c::type_e_resolved_value;
					last_token->resolved_value = function_result;
				}
				else // resolved a sub expression to a value
				{
					if ( sub_expression_result.get_length() != 1 ) // make sure that sub_expression_result resolved to a single value
					{
						_set_error( string16_c( core_list_mode_e_static, L"sub expression did not evaluate to exactly one value." ), token->original_line, token->original_column );
						return false;
					}

					token->type = token_c::type_e_resolved_value;
					token->resolved_value = sub_expression_result[ 0 ];
					token_list_b.insert_at_end( token );
				}
			}
			else if ( token->type >= token_c::type_e_none && token->type <= token_c::type_e_keyword_else )
			{
				_set_error( string16_c( core_list_mode_e_static, L"illegal keyword encountered in expression." ), token->original_line, token->original_column );
				return false;
			}
			else
			{
				last_token_was_name = token->type == token_c::type_e_value_name;
				token_list_b.insert_at_end( token );
			}
		}

		// 2, resolve math multiplication operations
		token_list_a.remove_all();
		for ( sint32_c i = 0; i < token_list_b.get_length(); i++ )
		{
			token_c * token = token_list_b[ i ];
			if ( token->type >= token_c::type_e_operator_times && token->type <= token_c::type_e_operator_modulo )
			{
				if ( !_resolve_l_r_operands( token_list_a, token_list_b, i, l_operand, r_operand ) )
				{
					return false;
				}
				i++;
				if ( l_operand->get_type() == script_value_c::type_e_number && r_operand->get_type() == script_value_c::type_e_number )
				{
					if ( token->type == token_c::type_e_operator_times )
					{
						l_operand->set_number_value( l_operand->get_number_value() * r_operand->get_number_value() );
					}
					else if ( token->type == token_c::type_e_operator_divide )
					{
						l_operand->set_number_value( l_operand->get_number_value() / r_operand->get_number_value() );
					}
					else //if ( token->type == token_c::type_e_operator_modulo )
					{
						l_operand->set_number_value( l_operand->get_number_value() - ( r_operand->get_number_value() * sint32_c( l_operand->get_number_value() / r_operand->get_number_value() ) ) );
					}
				}
				else
				{
					_set_error( string16_c( core_list_mode_e_static, L"times operator only works with number value types." ), token->original_line, token->original_column );
					return false;
				}
			}
			else
			{
				token_list_a.insert_at_end( token );
			}
		}

		// 3, resolve math addition operations
		token_list_b.remove_all();
		for ( sint32_c i = 0; i < token_list_a.get_length(); i++ )
		{
			token_c * token = token_list_a[ i ];
			if ( token->type >= token_c::type_e_operator_plus && token->type <= token_c::type_e_operator_minus )
			{
				if ( !_resolve_l_r_operands( token_list_b, token_list_a, i, l_operand, r_operand ) )
				{
					return false;
				}
				i++;
				if ( l_operand->get_type() == script_value_c::type_e_number && r_operand->get_type() == script_value_c::type_e_number )
				{
					if ( token->type >= token_c::type_e_operator_plus )
					{
						l_operand->set_number_value( l_operand->get_number_value() + r_operand->get_number_value() );
					}
					else //if ( token->type >= token_c::type_e_operator_minus )
					{
						l_operand->set_number_value( l_operand->get_number_value() - r_operand->get_number_value() );
					}
				}
				else
				{
					_set_error( string16_c( core_list_mode_e_static, L"plus operator only works with number value types." ), token->original_line, token->original_column );
					return false;
				}
			}
			else
			{
				token_list_b.insert_at_end( token );
			}
		}

		// 4, resolve logical relational testing
		token_list_a.remove_all();
		for ( sint32_c i = 0; i < token_list_b.get_length(); i++ )
		{
			token_c * token = token_list_b[ i ];
			if ( token->type >= token_c::type_e_operator_is_less_than && token->type <= token_c::type_e_operator_is_greater_than_or_equal_to )
			{
				if ( !_resolve_l_r_operands( token_list_a, token_list_b, i, l_operand, r_operand ) )
				{
					return false;
				}
				i++;
				if ( l_operand->get_type() == script_value_c::type_e_number && r_operand->get_type() == script_value_c::type_e_number )
				{
					if ( token->type == token_c::type_e_operator_is_less_than )
					{
						l_operand->set_number_value( l_operand->get_number_value() < r_operand->get_number_value() ? 1.0f : 0.0f );
					}
					else if ( token->type == token_c::type_e_operator_is_less_than_or_equal_to )
					{
						l_operand->set_number_value( l_operand->get_number_value() <= r_operand->get_number_value() ? 1.0f : 0.0f );
					}
					else if ( token->type == token_c::type_e_operator_is_greater_than )
					{
						l_operand->set_number_value( l_operand->get_number_value() > r_operand->get_number_value() ? 1.0f : 0.0f );
					}
					else //if ( token->type == token_c::type_e_operator_is_greater_than_or_equal_to )
					{
						l_operand->set_number_value( l_operand->get_number_value() >= r_operand->get_number_value() ? 1.0f : 0.0f );
					}
				}
				else
				{
					_set_error( string16_c( core_list_mode_e_static,L"relational comparison operators only works with number value types." ), token->original_line, token->original_column );
					return false;
				}
			}
			else
			{
				token_list_a.insert_at_end( token );
			}
		}

		// 5, resolve logical equality testing
		token_list_b.remove_all();
		for ( sint32_c i = 0; i < token_list_a.get_length(); i++ )
		{
			token_c * token = token_list_a[ i ];
			if ( token->type >= token_c::type_e_operator_is_equal_to && token->type <= token_c::type_e_operator_is_not_equal_to )
			{
				if ( !_resolve_l_r_operands( token_list_b, token_list_a, i, l_operand, r_operand ) )
				{
					return false;
				}
				i++;
				if ( l_operand->get_type() != r_operand->get_type() )
				{
					if ( token->type == token_c::type_e_operator_is_equal_to )
					{
						l_operand->set_number_value( 0.0f );
					}
					else
					{
						l_operand->set_number_value( 1.0 );
					}
				}
				else if ( l_operand->get_type() == script_value_c::type_e_number )
				{
					if ( token->type == token_c::type_e_operator_is_equal_to )
					{
						l_operand->set_number_value( l_operand->get_number_value() == r_operand->get_number_value() ? 1.0f : 0.0f );
					}
					else
					{
						l_operand->set_number_value( l_operand->get_number_value() != r_operand->get_number_value() ? 1.0f : 0.0f );
					}
				}
				else if ( l_operand->get_type() == script_value_c::type_e_string )
				{
					if ( token->type == token_c::type_e_operator_is_equal_to )
					{
						l_operand->set_number_value( l_operand->get_string_value() == r_operand->get_string_value() ? 1.0f : 0.0f );
					}
					else
					{
						l_operand->set_number_value( l_operand->get_string_value() != r_operand->get_string_value() ? 1.0f : 0.0f );
					}
				}
				else if ( l_operand->get_type() == script_value_c::type_e_none )
				{
					if ( token->type == token_c::type_e_operator_is_equal_to )
					{
						l_operand->set_number_value( 1.0f );
					}
					else
					{
						l_operand->set_number_value( 0.0f );
					}
				}
			}
			else
			{
				token_list_b.insert_at_end( token );
			}
		}

		// 6, resolve logical not
		token_list_a.remove_all();
		for ( sint32_c i = 0; i < token_list_b.get_length(); i++ )
		{
			token_c * token = token_list_b[ i ];
			if ( token->type == token_c::type_e_operator_not )
			{
				if ( !_resolve_r_operand( token_list_b, i, r_operand ) )
				{
					return false;
				}
				i++;
				token->resolved_value.set_number_value( !r_operand->resolve_as_boolean() ? 1.0f : 0.0f );
				token_list_a.insert_at_end( token );
			}
			else
			{
				token_list_a.insert_at_end( token );
			}
		}

		// 7, resolve logical or
		token_list_b.remove_all();
		for ( sint32_c i = 0; i < token_list_a.get_length(); i++ )
		{
			token_c * token = token_list_a[ i ];
			if ( token->type == token_c::type_e_operator_or )
			{
				if ( !_resolve_l_r_operands( token_list_b, token_list_a, i, l_operand, r_operand ) )
				{
					return false;
				}
				i++;
				l_operand->set_number_value( ( l_operand->resolve_as_boolean() || r_operand->resolve_as_boolean() ) ? 1.0f : 0.0f );
			}
			else
			{
				token_list_b.insert_at_end( token );
			}
		}

		// 8, resolve logical and
		token_list_a.remove_all();
		for ( sint32_c i = 0; i < token_list_b.get_length(); i++ )
		{
			token_c * token = token_list_b[ i ];
			if ( token->type == token_c::type_e_operator_and )
			{
				if ( !_resolve_l_r_operands( token_list_a, token_list_b, i, l_operand, r_operand ) )
				{
					return false;
				}
				i++;
				l_operand->set_number_value( ( l_operand->resolve_as_boolean() && r_operand->resolve_as_boolean() ) ? 1.0f : 0.0f );
			}
			else
			{
				token_list_a.insert_at_end( token );
			}
		}

		// if there is only one token left and if it is a resolved value, then it is our final result and we are done
		if ( token_list_a.get_length() == 1 )
		{
			token_c * token = token_list_a[ 0 ];
			if ( token->type == token_c::type_e_resolved_value )
			{
				result.insert_at_end( token->resolved_value );
				return true;
			}
		}

		// 9, resolve assignments
		l_operand = nullptr;
		token_list_b.remove_all();
		for ( sint32_c i = token_list_a.get_length() - 1; i >= 0; i-- ) // scan from right to left
		{
			token_c * token = token_list_a[ i ];
			if ( token->type == token_c::type_e_operator_assign )
			{
				// get left operand, it needs to be a name
				if ( i == 0 )
				{
					_set_error( string16_c( core_list_mode_e_static, L"coult not find left operand for assignment operator." ), token->original_line, token->original_column );
					return false;
				}
				i--;
				token_c * l_token = token_list_a[ i - 1 ];
				if ( l_token->type != token_c::type_e_value_name )
				{
					_set_error( string16_c( core_list_mode_e_static, L"left operand for assignment operator must be a name." ), token->original_line, token->original_column );
					return false;
				}
				l_operand = _add_or_get_variable( l_token->string ); // this will get an existing property or make a new property in its proper scope
				if ( l_operand == nullptr )
				{
					string16_c error_message;
					error_message = L"could not assign to variable of name \"";
					error_message += l_token->string;
					error_message += L"\", the name probably refers to an invalid scope.";
					_set_error( error_message, l_token->original_line, l_token->original_column );
					return false;
				}

				// get right operand, it needs to be a value
				if ( !_resolve_r_operand( token_list_a, i, r_operand ) )
				{
					return false;
				}

				// do the assignment, this will set the value of the variable within its own scope
				*l_operand = *r_operand;

				// but we still have to copy back value of l_operand into the left token and mark the left token as resolved
				// because the left token for this iteration might contain the right operand for the next iteration
				l_token->resolved_value = *l_operand;
				l_token->type = token_c::type_e_resolved_value;
			}
		}

		// if the left operand is set then it is our final result
		if ( l_operand != nullptr )
		{
			result.insert_at_end( *l_operand );
		}

		return true;
	}

	boolean_c script_instance_c::_resolve_l_r_operands( core_list_c< token_c * > & resolved_token_list, core_list_c< token_c * > & token_list, sint32_c operator_token_index, script_value_c * & l_operand, script_value_c * & r_operand )
	{
		l_operand = 0;
		r_operand = 0;

		token_c * operator_token = token_list[ operator_token_index ];
		if ( resolved_token_list.get_length() == 0 )
		{
			_set_error( string16_c( core_list_mode_e_static, L"operator does not have operand." ), operator_token->original_line, operator_token->original_column );
			return false;
		}

		sint32_c r_token_index = operator_token_index + 1;
		if ( r_token_index >= token_list.get_length() )
		{
			_set_error( string16_c( core_list_mode_e_static, L"operator does not have right operand." ), operator_token->original_line, operator_token->original_column );
			return false;
		}

		token_c * l_token = resolved_token_list[ resolved_token_list.get_length() - 1 ]; // get last token from output
		token_c * r_token = token_list[ r_token_index ]; // get next token from input

		if ( l_token->type == token_c::type_e_resolved_value )
		{
			l_operand = &l_token->resolved_value;
		}
		else if ( l_token->type == token_c::type_e_value_name )
		{
			// value is a variable name, resolve now and store result in token
			l_operand = _get_variable( l_token->string );
			if ( l_operand == 0 )
			{
				string16_c error_message;
				error_message = L"could not resolve left operand variable \"";
				error_message += l_token->string;
				error_message += L"\" to value, variable does not exist.";
				_set_error( error_message, l_token->original_line, l_token->original_column );
				return false;
			}
			l_token->resolved_value = *l_operand; // store resolved result
			l_operand = &l_token->resolved_value; // set result as address of value in token
			l_token->type = token_c::type_e_resolved_value; // mark the token as resolved
		}
		else
		{
			_set_error( string16_c( core_list_mode_e_static, L"left operand must be a value." ), l_token->original_line, l_token->original_column );
			return false;
		}

		if ( r_token->type == token_c::type_e_resolved_value )
		{
			// value stored in token was already resolved
			r_operand = &r_token->resolved_value;
		}
		else if ( r_token->type == token_c::type_e_value_name )
		{
			// value is a variable name, resolve now and store result in token
			r_operand = _get_variable( r_token->string );
			if ( r_operand == 0 )
			{
				string16_c error_message;
				error_message = L"could not resolve right operand variable \"";
				error_message += r_token->string;
				error_message += L"\" to value, variable does not exist.";
				_set_error( error_message, r_token->original_line, r_token->original_column );
				return false;
			}
			r_token->resolved_value = *r_operand; // store resolved result
			r_operand = &r_token->resolved_value; // set result as address of value in token
			r_token->type = token_c::type_e_resolved_value; // mark the token as resolved
		}
		else
		{
			_set_error( string16_c( core_list_mode_e_static, L"right operand must be a value." ), r_token->original_line, r_token->original_column );
			return false;
		}

		return true;
	}

	boolean_c script_instance_c::_resolve_r_operand( core_list_c< token_c * > & token_list, sint32_c operator_token_index, script_value_c * & r_operand )
	{
		r_operand = 0;

		token_c * operator_token = token_list[ operator_token_index ];

		sint32_c r_token_index = operator_token_index + 1;
		if ( r_token_index >= token_list.get_length() )
		{
			_set_error( string16_c( core_list_mode_e_static, L"right operand was not found." ), operator_token->original_line, operator_token->original_column );
			return status_e_error;
		}

		token_c * r_token = token_list[ r_token_index ];

		if ( r_token->type == token_c::type_e_resolved_value )
		{
			// value stored in token was already resolved
			r_operand = &r_token->resolved_value;
		}
		else if ( r_token->type == token_c::type_e_value_name )
		{
			// value is a variable name, resolve now and store result in token
			r_operand = _get_variable( r_token->string );
			if ( r_operand == 0 )
			{
				string16_c error_message;
				error_message = L"right operand could not resolve to variable \"";
				error_message += r_token->string;
				error_message += L"\" to value, variable does not exist.";
				_set_error( error_message, r_token->original_line, r_token->original_column );
				return false;
			}
			r_token->resolved_value = *r_operand; // store resolved result
			r_operand = &r_token->resolved_value; // set result as address of value in token
			r_token->type = token_c::type_e_resolved_value; // mark the token as resolved
		}
		else
		{
			_set_error( string16_c( core_list_mode_e_static, L"right operand must be a value." ), r_token->original_line, r_token->original_column );
			return false;
		}

		return true;
	}

	script_scope_c * script_instance_c::_resolve_scope( string8_c const & full_name, string8_c & name )
	{
		core_list_c< string8_c > path_elements;
		ops::string8_split_at_delimiter( full_name, string8_c( core_list_mode_e_static, "." ), path_elements );
		if ( path_elements.get_length() == 1 )
		{
			name = path_elements[ 0 ];
			return &_scope;
		}
		if ( path_elements.get_length() == 2 )
		{
			name = path_elements[ 1 ];
			return engine_c::get_instance()->get_script_manager()->find_scope( path_elements[ 0 ] );
		}
		return nullptr;
	}

	script_value_c * script_instance_c::_get_variable( string8_c const & full_name )
	{
		string8_c name;
		script_scope_c * scope = _resolve_scope( full_name, name );
		if ( scope )
		{
			return scope->get_variable( name );
		}
		return nullptr;
	}

	script_value_c * script_instance_c::_add_or_get_variable( string8_c const & full_name )
	{
		string8_c name;
		script_scope_c * scope = _resolve_scope( full_name, name );
		if ( scope )
		{
			return scope->add_or_get_variable( name );
		}
		return nullptr;
	}

	void_c script_instance_c::_remove_variable( string8_c const & full_name )
	{
		string8_c name;
		script_scope_c * scope = _resolve_scope( full_name, name );
		if ( scope )
		{
			scope->remove_variable( name );
		}
	}

	void_c script_instance_c::_set_error( string16_c const & message, sint32_c const line, sint32_c const column )
	{
		string8_c line_string;
		ops::convert_sint32_to_string8( line, line_string );
		string8_c column_string;
		ops::convert_sint32_to_string8( column, column_string );
		_error_message = "error at: ";
		_error_message += line_string;
		_error_message += " ";
		_error_message += column_string;
		_error_message += " \"";
		_error_message += message;
		_error_message += "\"";
	}

	void_c script_instance_c::_set_error( string16_c const & message )
	{
		_set_error( message, _line_index, 0 );
	}

	script_instance_c::script_instance_c( script_context_c * Context )
		: _token_list()
		, _line_list()
		, _label_list()
		, _scope()
		, _file_path()
		, _loaded( false )
		, _status( status_e_wait )
		, _line_index( 0 )
		, _indentation( 0 )
		, _time_step( 0.0f )
		, _error_message()
		, _serial_index( -1 )
		, _master_serial_index( -1 )
		, _master( nullptr )
		, _context( Context )
	{
	}

	script_instance_c::~script_instance_c()
	{
		//if ( _master && _master->get_scene() )
		//{
		//	for ( sint32_c i = 0; i < _slaves.get_count(); i++ )
		//	{
		//		_master->get_scene()->remove_object( _slaves[ i ] );
		//		delete _slaves[ i ];
		//	}
		//}
	}

	boolean_c script_instance_c::load( string16_c const & file_path, data_stream_c * file_stream )
	{
		assert( _loaded == false );
		assert( file_stream );
		assert( file_stream->get_position() == 0 );

		// load source code and extract lines, tokens, and labels
		sint32_c source_code_size = file_stream->get_size();
		string8_c source_code;
		source_code.character_list.construct_mode_dynamic( source_code_size + 1, source_code_size + 1 );
		file_stream->load( source_code.character_list.get_internal_array(), source_code.character_list.get_length() );
		source_code.character_list[ source_code_size ] = 0;

		line_c line;
		token_c token;

		script_line_scanner_c line_scanner( source_code );
		script_token_scanner_c token_scanner;

		sint32_c line_indentation;
		while ( line_scanner.get_next_line( token_scanner.source, line_indentation ) )
		{
			line.indentation = line_indentation;
			line.token_start = _token_list.get_length();
			line.token_count = 0;

			token_scanner.index = 0;
			token_scanner.column = 0;

			while ( true )
			{
				token_scanner.skip_white_space();
				token.original_line = line_scanner.line;
				token.original_column = token_scanner.column;

				// get next token
				string8_c token_string;
				token_c::type_e token_type;
				status_e token_scanner_status = token_scanner.get_next_token( token_string, token_type );
				if ( token_scanner_status == status_e_error )
				{
					_set_error( token_scanner.error_message, line_scanner.line, token_scanner.column );
					return false;
				}
				else if ( token_scanner_status == status_e_done )
				{
					break;
				}

				// resolve constant values if able
				token.type = token_type;
				if ( token_type == token_c::type_e_value_name || token_type == token_c::type_e_value_none || token_type == token_c::type_e_value_boolean || token_type == token_c::type_e_value_number || token_type == token_c::type_e_value_string )
				{
					token.string = token_string;
				}

				// 
				_token_list.insert_at_end( token );
			}

			// add line
			line.token_count = _token_list.get_length() - line.token_start;
			if ( line.token_count > 0 )
			{
				_line_list.insert_at_end( line );
			}
		}

		// resolve token array pointers
		for ( sint32_c i = 0; i < _line_list.get_length(); i++ )
		{
			_line_list[ i ].token_array = &_token_list[ _line_list[ i ].token_start ];
		}

		// find labels
		for ( sint32_c i = 0; i < _line_list.get_length(); i++ )
		{
			line_c & line = _line_list[ i ];
			if ( line.token_array[ 0 ].type == token_c::type_e_keyword_label )
			{
				if ( line.token_count != 2 )
				{
					_set_error( string16_c( core_list_mode_e_static, L"label line must contain exactly 2 tokens." ), i, 0 );
					return false;
				}

				if ( line.token_array[ 1 ].type != token_c::type_e_value_name )
				{
					_set_error( string16_c( core_list_mode_e_static, L"label must have a name." ), i, 0 );
					return false;
				}

				label_c label;
				label.line = i;
				label.name = line.token_array[ 1 ].string;
				_label_list.insert_at_end( label );
			}
		}

		// done
		_loaded = true;
		return true;
	}

	boolean_c script_instance_c::update( float32_c time_step )
	{
		if ( _status == status_e_error || _status == status_e_done )
		{
			return false;
		}

		_time_step = time_step;

		while ( _line_index < _line_list.get_length() )
		{
			line_c & line = _line_list[ _line_index ];

			if ( line.indentation < _indentation )
			{
				_indentation = line.indentation;
			}
			else if ( line.indentation > _indentation )
			{
				_set_error( string16_c( core_list_mode_e_static, L"bad indentation." ), line.token_array[ 0 ].original_line, line.token_array[ 0 ].original_column );
				_status = status_e_error;
				return false;
			}

			if ( line.token_array[ 0 ].type == token_c::type_e_keyword_else )
			{
				if ( line.resolved_if_group == false )
				{
					if ( line.token_array[ 1 ].type == token_c::type_e_keyword_if ) // else if
					{
						// resolve the if expression
						core_list_c< script_value_c > if_expression_result;
						if ( !_evaluate_expression( if_expression_result, &line.token_array[ 2 ], line.token_count - 2 ) )
						{
							return false;
						}

						if ( if_expression_result.get_length() != 1 )
						{
							_set_error( string16_c( core_list_mode_e_static, L"if conditional expression did not resolve into a single value." ), line.token_array[ 0 ].original_line, line.token_array[ 0 ].original_column );
							_status = status_e_error;
							return false;
						}

						if ( if_expression_result.get_length() == 1 && if_expression_result[ 0 ].resolve_as_boolean() )
						{
							// tag all of the if/else if/else's in the if group as resolved
							script_reset_if_block_resolution( _line_list.get_internal_array(), _line_index, _line_list.get_length(), true );

							// go to next line (enter the sub block)
							_line_index++;
							if ( _line_index < _line_list.get_length() )
							{
								_indentation = _line_list[ _line_index ].indentation;
							}
						}
					}
					else // else
					{
					}
				}
				else // go to end of if group
				{
				}
			}
			else if ( line.token_array[ 0 ].type == token_c::type_e_keyword_if ) // if
			{
				// tag all of the if/else if/else's the if group as unresolved
				script_reset_if_block_resolution( _line_list.get_internal_array(), _line_index, _line_list.get_length(), false );

				// resolve the if expression
				core_list_c< script_value_c > if_expression_result;
				if ( !_evaluate_expression( if_expression_result, &line.token_array[ 1 ], line.token_count - 1 ) )
				{
					return false;
				}

				if ( if_expression_result.get_length() != 1 )
				{
					_set_error( string16_c( core_list_mode_e_static, L"if conditional expression did not resolve into a single value." ), line.token_array[ 0 ].original_line, line.token_array[ 0 ].original_column );
					_status = status_e_error;
					return false;
				}

				// enter the if block if needed
				if ( if_expression_result.get_length() == 1 && if_expression_result[ 0 ].resolve_as_boolean() )
				{
					// tag all of the if/else if/else's in the if group as resolved
					script_reset_if_block_resolution( _line_list.get_internal_array(), _line_index, _line_list.get_length(), true );

					// go to next line (enter the sub block)
					_line_index++;
					if ( _line_index < _line_list.get_length() )
					{
						_indentation = _line_list[ _line_index ].indentation;
					}
				}
				else // go to next else if/else
				{
					_line_index++;
					while ( _line_index < _line_list.get_length() )
					{
						if ( _line_list[ _line_index ].indentation == _indentation && _line_list[ _line_index ].token_array[ 0 ].type == token_c::type_e_keyword_else )
						{
							_line_index++;
							break;
						}
						_line_index++;
					}
				}
			}
			else if ( line.token_array[ 0 ].type == token_c::type_e_keyword_goto ) // goto
			{
				string8_c & label_name = line.token_array[ 1 ].string;
				for ( sint32_c j = 0; j < _label_list.get_length(); j++ )
				{
					if ( _label_list[ j ].name == label_name )
					{
						_line_index = _label_list[ j ].line;
						_line_index++;
						continue;
					}
					string16_c error_message;
					error_message = L"could not find goto label \"";
					error_message += label_name;
					error_message += "L\".";
					_set_error( error_message, line.token_array[ 1 ].original_line, line.token_array[ 1 ].original_column );
					_status = status_e_error;
					return false;
				}
			}
			else if ( line.token_array[ 0 ].type == token_c::type_e_keyword_label ) // label, do nothing
			{
				_line_index++;
			}
			else if ( line.token_array[ 0 ].type == token_c::type_e_keyword_finish )
			{
				_status = status_e_done;
				return true;
			}
			else // normal line, evaluate it as an expression
			{
				core_list_c< script_value_c > result;
				if ( !_evaluate_expression( result, line.token_array, line.token_count ) )
				{
					return false;
				}
				if ( _status == status_e_wait )
				{
					return true;
				}
				_line_index++;
			}
		}

		_status = status_e_done;
		return true;
	}

	script_instance_c::status_e script_instance_c::get_status()
	{
		return static_cast< status_e >( _status );
	}

	void_c script_instance_c::set_status( status_e value )
	{
		_status = value;
	}

	string16_c const & script_instance_c::get_file_path() const
	{
		return _file_path;
	}

	string16_c const & script_instance_c::get_error_message() const
	{
		return _error_message;
	}

	sint32_c script_instance_c::get_serial_index()
	{
		return _serial_index;
	}

	sint32_c script_instance_c::get_master_serial_index()
	{
		return _master_serial_index;
	}

	void_c script_instance_c::set_master_serial_index( sint32_c value )
	{
		_master_serial_index = value;
	}

	void_c * script_instance_c::get_master()
	{
		return _master;
	}

	void_c script_instance_c::set_master( void_c * value )
	{
		_master = value;
	}

	script_context_c * script_instance_c::get_context()
	{
		return _context;
	}

	void_c script_instance_c::serial_save( data_scribe_binary_property_c & scribe )
	{
		scribe.save_list_begin( "script" );

		scribe.save_list_begin( "token_list" );
		for ( sint32_c i = 0; i < _token_list.get_length(); i++ )
		{
			token_c & token = _token_list[ i ];
			scribe.save_list_begin( "token" );
			scribe.save_uint8_buffer( "type", &token.type, 1 );
			if ( token.string.get_length() > 0 )
			{
				scribe.save_string8( "string", token.string );
			}
			scribe.save_sint32_buffer( "original_line", &token.original_line, 1 );
			scribe.save_sint32_buffer( "original_column", &token.original_column, 1 );
			scribe.save_end();
			scribe.save_list_end( 0 );
		}
		scribe.save_list_end( _token_list.get_length() );

		scribe.save_list_begin( "line_list" );
		for ( sint32_c i = 0; i < _line_list.get_length(); i++ )
		{
			line_c & line = _line_list[ i ];
			scribe.save_list_begin( "line" );
			scribe.save_sint32_buffer( "indentation", &line.indentation, 1 );
			scribe.save_sint32_buffer( "token_start", &line.token_start, 1 );
			scribe.save_sint32_buffer( "token_count", &line.token_count, 1 );
			scribe.save_end();
			scribe.save_list_end( 0 );
		}
		scribe.save_list_end( _line_list.get_length() );

		scribe.save_list_begin( "label_list" );
		for ( sint32_c i = 0; i < _label_list.get_length(); i++ )
		{
			label_c & label = _label_list[ i ];
			scribe.save_list_begin( "label" );
			scribe.save_string8( "name", label.name );
			scribe.save_sint32_buffer( "line", &label.line, 1 );
			scribe.save_end();
			scribe.save_list_end( 0 );
		}
		scribe.save_list_end( _label_list.get_length() );

		_scope.save( scribe, "scope" );

		scribe.save_uint8_buffer( "status", &_status, 1 );
		scribe.save_sint32_buffer( "line_index", &_line_index, 1 );
		scribe.save_sint32_buffer( "indentation", &_indentation, 1 );
		scribe.save_float64_buffer( "time_step", &_time_step, 1 );
		scribe.save_string8( "error_message", string8_c( _error_message ) );

		scribe.save_string8( "file_path", string8_c( _file_path ) );
		uint8_c loaded_uint8 = _loaded ? 1 : 0;
		scribe.save_uint8_buffer( "loaded", &loaded_uint8, 1 );

		scribe.save_sint32_buffer( "serial_index", &_serial_index, 1 );

		scribe.save_sint32_buffer( "master_serial_index", &_master_serial_index, 1 );

		scribe.save_end();
		scribe.save_list_end( 0 );
	}

	void_c script_instance_c::serial_load( data_scribe_binary_property_c & scribe )
	{
		assert( scribe.loaded_check( "script", data_type_e_list ) );
		scribe.loaded_list_enter();
		while ( scribe.load() )
		{
			if ( scribe.loaded_check( "token_list", data_type_e_list ) )
			{
				scribe.loaded_list_enter();
				sint32_c length = scribe.loaded_list_length();
				_token_list.construct_mode_dynamic( length, length );
				for ( sint32_c i = 0; i < length; i++ )
				{
					token_c & token = _token_list[ i ];
					scribe.load();
					assert( scribe.loaded_check( "token", data_type_e_list, 0 ) );
					scribe.loaded_list_enter();
					while ( scribe.load() )
					{
						if ( scribe.loaded_check( "type", data_type_e_uint8, 1 ) )
						{
							token.type = scribe.loaded_uint8_buffer()[ 0 ];
						}
						else if ( scribe.loaded_check( "string", data_type_e_string8, 0 ) )
						{
							token.string = scribe.loaded_string8();
						}
						else if ( scribe.loaded_check( "original_line", data_type_e_sint32, 1 ) )
						{
							token.original_line = scribe.loaded_sint32_buffer()[ 0 ];
						}
						else if ( scribe.loaded_check( "original_column", data_type_e_sint32, 1 ) )
						{
							token.original_line = scribe.loaded_sint32_buffer()[ 0 ];
						}
					}
					scribe.loaded_list_exit();
				}
				scribe.loaded_list_exit();
			}
			else if ( scribe.loaded_check( "line_list", data_type_e_uint8, 0 ) )
			{
				scribe.loaded_list_enter();
				sint32_c length = scribe.loaded_list_length();
				_line_list.construct_mode_dynamic( length, length );
				for ( sint32_c i = 0; i < length; i++ )
				{
					line_c & line = _line_list[i];
					scribe.load();
					assert( scribe.loaded_check( "line", data_type_e_list, 0 ) );
					scribe.loaded_list_enter();
					while ( scribe.load() )
					{
						if ( scribe.loaded_check( "indentation", data_type_e_sint32, 1 ) )
						{
							line.indentation = scribe.loaded_sint32_buffer()[ 0 ];
						}
						else if ( scribe.loaded_check( "token_start", data_type_e_sint32, 1 ) )
						{
							line.token_start = scribe.loaded_sint32_buffer()[ 0 ];
						}
						else if ( scribe.loaded_check( "token_count", data_type_e_sint32, 1 ) )
						{
							line.token_count = scribe.loaded_sint32_buffer()[ 0 ];
						}
					}
					scribe.loaded_list_exit();
					assert( line.token_start + line.token_count <= _token_list.get_length() );
					line.token_array = &_token_list[ line.token_start ];
				}
				scribe.loaded_list_exit();
			}
			else if ( scribe.loaded_check( "label_list", data_type_e_list, 0 ) )
			{
				scribe.loaded_list_enter();
				sint32_c length = scribe.loaded_list_length();
				_label_list.construct_mode_dynamic( length, length );
				for ( sint32_c i = 0; i < length; i++ )
				{
					label_c & label = _label_list[i];
					scribe.load();
					assert( scribe.loaded_check( "label", data_type_e_list, 0 ) );
					scribe.loaded_list_enter();
					while ( scribe.load() )
					{
						if ( scribe.loaded_check( "name", data_type_e_string8, 0 ) )
						{
							label.name = scribe.loaded_string8();
						}
						else if ( scribe.loaded_check( "line", data_type_e_sint32, 1 ) )
						{
							label.line = scribe.loaded_sint32_buffer()[ 0 ];
						}
					}
					scribe.loaded_list_exit();
				}
				scribe.loaded_list_exit();
			}
			else if ( scribe.loaded_check( "scope", data_type_e_list, 0 ) )
			{
				_scope.load( scribe, "scope" );
			}
			else if ( scribe.loaded_check( "status", data_type_e_uint8, 1 ) )
			{
				_status = static_cast< status_e >( scribe.loaded_uint8_buffer()[ 0 ] );
			}
			else if ( scribe.loaded_check( "line_index", data_type_e_sint32, 1 ) )
			{
				_line_index = scribe.loaded_sint32_buffer()[ 0 ];
			}
			else if ( scribe.loaded_check( "indentation", data_type_e_sint32, 1 ) )
			{
				_indentation = scribe.loaded_sint32_buffer()[ 0 ];
			}
			else if ( scribe.loaded_check( "time_step", data_type_e_float64, 1 ) )
			{
				_time_step = scribe.loaded_float64_buffer()[ 0 ];
			}
			else if ( scribe.loaded_check( "error_message", data_type_e_string8, 0 ) )
			{
				_error_message = scribe.loaded_string8();
			}

			else if ( scribe.loaded_check( "file_path", data_type_e_string8, 0 ) )
			{
				_file_path = scribe.loaded_string8();
			}
			else if ( scribe.loaded_check( "loaded", data_type_e_uint8, 1 ) )
			{
				_loaded = scribe.loaded_uint8_buffer()[ 0 ] != 0;
			}

			else if ( scribe.loaded_check( "serial_index", data_type_e_sint32, 1 ) )
			{
				_serial_index = scribe.loaded_sint32_buffer()[ 0 ];
			}

			else if ( scribe.loaded_check( "master_serial_index", data_type_e_sint32, 1 ) )
			{
				_master_serial_index = scribe.loaded_sint32_buffer()[ 0 ];
			}
		}
		scribe.loaded_list_exit();
	}

}
