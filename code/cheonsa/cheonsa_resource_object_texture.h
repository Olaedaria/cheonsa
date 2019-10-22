#pragma once

#include "cheonsa_resource_object.h"
#include "cheonsa_video_interface.h"
#include "cheonsa_image.h"

namespace cheonsa
{

	// 2d texture.
	// can load from png or jpg file.
	class resource_object_texture_c : public resource_object_c
	{
	public:
		typedef resource_object_reference_c< resource_object_texture_c > reference_c;

	public:
		static char8_c const * get_type_static() { return "texture"; }
		virtual char8_c const * get_type() const override { return get_type_static(); }

	public:
		video_texture_c * _video_texture;

	private:
		boolean_c _is_wrapper; // if true then this texture resource instance is wrapping an externally managed _video_texture instance. if false then this texture resource instance in charge of managing its own _video_texture instance.

	public:
		virtual boolean_c _load( data_stream_c * stream ) override;
		virtual void_c _unload() override;

	public:
		resource_object_texture_c();
		resource_object_texture_c( resource_object_texture_c const & ) = delete;
		resource_object_texture_c( boolean_c is_wrapper ); // constructs this texture in wrapper mode.
		virtual ~resource_object_texture_c() override;
		resource_object_texture_c & operator = ( resource_object_texture_c const & ) = delete;

		void_c wrap( video_texture_c * video_texture ); // changes the video texture that is wrapped by this instance.

		// creates a video texture from the given image.
		static boolean_c create_video_texture_from_image(
			image_c const & image,
			video_texture_c * & video_texture // the a video texture pointer variable, this will be set to a new instance of a video texture if the function is successful. the format of the resulting texture will be the closest fit to the format of the input image.
		);

		// gets the image from a video texture.
		// the result image format will be as close of a match as possible to the video texture format.
		// this will assert if the video texture was not created with get_data_enable set to true.
		static boolean_c get_image_from_video_texture(
			video_texture_c const * video_texture,
			image_c & image // to get the original format then set channel_count and channel_bit_depth to 0. to request a specific format then set channel_count and channel_bit_depth, then the result will be converted to fit this format.
		);

	};

}
