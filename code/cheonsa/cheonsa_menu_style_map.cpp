#include "cheonsa_menu_style_map.h"
#include "cheonsa_engine.h"
#include "cheonsa_ops.h"

namespace cheonsa
{

	menu_style_map_c::reference_c::reference_c()
		: _key()
		, _value( nullptr )
	{
	}

	void_c menu_style_map_c::reference_c::refresh()
	{
		_value = global_engine_instance.interfaces.menu_style_manager->find_style_map( _key );
		on_refreshed.invoke( this );
	}

	void_c menu_style_map_c::reference_c::clear()
	{
		_key = string8_c();
		_value = nullptr;
		on_refreshed.invoke( this );
	}

	string8_c const & menu_style_map_c::reference_c::get_key() const
	{
		return _key;
	}

	void_c menu_style_map_c::reference_c::set_key( string8_c const & value )
	{
		_key = value;
		refresh();
	}

	menu_style_map_c const * menu_style_map_c::reference_c::get_value() const
	{
		return _value;
	}

	void_c menu_style_map_c::reference_c::set_value( menu_style_map_c const * value )
	{
		_key = value ? value->key : string8_c();
		_value = value;
		on_refreshed.invoke( this );
	}

	menu_style_map_c::entry_c::entry_c()
		//: target_type( target_type_e_none )
		//, target_name()
		: target()
		, style_key()
		, anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom ) // anchor to all sides by default, so that element takes up full area of mother.
		, anchor_measures() // default all measures to 0, so that element takes up full area of mother.
	{
	}

	string8_c menu_style_map_c::entry_c::get_target_type() const
	{
		string8_c result;
		sint32_c index = 0;
		if ( ops::string8_find_index_of( target, string8_c( mode_e_static, ":" ), index ) )
		{
			result = ops::string8_sub_string( target, 0, index );
		}
		return result;
	}

	string8_c menu_style_map_c::entry_c::get_target_name() const
	{
		string8_c result;
		sint32_c index = 0;
		if ( ops::string8_find_index_of( target, string8_c( mode_e_static, ":" ), index ) )
		{
			result = ops::string8_sub_string( target, index + 1, target.get_length() - ( index + 1 ) );
		}
		return result;
	}

	menu_style_map_c::property_c::property_c()
		: name()
		, value()
	{
	}

	menu_style_map_c::menu_style_map_c()
		: key()
		, entry_list()
		, property_list()
	{
	}

	menu_style_map_c::~menu_style_map_c()
	{
		for ( sint32_c i = 0; i < entry_list.get_length(); i++ )
		{
			delete entry_list[ i ];
		}
		for ( sint32_c i = 0; i < property_list.get_length(); i++ )
		{
			delete property_list[ i ];
		}
	}

	void_c menu_style_map_c::load( data_scribe_markup_c::node_c const * node )
	{
		data_scribe_markup_c::attribute_c const * attribute;

		attribute = node->find_attribute( "key" );
		if ( attribute )
		{
			key = attribute->get_value();
		}

		data_scribe_markup_c::node_c const * sub_node = node->get_first_daughter();
		while ( sub_node != nullptr )
		{
			if ( sub_node->get_value() == "entry" )
			{
				entry_c * entry = new entry_c();
				entry_list.insert_at_end( entry );

				attribute = sub_node->find_attribute( "target" );
				if ( attribute )
				{
					entry->target = attribute->get_value();
				}

				attribute = sub_node->find_attribute( "style_key" );
				if ( attribute )
				{
					entry->style_key = attribute->get_value();
				}

				attribute = sub_node->find_attribute( "anchor" );
				if ( attribute )
				{
					entry->anchor = menu_anchor_e_none;
					sint32_c index_of = 0;
					if ( ops::string8_find_index_of( attribute->get_value(), string8_c( mode_e_static, "left" ), index_of ) )
					{
						entry->anchor |= menu_anchor_e_left;
					}
					if ( ops::string8_find_index_of( attribute->get_value(), string8_c( mode_e_static, "top" ), index_of ) )
					{
						entry->anchor |= menu_anchor_e_top;
					}
					if ( ops::string8_find_index_of( attribute->get_value(), string8_c( mode_e_static, "right" ), index_of ) )
					{
						entry->anchor |= menu_anchor_e_right;
					}
					if ( ops::string8_find_index_of( attribute->get_value(), string8_c( mode_e_static, "bottom" ), index_of ) )
					{
						entry->anchor |= menu_anchor_e_bottom;
					}
				}

				attribute = sub_node->find_attribute( "anchor_measures" );
				if ( attribute )
				{
					ops::convert_string8_to_float32xn( attribute->get_value(), core_list_c< float32_c >( mode_e_static, entry->anchor_measures.as_array(), 4 ) );
				}
			}
			else if ( sub_node->get_value() == "property" )
			{
				property_c * property = new property_c();
				property_list.insert_at_end( property );

				attribute = sub_node->find_attribute( "name" );
				if ( attribute )
				{
					property->name = attribute->get_value();
				}

				attribute = sub_node->find_attribute( "value" );
				if ( attribute )
				{
					property->value = attribute->get_value();
				}
			}
			sub_node = sub_node->get_next_sister();
		}
	}

	menu_style_map_c::entry_c const * menu_style_map_c::find_entry( string8_c const & target ) const
	{
		for ( sint32_c i = 0; i < entry_list.get_length(); i++ )
		{
			if ( entry_list[ i ]->target == target )
			{
				return entry_list[ i ];
			}
		}
		return nullptr;
	}

	menu_style_map_c::property_c const * menu_style_map_c::find_property( string8_c const & name ) const
	{
		for ( sint32_c i = 0; i < property_list.get_length(); i++ )
		{
			if ( property_list[ i ]->name == name )
			{
				return property_list[ i ];
			}
		}
		return nullptr;
	}

	boolean_c menu_style_map_c::find_property_as_float32( string8_c const & name, float32_c & property_value_as_float32 ) const
	{
		property_c const * property = find_property( name );
		if ( property )
		{
			return ops::convert_string8_to_float32( property->value, property_value_as_float32 );
		}
		return false;
	}

}
