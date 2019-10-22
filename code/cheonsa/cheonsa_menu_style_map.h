#pragma once

#include "cheonsa_menu_types.h"

namespace cheonsa
{

	// the style map defines which styles to map to which daughter elements, and which style maps to map to which daughter controls.
	// it also can define named property values that control implementations can use, for example for defining the fixed thickness of scroll bars, or fixed height of text boxes or combo boxes, etc.
	class menu_style_map_c
	{
	public:
		// used by controls to hold a reference to a style map inistance.
		class reference_c
		{
		private:
			string8_c _key; // persistent key, remains the same between reloading of style files.
			menu_style_map_c const * _value; // volatile reference, changes between reloading of style files.

		public:
			reference_c();
			reference_c( reference_c const & ) = delete;
			reference_c & operator = ( reference_c const & ) = delete;
			
			void_c refresh();

			void_c clear();

			string8_c const & get_key() const;
			void_c set_key( string8_c const & value );

			menu_style_map_c const * get_value() const;
			void_c set_value( menu_style_map_c const * value );

			core_event_c< void_c, reference_c const * > on_refreshed;

		};

		// maps a style key assignment to an element in the control.
		class entry_c
		{
		public:
			//enum target_type_e
			//{
			//	target_type_e_none,
			//	target_type_e_element, // maps an element style to a sub-element in this control.
			//	target_type_e_control, // maps a control style map to a sub-control in this control.
			//};
			string8_c target;
			//target_type_e target_type;
			//string8_c target_name; // the name of the element or control to apply the style or style map to.
			string8_c style_key; // the key of the element style or control style map to apply to the target element or control. if the target is a frame element then this will be a frame style key, if the target is a text element then this will be a text style key, if the target is a control then this will be a style map key.
			menu_anchor_e anchor; // used to adjust the insets of the element rectangle relative to the control rectangle.
			box32x2_c anchor_measures;

		public:
			entry_c();

			string8_c get_target_type() const; // everything before the ":" in target, should be "frame", "text", or "control".
			string8_c get_target_name() const; // everything after the ":" in target.

		};

		// defines an arbitrary property that can be used to define colors or layout metrics or anything (depending on if the control is programmed to use the property).
		class property_c
		{
		public:
			string8_c name; // this property's name.
			string8_c value;

		public:
			property_c();

		};

	public:
		string8_c key; // unique key to identify this style map by.
		core_list_c< entry_c * > entry_list; // defines style mappings for sub-elements and sub-controls.
		core_list_c< property_c * > property_list; // defines property values for this control.

	public:
		menu_style_map_c();
		~menu_style_map_c();

		void_c load( data_scribe_markup_c::node_c const * node );

		entry_c const * find_entry( string8_c const & target ) const;

		property_c const * find_property( string8_c const & name ) const;
		boolean_c find_property_as_float32( string8_c const & name, float32_c & property_value_as_float32 ) const;

	};

}
