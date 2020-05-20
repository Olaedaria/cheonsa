#pragma once

#include "cheonsa__types.h"
#include "cheonsa_resource_file.h"
#include "cheonsa_video_interface.h"
#include "cheonsa_image.h"

namespace cheonsa
{

	// 2d texture.
	// loads ".png" and ".jpg" files.
	// can load 1 channel (r), 2 channel (rg), 3 channel (will be converted to rgba with opaque alpha (1.0) for gpu (gpu does not support 3 channel textures)), and 4 channel (rgba) textures.
	// can load 8-bit per channel bit depth (jpg, png) and 16-bit per channel bit depth (png). these are interpreted ias 8-bit unorm and 16-bit unorm respectively on the gpu.
	// for png, can load palettized textures (will be converted to rgba for gpu).
	class resource_file_texture_c
		: public resource_file_c
	{
	public:
		typedef resource_file_reference_c< resource_file_texture_c > reference_c;

	public:
		static char8_c const * get_resource_file_type_static() { return "texture"; }
		virtual char8_c const * get_resource_file_type() const override { return get_resource_file_type_static(); }

	private:
		boolean_c _is_wrapper; // if true then this texture resource instance is wrapping an externally managed _video_texture instance. if false then this texture resource instance in charge of managing its own _video_texture instance.
		video_texture_c * _video_texture;

	public:
		virtual void_c _load( data_stream_c * stream ) override;
		virtual void_c _unload() override;

	public:
		resource_file_texture_c( string16_c const & file_path );
		resource_file_texture_c( boolean_c is_wrapper ); // constructs this texture in wrapper mode.

		video_texture_c * get_video_texture() const;
		void_c set_video_texture( video_texture_c * video_texture ); // only valid to use if this texture resource was constructed with the is_wrapper constructor.

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
