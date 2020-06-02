#include "cheonsa__ops.h"
#include "cheonsa_engine.h"
#include <cassert>
#if defined( cheonsa_platform_windows )
#include "cheonsa_video_interface_d3d11.h"
#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <d3dcompiler.h> // just for D3DCOMPILE_PACK_MATRIX_ROW_MAJOR.
#if defined( _DEBUG )
#include <dxgidebug.h>
#define com_safe_release( p ) { if ( ( p ) ) { ( p )->Release(); ( p ) = nullptr; } } // "com" means "component object model", it is a microsoft design pattern that the direct3d uses.
#endif

namespace cheonsa
{

	FARPROC _D3DCompile = nullptr; // global pointer to D3DCompile function acquired from "d3dcompiler_*.dll".

	//FARPROC video_interface_d3d11_c::_D3DCompile = nullptr;

	//typedef HRESULT( WINAPI *D3DCompile_f )(
	//	_In_reads_bytes_( SrcDatasize ) LPCVOID pSrcData,
	//	_In_ SIZE_T SrcDatasize,
	//	_In_opt_ LPCSTR pSourceName,
	//	_In_reads_opt_( _Inexpressible_( pDefines->name != NULL ) ) CONST D3D_SHADER_MACRO* pDefines,
	//	_In_opt_ ID3DInclude* pInclude,
	//	_In_ LPCSTR pEntrypoint,
	//	_In_ LPCSTR pTarget,
	//	_In_ UINT Flags1,
	//	_In_ UINT Flags2,
	//	_Out_ ID3DBlob** ppCode,
	//	_Out_opt_ ID3DBlob** ppErrorMsgs );

	namespace d3d11_helpers
	{

		DXGI_FORMAT get_format( video_texture_format_e texture_format )
		{
			static DXGI_FORMAT const map[ video_texture_format_e_count_ ] = {
				DXGI_FORMAT_UNKNOWN,
				
				DXGI_FORMAT_R8_SINT,
				DXGI_FORMAT_R8_UINT,
				DXGI_FORMAT_R8_SNORM,
				DXGI_FORMAT_R8_UNORM,

				DXGI_FORMAT_R16_SINT,
				DXGI_FORMAT_R16_UINT,
				DXGI_FORMAT_R16_SNORM,
				DXGI_FORMAT_R16_UNORM,
				DXGI_FORMAT_R16_FLOAT,

				DXGI_FORMAT_R32_SINT,
				DXGI_FORMAT_R32_UINT,
				DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_R32_FLOAT,

				DXGI_FORMAT_R8G8_SINT,
				DXGI_FORMAT_R8G8_UINT,
				DXGI_FORMAT_R8G8_SNORM,
				DXGI_FORMAT_R8G8_UNORM,

				DXGI_FORMAT_R16G16_SINT,
				DXGI_FORMAT_R16G16_UINT,
				DXGI_FORMAT_R16G16_SNORM,
				DXGI_FORMAT_R16G16_UNORM,
				DXGI_FORMAT_R16G16_FLOAT,

				DXGI_FORMAT_R32G32_SINT,
				DXGI_FORMAT_R32G32_UINT,
				DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_R32G32_FLOAT,

				DXGI_FORMAT_R8G8B8A8_SINT,
				DXGI_FORMAT_R8G8B8A8_UINT,
				DXGI_FORMAT_R8G8B8A8_SNORM,
				DXGI_FORMAT_R8G8B8A8_UNORM,

				DXGI_FORMAT_R8G8B8A8_UNORM,

				DXGI_FORMAT_R16G16B16A16_SINT,
				DXGI_FORMAT_R16G16B16A16_UINT,
				DXGI_FORMAT_R16G16B16A16_SNORM,
				DXGI_FORMAT_R16G16B16A16_UNORM,
				DXGI_FORMAT_R16G16B16A16_FLOAT,

				DXGI_FORMAT_R32G32B32A32_SINT,
				DXGI_FORMAT_R32G32B32A32_UINT,
				DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_R32G32B32A32_FLOAT
			};
			assert( texture_format > 0 && texture_format < video_texture_format_e_count_ );
			assert( map[ texture_format ] != DXGI_FORMAT_UNKNOWN );
			return map[ texture_format ];
		}

		DXGI_FORMAT get_format( video_depth_stencil_format_e depth_stencil_format )
		{
			static DXGI_FORMAT const map[ video_depth_stencil_format_e_count_ ] = {
				DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_D24_UNORM_S8_UINT,
				DXGI_FORMAT_D32_FLOAT
			};
			assert( depth_stencil_format > 0 && depth_stencil_format < video_depth_stencil_format_e_count_ );
			assert( map[ depth_stencil_format ] != DXGI_FORMAT_UNKNOWN );
			return map[ depth_stencil_format ];
		}

		DXGI_FORMAT get_format( video_vertex_format_c::element_c::type_e type, uint8_c count )
		{
			switch ( type )
			{
				case video_vertex_format_c::element_c::type_e_uint8:
					switch ( count )
					{
						case 1:
							return DXGI_FORMAT_R8_UINT;
						case 2:
							return DXGI_FORMAT_R8G8_UINT;
						case 4:
							return DXGI_FORMAT_R8G8B8A8_UINT;
					}
					break;
				case video_vertex_format_c::element_c::type_e_sint8:
					switch ( count )
					{
						case 1:
							return DXGI_FORMAT_R8_SINT;
						case 2:
							return DXGI_FORMAT_R8G8_SINT;
						case 4:
							return DXGI_FORMAT_R8G8B8A8_SINT;
					}
					break;
				case video_vertex_format_c::element_c::type_e_unorm8:
					switch ( count )
					{
						case 1:
							return DXGI_FORMAT_R8_UNORM;
						case 2:
							return DXGI_FORMAT_R8G8_UNORM;
						case 4:
							return DXGI_FORMAT_R8G8B8A8_UNORM;
					}
					break;
				case video_vertex_format_c::element_c::type_e_snorm8:
					switch ( count )
					{
						case 1:
							return DXGI_FORMAT_R8_SNORM;
						case 2:
							return DXGI_FORMAT_R8G8_SNORM;
						case 4:
							return DXGI_FORMAT_R8G8B8A8_SNORM;
					}
					break;
				case video_vertex_format_c::element_c::type_e_uint16:
					switch ( count )
					{
						case 1:
							return DXGI_FORMAT_R16_UINT;
						case 2:
							return DXGI_FORMAT_R16G16_UINT;
						case 4:
							return DXGI_FORMAT_R16G16B16A16_UINT;
					}
					break;
				case video_vertex_format_c::element_c::type_e_sint16:
					switch ( count )
					{
						case 1:
							return DXGI_FORMAT_R16_SINT;
						case 2:
							return DXGI_FORMAT_R16G16_SINT;
						case 4:
							return DXGI_FORMAT_R16G16B16A16_SINT;
					}
					break;
				case video_vertex_format_c::element_c::type_e_unorm16:
					switch ( count )
					{
						case 1:
							return DXGI_FORMAT_R16_UNORM;
						case 2:
							return DXGI_FORMAT_R16G16_UNORM;
						case 4:
							return DXGI_FORMAT_R16G16B16A16_UNORM;
					}
					break;
				case video_vertex_format_c::element_c::type_e_snorm16:
					switch ( count )
					{
						case 1:
							return DXGI_FORMAT_R16_SNORM;
						case 2:
							return DXGI_FORMAT_R16G16_SNORM;
						case 4:
							return DXGI_FORMAT_R16G16B16A16_SNORM;
					}
					break;
				case video_vertex_format_c::element_c::type_e_float16:
					switch ( count )
					{
						case 1:
							return DXGI_FORMAT_R16_FLOAT;
						case 2:
							return DXGI_FORMAT_R16G16_FLOAT;
						case 4:
							return DXGI_FORMAT_R16G16B16A16_FLOAT;
					}
					break;
				case video_vertex_format_c::element_c::type_e_float32:
					switch ( count )
					{
						case 1:
							return DXGI_FORMAT_R32_FLOAT;
						case 2:
							return DXGI_FORMAT_R32G32_FLOAT;
						case 3:
							return DXGI_FORMAT_R32G32B32_FLOAT;
						case 4:
							return DXGI_FORMAT_R32G32B32A32_FLOAT;
					}
					break;
			}
			assert( false );
			return DXGI_FORMAT_UNKNOWN;
		}

		DXGI_FORMAT get_format( video_index_format_e index_format )
		{
			static DXGI_FORMAT const map[ video_index_format_e_count_ ] = {
				DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_R16_UINT,
				DXGI_FORMAT_R32_UINT
			};
			assert( index_format > 0 && index_format < video_index_format_e_count_ );
			return map[ index_format ];
		}

		D3D11_PRIMITIVE_TOPOLOGY get_primitive_topology( video_primitive_type_e primitive_type )
		{
			static D3D11_PRIMITIVE_TOPOLOGY const map[ video_primitive_type_e_count_ ] = {
				D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
				D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
				D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
			};
			assert( primitive_type >= 0 && primitive_type < video_primitive_type_e_count_ );
			return map[ primitive_type ];
		}

		D3D11_COMPARISON_FUNC get_comparison_function( video_compare_function_e compare_mode )
		{
			static D3D11_COMPARISON_FUNC const map[ video_compare_function_e_count_ ] = {
				D3D11_COMPARISON_NEVER,
				D3D11_COMPARISON_NEVER,
				D3D11_COMPARISON_ALWAYS,
				D3D11_COMPARISON_EQUAL,
				D3D11_COMPARISON_NOT_EQUAL,
				D3D11_COMPARISON_LESS,
				D3D11_COMPARISON_LESS_EQUAL,
				D3D11_COMPARISON_GREATER,
				D3D11_COMPARISON_GREATER_EQUAL
			};
			assert( compare_mode >= 0 && compare_mode < video_compare_function_e_count_ );
			return map[ compare_mode ];
		}

		D3D11_FILL_MODE get_fill_mode( video_fill_type_e fill_mode )
		{
			static D3D11_FILL_MODE const map[ video_fill_type_e_count_ ] = {
				D3D11_FILL_WIREFRAME, // Direct3D 10 does not support points.
				D3D11_FILL_WIREFRAME,
				D3D11_FILL_SOLID
			};
			assert( fill_mode >= 0 && fill_mode < video_fill_type_e_count_ );
			return map[ fill_mode ];
		}

		D3D11_CULL_MODE get_cull_mode( video_cull_type_e cull_mode )
		{
			static D3D11_CULL_MODE const map[ video_cull_type_e_count_ ] = {
				D3D11_CULL_NONE,
				D3D11_CULL_FRONT,
				D3D11_CULL_BACK
			};
			assert( cull_mode >= 0 && cull_mode < video_cull_type_e_count_ );
			return map[ cull_mode ];
		}

		D3D11_FILTER get_texture_filter_mode( video_sample_filter_type_e sample_filter_mode, boolean_c with_comparison )
		{
			if ( with_comparison == false )
			{
				switch ( sample_filter_mode )
				{
					case ( video_sample_filter_type_e_point ):
						return D3D11_FILTER_MIN_MAG_MIP_POINT;
					case ( video_sample_filter_type_e_linear ):
						return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
					case ( video_sample_filter_type_e_anisotropic ):
						return D3D11_FILTER_ANISOTROPIC;
				}
			}
			else
			{
				switch ( sample_filter_mode )
				{
					case ( video_sample_filter_type_e_point ):
						return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
					case ( video_sample_filter_type_e_linear ):
						return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
					case ( video_sample_filter_type_e_anisotropic ):
						return D3D11_FILTER_COMPARISON_ANISOTROPIC;
				}
			}
			assert( false );
			return D3D11_FILTER_ANISOTROPIC;
		}

		D3D11_TEXTURE_ADDRESS_MODE get_texture_address_mode( video_sample_address_type_e sample_address_mode )
		{
			static D3D11_TEXTURE_ADDRESS_MODE const map[ video_sample_address_type_e_count_ ] = {
				D3D11_TEXTURE_ADDRESS_CLAMP,
				D3D11_TEXTURE_ADDRESS_WRAP
			};
			assert( sample_address_mode >= 0 && sample_address_mode < video_sample_address_type_e_count_ );
			return map[ sample_address_mode ];
		}

		D3D11_STENCIL_OP get_stencil_operation( video_stencil_operation_e stencil_operation )
		{
			static D3D11_STENCIL_OP const map[ video_stencil_operation_e_count_ ] = {
				D3D11_STENCIL_OP_KEEP,
				D3D11_STENCIL_OP_ZERO,
				D3D11_STENCIL_OP_REPLACE,
				D3D11_STENCIL_OP_INCR_SAT,
				D3D11_STENCIL_OP_DECR_SAT,
				D3D11_STENCIL_OP_INVERT,
				D3D11_STENCIL_OP_INCR,
				D3D11_STENCIL_OP_DECR
			};
			assert( stencil_operation >= 0 && stencil_operation < video_stencil_operation_e_count_ );
			return map[ stencil_operation ];
		}

		/*
		// creates a buffer resource with a shader resource view so it can be bound as a buffer to the shader.
		void_c create_buffer_with_srv(
			ID3D11Device * d3d11_device,
			ID3D11Buffer * * ppBuffer,
			ID3D11ShaderResourceView * * ppBufferSRV,
			sint32_c data_size )
		{
			assert( d3d11_device );
			assert( ppBuffer );
			assert( ppBufferSRV );
			assert( data_size );

			// make sure data matches up with float4 boundaries.
			sint32_c DataExcess = data_size % 16;
			assert( !DataExcess );

			HRESULT h_result;

			D3D11_BUFFER_DESC d3d11_buffer_description;
			d3d11_buffer_description.ByteWidth = data_size;
			d3d11_buffer_description.Usage = D3D11_USAGE_DYNAMIC;
			d3d11_buffer_description.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			d3d11_buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			d3d11_buffer_description.MiscFlags = 0;
			h_result = d3d11_device->CreateBuffer( &d3d11_buffer_description, NULL, ppBuffer );
			assert( SUCCEEDED( h_result ) );

			D3D11_SHADER_RESOURCE_VIEW_DESC d3d11_shader_resource_view_description;
			d3d11_shader_resource_view_description.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			d3d11_shader_resource_view_description.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			d3d11_shader_resource_view_description.Buffer.ElementOffset = 0;
			d3d11_shader_resource_view_description.Buffer.ElementWidth = data_size / 16;
			h_result = d3d11_device->CreateShaderResourceView( *ppBuffer, &d3d11_shader_resource_view_description, ppBufferSRV );
			assert( SUCCEEDED( h_result ) );
		}
		*/

		/*
		// creates a buffer resource that can be bound as a constant buffer to the shader.
		void_c create_constant_buffer(
			ID3D11Device * d3d11_device,
			ID3D11Buffer * * d3d11_buffer,
			sint32_c data_size )
		{
			assert( d3d11_device );
			assert( d3d11_buffer );
			assert( data_size );

			D3D11_BUFFER_DESC d3d11_buffer_description;
			d3d11_buffer_description.ByteWidth = data_size;
			d3d11_buffer_description.Usage = D3D11_USAGE_DYNAMIC;
			d3d11_buffer_description.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			d3d11_buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			d3d11_buffer_description.MiscFlags = 0;
			HRESULT h_result = d3d11_device->CreateBuffer( &d3d11_buffer_description, NULL, d3d11_buffer );
			assert( SUCCEEDED( h_result ) );
		}
		*/

		/*
		// fills a buffer with data.
		void_c fill_buffer(
			ID3D11Buffer * d3d11_buffer,
			void_c * data,
			sint32_c data_size )
		{
			ID3D11Device * d3d11_device = nullptr;
			d3d11_buffer->GetDevice( &d3d11_device );
			ID3D11DeviceContext * d3d11_device_context = nullptr;
			d3d11_device->GetImmediateContext( &d3d11_device_context );
			D3D11_MAPPED_SUBRESOURCE d3d11_mapped_subresource;
			assert( SUCCEEDED( d3d11_device_context->Map( d3d11_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3d11_mapped_subresource ) ) );
			ops::memory_copy( data, d3d11_mapped_subresource.pData, data_size );
			d3d11_device_context->Unmap( d3d11_buffer, 0 );
			d3d11_device_context->Release();
			d3d11_device->Release();
		}
		*/

		// creates a rasterizer state.
		boolean_c create_rasterizer_state(
			ID3D11Device * d3d11_device,
			ID3D11RasterizerState * * ppRasterizerState,
			video_cull_type_e cull_mode,
			video_fill_type_e fill_mode )
		{
			D3D11_RASTERIZER_DESC d3d11_rasterizer_description;
			d3d11_rasterizer_description.FillMode = get_fill_mode( fill_mode );
			d3d11_rasterizer_description.CullMode = get_cull_mode( cull_mode );
			d3d11_rasterizer_description.FrontCounterClockwise = TRUE;
			d3d11_rasterizer_description.DepthBias = 0;
			d3d11_rasterizer_description.DepthBiasClamp = 0.0f;
			d3d11_rasterizer_description.SlopeScaledDepthBias = 0.0f;
			d3d11_rasterizer_description.DepthClipEnable = TRUE;
			d3d11_rasterizer_description.ScissorEnable = FALSE;
			d3d11_rasterizer_description.MultisampleEnable = FALSE;
			d3d11_rasterizer_description.AntialiasedLineEnable = FALSE;
			HRESULT h_result = d3d11_device->CreateRasterizerState( &d3d11_rasterizer_description, ppRasterizerState );
			if ( FAILED( h_result ) )
			{
				debug_annoy( L"error", L"CreateRasterizerState failed." );
			}
			return SUCCEEDED( h_result );
		}

		// creates a depth stencil state.
		boolean_c create_depth_stencil_state(
			ID3D11Device * d3d11_device,
			ID3D11DepthStencilState * * d3d11_depth_stencil_state,
			boolean_c depth_test_enable,
			boolean_c depth_write_enable,
			video_compare_function_e depth_compare,
			boolean_c stencil_test_enable,
			uint8_c stencil_read_mask,
			uint8_c stencil_write_mask,
			video_compare_function_e stencil_back_face_compare_function,
			video_stencil_operation_e stencil_back_face_depth_fail_operation,
			video_stencil_operation_e stencil_back_face_pass_operation,
			video_stencil_operation_e stencil_back_face_fail_operation,
			video_compare_function_e stencil_front_face_compare_function,
			video_stencil_operation_e stencil_front_face_depth_fail_operation,
			video_stencil_operation_e stencil_front_face_pass_operation,
			video_stencil_operation_e stencil_front_face_fail_operation )
		{
			D3D11_DEPTH_STENCIL_DESC descDepthStencil;
			descDepthStencil.DepthEnable = depth_test_enable ? TRUE : FALSE;
			descDepthStencil.DepthWriteMask = depth_write_enable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
			descDepthStencil.DepthFunc = get_comparison_function( depth_compare );
			descDepthStencil.StencilEnable = stencil_test_enable ? TRUE : FALSE;
			descDepthStencil.StencilReadMask = stencil_read_mask;
			descDepthStencil.StencilWriteMask = stencil_write_mask;
			descDepthStencil.FrontFace.StencilFailOp = get_stencil_operation( stencil_front_face_fail_operation );
			descDepthStencil.FrontFace.StencilDepthFailOp = get_stencil_operation( stencil_front_face_depth_fail_operation );
			descDepthStencil.FrontFace.StencilPassOp = get_stencil_operation( stencil_front_face_pass_operation );
			descDepthStencil.FrontFace.StencilFunc = get_comparison_function( stencil_front_face_compare_function );
			descDepthStencil.BackFace.StencilFailOp = get_stencil_operation( stencil_back_face_fail_operation );
			descDepthStencil.BackFace.StencilDepthFailOp = get_stencil_operation( stencil_back_face_depth_fail_operation );
			descDepthStencil.BackFace.StencilPassOp = get_stencil_operation( stencil_back_face_pass_operation );
			descDepthStencil.BackFace.StencilFunc = get_comparison_function( stencil_back_face_compare_function );
			HRESULT h_result = d3d11_device->CreateDepthStencilState( &descDepthStencil, d3d11_depth_stencil_state );
			if ( FAILED( h_result ) )
			{
				debug_annoy( L"error", L"CreateDepthStencilState failed." );
			}
			return SUCCEEDED( h_result );
		}

		// creates a depth stencil state.
		boolean_c create_depth_stencil_state(
			ID3D11Device * d3d11_device,
			ID3D11DepthStencilState * * d3d11_depth_stencil_state,
			boolean_c depth_test_enable,
			boolean_c depth_write_enable,
			video_compare_function_e depth_compare )
		{
			return create_depth_stencil_state( d3d11_device, d3d11_depth_stencil_state, depth_test_enable, depth_write_enable, depth_compare, false, 0x00, 0x00, video_compare_function_e_never, video_stencil_operation_e_keep, video_stencil_operation_e_keep, video_stencil_operation_e_keep, video_compare_function_e_never, video_stencil_operation_e_keep, video_stencil_operation_e_keep, video_stencil_operation_e_keep );
		}

		// creates a make_matrix_blended state.
		boolean_c create_blend_state(
			ID3D11Device * d3d11_device,
			ID3D11BlendState * * d3d11_blend_state,
			video_blend_type_e make_matrix_blended )
		{
			D3D11_BLEND_DESC d3d11_blend_description;
			d3d11_blend_description.AlphaToCoverageEnable = FALSE;
			d3d11_blend_description.IndependentBlendEnable = FALSE;

			switch ( make_matrix_blended )
			{
				case video_blend_type_e_set:
					d3d11_blend_description.RenderTarget[ 0 ].BlendEnable = FALSE;
					d3d11_blend_description.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
					break;

				case video_blend_type_e_mix:
					d3d11_blend_description.RenderTarget[ 0 ].BlendEnable = TRUE;
					d3d11_blend_description.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_SRC_ALPHA;
					d3d11_blend_description.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
					d3d11_blend_description.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
					d3d11_blend_description.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE; // it's not often that we care to blend alpha, but this is how we do it, when rendering transparency to a texture that will be used as input to a later transparent pass.
					d3d11_blend_description.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ONE;
					d3d11_blend_description.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
					d3d11_blend_description.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
					break;

				case video_blend_type_e_add:
					d3d11_blend_description.RenderTarget[ 0 ].BlendEnable = TRUE;
					d3d11_blend_description.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_ONE;
					d3d11_blend_description.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_ONE;
					d3d11_blend_description.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
					d3d11_blend_description.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
					d3d11_blend_description.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;
					d3d11_blend_description.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
					d3d11_blend_description.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
					break;

				case video_blend_type_e_sub:
					d3d11_blend_description.RenderTarget[ 0 ].BlendEnable = TRUE;
					d3d11_blend_description.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_ONE;
					d3d11_blend_description.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_ONE;
					d3d11_blend_description.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_SUBTRACT;
					d3d11_blend_description.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
					d3d11_blend_description.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;
					d3d11_blend_description.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
					d3d11_blend_description.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
					break;

				case video_blend_type_e_min:
					d3d11_blend_description.RenderTarget[ 0 ].BlendEnable = TRUE;
					d3d11_blend_description.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_ONE;
					d3d11_blend_description.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_ONE;
					d3d11_blend_description.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_MIN;
					d3d11_blend_description.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
					d3d11_blend_description.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;
					d3d11_blend_description.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
					d3d11_blend_description.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
					break;

				case video_blend_type_e_max:
					d3d11_blend_description.RenderTarget[ 0 ].BlendEnable = TRUE;
					d3d11_blend_description.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_ONE;
					d3d11_blend_description.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_ONE;
					d3d11_blend_description.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_MAX;
					d3d11_blend_description.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
					d3d11_blend_description.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;
					d3d11_blend_description.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
					d3d11_blend_description.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
					break;
			}

			HRESULT h_result = d3d11_device->CreateBlendState( &d3d11_blend_description, d3d11_blend_state );
			if ( FAILED( h_result ) )
			{
				debug_annoy( L"error", L"CreateBlendState failed." );
			}
			return SUCCEEDED( h_result );
		}

		// creates a sampler state
		boolean_c create_sampler_state(
			ID3D11Device * d3d11_device,
			ID3D11SamplerState * * d3d11_sampler_state,
			video_sample_filter_type_e texture_filter_mode,
			video_sample_address_type_e texture_address_mode,
			video_compare_function_e compare_function )
		{
			D3D11_SAMPLER_DESC d3d11_sampler_description;
			d3d11_sampler_description.Filter = get_texture_filter_mode( texture_filter_mode, compare_function != video_compare_function_e_never );
			d3d11_sampler_description.AddressU = get_texture_address_mode( texture_address_mode );
			d3d11_sampler_description.AddressV = get_texture_address_mode( texture_address_mode );
			d3d11_sampler_description.AddressW = get_texture_address_mode( texture_address_mode );
			d3d11_sampler_description.MipLODBias = 0.0f;
			d3d11_sampler_description.MaxAnisotropy = 16;
			d3d11_sampler_description.ComparisonFunc = get_comparison_function( compare_function );
			d3d11_sampler_description.BorderColor[ 0 ] = 0.0f;
			d3d11_sampler_description.BorderColor[ 1 ] = 0.0f;
			d3d11_sampler_description.BorderColor[ 2 ] = 0.0f;
			d3d11_sampler_description.BorderColor[ 3 ] = 0.0f;
			d3d11_sampler_description.MinLOD = 0.0f;
			d3d11_sampler_description.MaxLOD = D3D11_FLOAT32_MAX;
			HRESULT h_result = d3d11_device->CreateSamplerState( &d3d11_sampler_description, d3d11_sampler_state );
			if ( FAILED( h_result ) )
			{
				debug_annoy( L"error", L"CreateSamplerState failed." );
			}
			return SUCCEEDED( h_result );
		}

		char8_c const * get_semantic_name( video_vertex_format_c::element_c::name_e name )
		{
			static char8_c const * map[ video_vertex_format_c::element_c::name_e_count_ ] =
			{
				nullptr,
				"POSITION",
				"NORMAL",
				"TANGENT",
				"BINORMAL",
				"TEXCOORD",
				"COLOR",
				"BLENDWEIGHT",
				"BLENDINDICES"
			};
			assert( name > 0 && name < video_vertex_format_c::element_c::name_e_count_ );
			return map[ name ];
		}

		const char8_c * get_semantic_name( video_vertex_format_c::element_c::name_e name, sint32_c index )
		{
			static char8_c const * map[ video_vertex_format_c::element_c::name_e_count_ * 4 ] =
			{
				nullptr,
				nullptr,
				nullptr,
				nullptr,
				"POSITION0",
				"POSITION1",
				"POSITION2",
				"POSITION3",
				"NORMAL0",
				"NORMAL1",
				"NORMAL2",
				"NORMAL3",
				"TANGENT0",
				"TANGENT1",
				"TANGENT2",
				"TANGENT3",
				"BINORMAL0",
				"BINORMAL1",
				"BINORMAL2",
				"BINORMAL3",
				"TEXCOORD0",
				"TEXCOORD1",
				"TEXCOORD2",
				"TEXCOORD3",
				"COLOR0",
				"COLOR1",
				"COLOR2",
				"COLOR3",
				"BLENDWEIGHT0",
				"BLENDWEIGHT1",
				"BLENDWEIGHT2",
				"BLENDWEIGHT3",
				"BLENDINDICES0",
				"BLENDINDICES1",
				"BLENDINDICES2",
				"BLENDINDICES3"
			};
			assert( name > 0 && name < video_vertex_format_c::element_c::name_e_count_ );
			assert( index >= 0 && index < 4 );
			return map[ name * 4 + index ];
		}
	}

	class CD3DInclude : public ID3DInclude
	{
	protected:
		video_shader_includer_c * _shader_includer;

	public:
		CD3DInclude( video_shader_includer_c * shader_includer )
		{
			_shader_includer = shader_includer;
		}

		virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Close( THIS_ LPCVOID pData ) override
		{
			if ( _shader_includer != 0 )
			{
				_shader_includer->close();
			}
			return S_OK;
		}

		virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Open( THIS_ D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes ) override
		{
			if ( _shader_includer != 0 )
			{
				void_c * source_code = nullptr;
				sint32_c source_code_size = 0;
				_shader_includer->open( pFileName, &source_code, &source_code_size );
				*ppData = static_cast< LPCVOID >( source_code );
				*pBytes = static_cast< UINT >( source_code_size );
			}
			return S_OK;
		}

	};

	class video_vertex_buffer_d3d11_c : public video_vertex_buffer_c
	{
	protected:
		friend class video_interface_d3d11_c;
		video_vertex_format_c _vertex_format;
		sint32_c _vertex_count;
		boolean_c _set_data_enable;
		boolean_c _get_data_enable;
		boolean_c _target_enable;
		ID3D11Buffer * _d3d11_buffer;
		sint32_c _size;

	protected:
		video_vertex_buffer_d3d11_c( video_interface_c * video_interface, ID3D11Device * d3d11_device, video_vertex_format_c const * vertex_format, sint32_c vertex_count, void_c * data, sint32_c data_size, boolean_c set_data_enable, boolean_c get_data_enable, boolean_c target_enable )
			: video_vertex_buffer_c( video_interface )
			, _vertex_format( *vertex_format )
			, _vertex_count( vertex_count )
			, _set_data_enable( false )
			, _get_data_enable( false )
			, _target_enable( false )
			, _d3d11_buffer( nullptr )
			, _size( 0 )
		{
			// check stuff
			assert( vertex_format );
			assert( vertex_format->elements_count >= 1 && vertex_format->elements_count <= 8 );
			assert( vertex_format->stride > 0 && vertex_format->stride <= 128 );
			assert( vertex_count > 0 );
			if ( set_data_enable == true )
			{
				assert( get_data_enable == false );
				assert( target_enable == false );
			}
			if ( get_data_enable == true )
			{
				assert( set_data_enable == false );
				assert( target_enable == false );
			}
			if ( target_enable == true )
			{
				assert( set_data_enable == false );
				assert( get_data_enable == false );
			}

			// copy stuff
			_size = _vertex_format.stride * vertex_count;
			_set_data_enable = set_data_enable;
			_get_data_enable = get_data_enable;
			_target_enable = target_enable;
			if ( set_data_enable == false && target_enable == false )
			{
				assert( data );
				assert( data_size == _size );
			}

			// create buffer description
			D3D11_BUFFER_DESC d3d11_buffer_description;
			d3d11_buffer_description.ByteWidth = _size;
			d3d11_buffer_description.Usage = D3D11_USAGE_IMMUTABLE;
			d3d11_buffer_description.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			d3d11_buffer_description.CPUAccessFlags = 0;
			d3d11_buffer_description.MiscFlags = 0;
			if ( set_data_enable )
			{
				d3d11_buffer_description.Usage = D3D11_USAGE_DYNAMIC;
				d3d11_buffer_description.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
			}
			if ( get_data_enable )
			{
				d3d11_buffer_description.Usage = D3D11_USAGE_STAGING;
				d3d11_buffer_description.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
			}
			if ( target_enable )
			{
				d3d11_buffer_description.Usage = D3D11_USAGE_DEFAULT;
				d3d11_buffer_description.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
			}

			// create data description
			D3D11_SUBRESOURCE_DATA d3d11_sub_resource_data_description;
			d3d11_sub_resource_data_description.pSysMem = data;
			d3d11_sub_resource_data_description.SysMemPitch = 0;
			d3d11_sub_resource_data_description.SysMemSlicePitch = 0;

			// create buffer
			HRESULT h_result = d3d11_device->CreateBuffer( &d3d11_buffer_description, ( data ? &d3d11_sub_resource_data_description : NULL ), &_d3d11_buffer );
			assert( SUCCEEDED( h_result ) );
		}

	public:
		virtual ~video_vertex_buffer_d3d11_c() override
		{
			com_safe_release( _d3d11_buffer );
		}

		virtual video_vertex_format_c const * get_vertex_format() const override
		{
			return &_vertex_format;
		}

		virtual sint32_c get_vertex_count() const override
		{
			return _vertex_count;
		}

		virtual void_c set_data( void_c * data, sint32_c data_size ) const override
		{
			assert( _set_data_enable == true );
			assert( data != 0 );
			assert( data_size <= _size );
			ID3D11Device * d3d11_device = nullptr;
			_d3d11_buffer->GetDevice( &d3d11_device );
			ID3D11DeviceContext * d3d11_device_context = nullptr;
			d3d11_device->GetImmediateContext( &d3d11_device_context );
			D3D11_MAPPED_SUBRESOURCE d3d11_mapped_subresource = {};
			assert( SUCCEEDED( d3d11_device_context->Map( _d3d11_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3d11_mapped_subresource ) ) );
			ops::memory_copy( data, d3d11_mapped_subresource.pData, data_size );
			d3d11_device_context->Unmap( _d3d11_buffer, 0 );
			d3d11_device_context->Release();
			d3d11_device->Release();
		}

	};

	class video_index_buffer_d3d11_c : public video_index_buffer_c
	{
	protected:
		friend class video_interface_d3d11_c;
		video_index_format_e _video_index_format;
		sint32_c _index_count;
		sint32_c _size;
		boolean_c _set_data_enable;
		ID3D11Buffer * _d3d11_buffer;
		DXGI_FORMAT _dxgi_format;

	protected:
		video_index_buffer_d3d11_c( video_interface_c * video_interface, ID3D11Device * d3d11_device, video_index_format_e index_format, sint32_c index_count, void_c * data, sint32_c data_size, boolean_c set_data_enable, boolean_c get_data_enable )
			: video_index_buffer_c( video_interface )
			, _video_index_format( video_index_format_e_none )
			, _index_count( 0 )
			, _size( 0 )
			, _set_data_enable( false )
			, _d3d11_buffer( nullptr )
			, _dxgi_format( DXGI_FORMAT_UNKNOWN )
		{
			// check stuff
			assert( index_format > video_index_format_e_none && index_format < video_index_format_e_count_ );
			assert( index_count > 0 );
			if ( set_data_enable == true )
			{
				assert( get_data_enable == false );
			}
			if ( get_data_enable == true )
			{
				assert( set_data_enable == false );
			}

			// copy stuff
			_video_index_format = index_format;
			_index_count = index_count;
			_size = video_get_index_format_size( index_format ) * index_count;
			_set_data_enable = set_data_enable;
			if ( set_data_enable == false )
			{
				assert( data );
				assert( data_size == _size );
			}
			_dxgi_format = d3d11_helpers::get_format( index_format );

			// create buffer description
			D3D11_BUFFER_DESC d3d11_buffer_description;
			d3d11_buffer_description.ByteWidth = _size;
			d3d11_buffer_description.Usage = D3D11_USAGE_IMMUTABLE;
			d3d11_buffer_description.CPUAccessFlags = 0;
			if ( set_data_enable )
			{
				d3d11_buffer_description.Usage = D3D11_USAGE_DYNAMIC;
				d3d11_buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			}
			if ( get_data_enable )
			{
				d3d11_buffer_description.Usage = D3D11_USAGE_STAGING;
				d3d11_buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			}
			d3d11_buffer_description.BindFlags = D3D11_BIND_INDEX_BUFFER;
			d3d11_buffer_description.MiscFlags = 0;

			// create data description
			D3D11_SUBRESOURCE_DATA d3d11_sub_resource_data_description;
			d3d11_sub_resource_data_description.pSysMem = data;
			d3d11_sub_resource_data_description.SysMemPitch = 0;
			d3d11_sub_resource_data_description.SysMemSlicePitch = 0;

			// create buffer
			HRESULT h_result = d3d11_device->CreateBuffer( &d3d11_buffer_description, ( data ? &d3d11_sub_resource_data_description : NULL ), &_d3d11_buffer );
			assert( SUCCEEDED( h_result ) );
		}

	public:
		virtual ~video_index_buffer_d3d11_c() override
		{
			com_safe_release( _d3d11_buffer );
		}

		virtual video_index_format_e get_index_format() override
		{
			return _video_index_format;
		}

		virtual sint32_c get_index_count() override
		{
			return _index_count;
		}

		virtual void_c set_data( void_c * data, sint32_c data_size ) override
		{
			assert( _set_data_enable == true );
			assert( data != 0 );
			assert( data_size <= _size );
			ID3D11Device * d3d11_device = nullptr;
			_d3d11_buffer->GetDevice( &d3d11_device );
			ID3D11DeviceContext * d3d11_device_context = nullptr;
			d3d11_device->GetImmediateContext( &d3d11_device_context );
			D3D11_MAPPED_SUBRESOURCE d3d11_mapped_subresource = {};
			assert( SUCCEEDED( d3d11_device_context->Map( _d3d11_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3d11_mapped_subresource ) ) );
			ops::memory_copy( data, d3d11_mapped_subresource.pData, data_size );
			d3d11_device_context->Unmap( _d3d11_buffer, 0 );
			d3d11_device_context->Release();
			d3d11_device->Release();
		}
	};

	class video_constant_buffer_d3d11_c : public video_constant_buffer_c
	{
	protected:
		friend class video_interface_d3d11_c;
		sint32_c _size;
		ID3D11Buffer * _d3d11_buffer;

	protected:
		video_constant_buffer_d3d11_c( video_interface_c * video_interface, ID3D11Device * d3d11_device, sint32_c size )
			: video_constant_buffer_c( video_interface )
			, _size( 0 )
			, _d3d11_buffer( nullptr )
		{
			// check stuff
			assert( size % 16 == 0 ); // constant buffers must be multiples of 16 bytes in size

			// copy stuff
			_size = size;

			// create buffer description
			D3D11_BUFFER_DESC d3d11_buffer_description;
			d3d11_buffer_description.ByteWidth = size;
			d3d11_buffer_description.Usage = D3D11_USAGE_DYNAMIC;
			d3d11_buffer_description.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			d3d11_buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			d3d11_buffer_description.MiscFlags = 0;

			// create buffer
			HRESULT h_result = d3d11_device->CreateBuffer( &d3d11_buffer_description, NULL, &_d3d11_buffer );
			assert( SUCCEEDED( h_result ) );
		}

	public:
		virtual ~video_constant_buffer_d3d11_c() override
		{
			com_safe_release( _d3d11_buffer );
		}

		virtual void_c set_data( void_c * data, sint32_c data_size ) const override
		{
			assert( data );
			assert( data_size <= _size );
			ID3D11Device * d3d11_device = nullptr;
			_d3d11_buffer->GetDevice( &d3d11_device );
			ID3D11DeviceContext * d3d11_device_context = nullptr;
			d3d11_device->GetImmediateContext( &d3d11_device_context );
			D3D11_MAPPED_SUBRESOURCE d3d11_mapped_subresource = {};
			assert( SUCCEEDED( d3d11_device_context->Map( _d3d11_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3d11_mapped_subresource ) ) );
			ops::memory_copy( data, d3d11_mapped_subresource.pData, data_size );
			d3d11_device_context->Unmap( _d3d11_buffer, 0 );
			d3d11_device_context->Release();
			d3d11_device->Release();
		}
	};

	class video_texture_buffer_d3d11_c : public video_texture_buffer_c
	{
	protected:
		friend class video_interface_d3d11_c;
		sint32_c _size;
		ID3D11Buffer * _d3d11_buffer;
		ID3D11ShaderResourceView * _d3d11_buffer_srv;

	protected:
		video_texture_buffer_d3d11_c( video_interface_c * video_interface, ID3D11Device * d3d11_device, sint32_c size )
			: video_texture_buffer_c( video_interface )
			, _size( 0 )
			, _d3d11_buffer( nullptr )
			, _d3d11_buffer_srv( nullptr )
		{
			// check stuff
			assert( size % 16 == 0 ); // constant buffers must be multiples of 16 bytes in size

			// copy stuff
			_size = size;

			// create buffer description
			D3D11_BUFFER_DESC d3d11_buffer_description;
			d3d11_buffer_description.ByteWidth = size;
			d3d11_buffer_description.Usage = D3D11_USAGE_DYNAMIC;
			d3d11_buffer_description.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			d3d11_buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			d3d11_buffer_description.MiscFlags = 0;

			// create buffer
			HRESULT h_result = d3d11_device->CreateBuffer( &d3d11_buffer_description, NULL, &_d3d11_buffer );
			assert( SUCCEEDED( h_result ) );

			// create shader resource view description
			D3D11_SHADER_RESOURCE_VIEW_DESC d3d11_shader_resource_view_description;
			d3d11_shader_resource_view_description.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			d3d11_shader_resource_view_description.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			d3d11_shader_resource_view_description.Buffer.ElementOffset = 0;
			d3d11_shader_resource_view_description.Buffer.ElementWidth = size / 16;

			// create shader resource view
			h_result = d3d11_device->CreateShaderResourceView( _d3d11_buffer, &d3d11_shader_resource_view_description, &_d3d11_buffer_srv );
			assert( SUCCEEDED( h_result ) );
		}

	public:
		virtual ~video_texture_buffer_d3d11_c() override
		{
			com_safe_release( _d3d11_buffer_srv );
			com_safe_release( _d3d11_buffer );
		}

		virtual void_c set_data( void_c * data, sint32_c data_size ) const override
		{
			assert( data );
			assert( data_size <= _size );
			ID3D11Device * d3d11_device = nullptr;
			_d3d11_buffer->GetDevice( &d3d11_device );
			ID3D11DeviceContext * d3d11_device_context = nullptr;
			d3d11_device->GetImmediateContext( &d3d11_device_context );
			D3D11_MAPPED_SUBRESOURCE d3d11_mapped_subresource = {};
			assert( SUCCEEDED( d3d11_device_context->Map( _d3d11_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3d11_mapped_subresource ) ) );
			ops::memory_copy( data, d3d11_mapped_subresource.pData, data_size );
			d3d11_device_context->Unmap( _d3d11_buffer, 0 );
			d3d11_device_context->Release();
			d3d11_device->Release();
		}
	};

	class video_texture_d3d11_c : public video_texture_c
	{
	protected:
		friend class video_interface_d3d11_c;
		friend class video_output_d3d11_c;
		friend class video_depth_stencil_d3d11_c;
		video_texture_format_e _texture_format; // pixel format of pixels (aka texels).
		sint32_c _width; // width of textre in pixels (aka texels).
		sint32_c _height; // height of texture in pixels (aka texels).
		sint32_c _multi_sample_count; // if _multi_sample_count == 1 then there is no multi_sample_count. if _multi_sample_count > 1 then there is multi_sample_count. _multi_sample_count can not == 0.
		sint32_c _array_slice_count; // number of texture array slices. if _count == 0 then Texture2D is used. if _count > 0 then texture is used.
		boolean_c _set_data_enable; // if true then the data of the texture on the GPU can be written by the CPU.
		boolean_c _get_data_enable; // if true then the data of the texture on the GPU can be read by the CPU.
		boolean_c _target_enable; // if true then the texture can be rendered to by the GPU.
		boolean_c _mip_enable; // if true then mip levels will be allocated in the texture.
		sint32_c _mip_level_count; // number of mips, calculated from width and height.
		sint32_c _size; // total size in bytes of texture data. includes msaa, mip levels, and array slices.
		ID3D11Texture2D * _d3d11_texture2d; // base texture allocation.
		ID3D11RenderTargetView * _d3d11_texture2d_rtv; // Texture2D or Texture2DMS render target view of first slice.
		ID3D11RenderTargetView * _d3d11_texture2darray_rtv; // Texture2DArray or Texture2DMSArray render target view of texture array, for rendering to all slices simultaneously.
		ID3D11RenderTargetView * * _d3d11_texture2darray_slice_rtvs; // Texture2DArray or Texture2DMSArray render target views, but each one is just 1 slice long, for rendering to each slice separately.
		ID3D11ShaderResourceView * _d3d11_texture2d_srv; // Texture2D or Texture2DMS shader resource view of first slice.
		ID3D11ShaderResourceView * _d3d11_texture2darray_srv; // Texture2DArray or Texture2DMSArray shader resource view of all slices.
		ID3D11ShaderResourceView * _d3d11_texturecube_srv; // TextureCube shader resource view of first multiple of 6 slices.
		ID3D11ShaderResourceView * _d3d11_texturecubearray_srv; // TextureCubeArray shader resource view of all multiple of 6 slices.

	protected:
		video_texture_d3d11_c( video_interface_c * video_interface )
			: video_texture_c( video_interface )
			, _texture_format( video_texture_format_e_none )
			, _width( 0 )
			, _height( 0 )
			, _multi_sample_count( 0 )
			, _array_slice_count( 0 )
			, _set_data_enable( false )
			, _get_data_enable( false )
			, _target_enable( false )
			, _mip_enable( false )
			, _mip_level_count( 0 )
			, _size( 0 )
			, _d3d11_texture2d( nullptr )
			, _d3d11_texture2d_rtv( nullptr )
			, _d3d11_texture2darray_rtv( nullptr )
			, _d3d11_texture2darray_slice_rtvs( nullptr )
			, _d3d11_texture2d_srv( nullptr )
			, _d3d11_texture2darray_srv( nullptr )
			, _d3d11_texturecube_srv( nullptr )
			, _d3d11_texturecubearray_srv( nullptr )
		{
		}

	public:
		virtual ~video_texture_d3d11_c() override
		{
			com_safe_release( _d3d11_texturecubearray_srv );
			com_safe_release( _d3d11_texturecube_srv );
			com_safe_release( _d3d11_texture2darray_srv );
			com_safe_release( _d3d11_texture2d_srv );
			if ( _d3d11_texture2darray_slice_rtvs )
			{
				for ( sint32_c i = 0; i < _array_slice_count; i++ )
				{
					com_safe_release( _d3d11_texture2darray_slice_rtvs[ i ] );
				}
				delete[] _d3d11_texture2darray_slice_rtvs;
				_d3d11_texture2darray_slice_rtvs = nullptr;
			}
			com_safe_release( _d3d11_texture2darray_rtv );
			com_safe_release( _d3d11_texture2d_rtv );
			com_safe_release( _d3d11_texture2d );
		}

		void_c start( ID3D11Device * d3d11_device, video_texture_format_e texture_format, sint32_c width, sint32_c height, sint32_c multi_sample_count, sint32_c array_slice_count, void_c const * top_mip_level_data, sint32_c top_mip_level_data_size, boolean_c set_data_enable, boolean_c get_data_enable, boolean_c target_enable, boolean_c mip_enable )
		{
			// validate current state and inputs.
			assert( _texture_format == video_texture_format_e_none );
			assert( texture_format > video_texture_format_e_none && texture_format < video_texture_format_e_count_ );
			assert( width > 0 );
			assert( height > 0 );
			assert( multi_sample_count == 1 || multi_sample_count == 2 || multi_sample_count == 4 || multi_sample_count == 8 );
			assert( array_slice_count > 0 );
			assert( !top_mip_level_data == !top_mip_level_data_size ); // exclusive or, both must be set or neither must be set.
			if ( mip_enable )
			{
				assert( multi_sample_count == 1 ); // mip maps are not compatible with multi sample.
			}
			if ( set_data_enable == true )
			{
				assert( get_data_enable == false ); // set data is not compatible with get data.
				assert( target_enable == false ); // set data is not compatible with render target.
				assert( array_slice_count == 1 ); // can't set data on texture arrays.
			}
			if ( get_data_enable == true )
			{
				assert( set_data_enable == false ); // get data is not compatible with set data.
				assert( target_enable == false ); // get data is not compatible with render target.
			}
			if ( target_enable == true )
			{
				assert( set_data_enable == false ); // set data is not compatible with render target.
				assert( get_data_enable == false ); // get data is not compatible with render target.
			}

			// copy inputs.
			_texture_format = texture_format;
			_width = width;
			_height = height;
			_multi_sample_count = multi_sample_count;
			_array_slice_count = array_slice_count;
			_set_data_enable = set_data_enable;
			_get_data_enable = get_data_enable;
			_target_enable = target_enable;
			_mip_enable = mip_enable;

			// find out how many mips we need to get down to 1x1.
			_mip_level_count = mip_enable ? 1 + static_cast< sint32_c >( ops::math_round_down( ops::math_natural_logarithm( ops::math_maximum( static_cast< float32_c >( width ), static_cast< float32_c >( height ) ) ) / ops::math_natural_logarithm( 2.0f ) ) ) : 1; // log2(x) { return log(x) / log(2); }

			// calculate total texture size.
			for ( sint32_c i = 0; i < _mip_level_count; i++ )
			{
				_size += width * height * multi_sample_count * video_texture_format_size_get( texture_format );
				width /= 2;
				if ( width < 1 )
				{
					width = 1;
				}
				height /= 2;
				if ( height < 1 )
				{
					height = 1;
				}
			}
			_size *= array_slice_count;
			width = _width;
			height = _height;

			// create texture description.
			D3D11_TEXTURE2D_DESC d3d11_texture2d_description = {};
			d3d11_texture2d_description.Width = width;
			d3d11_texture2d_description.Height = height;
			d3d11_texture2d_description.MipLevels = _mip_level_count;
			d3d11_texture2d_description.ArraySize = array_slice_count;
			d3d11_texture2d_description.Format = d3d11_helpers::get_format( texture_format );
			d3d11_texture2d_description.SampleDesc.Count = multi_sample_count;
			d3d11_texture2d_description.SampleDesc.Quality = multi_sample_count == 1 ? 0 : D3D11_STANDARD_MULTISAMPLE_PATTERN;
			d3d11_texture2d_description.Usage = D3D11_USAGE_IMMUTABLE;
			d3d11_texture2d_description.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			d3d11_texture2d_description.CPUAccessFlags = 0;
			d3d11_texture2d_description.MiscFlags = ( mip_enable ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0 ) | ( array_slice_count % 6 == 0 ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0 );
			//if ( target_enable == true && multi_sample_count == 1 && texture_format == video_texture_format_e_B8G8R8A8_UNORM )
			//{
			//	desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED; // hack! ugh i forgot what this was even for. it probably had something to do with getting the engine to run in the editor which was using WPF. now i think we're using a .NET control hosted inside of a WPF control, so we don't have to jump through this hoop any more.
			//}
			if ( set_data_enable )
			{
				d3d11_texture2d_description.Usage = D3D11_USAGE_DYNAMIC;
				d3d11_texture2d_description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			}
			if ( get_data_enable )
			{
				d3d11_texture2d_description.Usage = D3D11_USAGE_STAGING;
				d3d11_texture2d_description.BindFlags = 0;
				d3d11_texture2d_description.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			}
			if ( target_enable )
			{
				d3d11_texture2d_description.Usage = D3D11_USAGE_DEFAULT;
				d3d11_texture2d_description.BindFlags |= D3D11_BIND_RENDER_TARGET;
			}
			if ( mip_enable )
			{
				d3d11_texture2d_description.Usage = D3D11_USAGE_DEFAULT;
				d3d11_texture2d_description.BindFlags |= D3D11_BIND_RENDER_TARGET;
				d3d11_texture2d_description.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
			}
			if ( !set_data_enable && !get_data_enable && !target_enable && !mip_enable )
			{
				d3d11_texture2d_description.Usage = D3D11_USAGE_DEFAULT;
			}

			// create subresource descriptions (one for each mip level, or one for each array slice).
			// all of the mip levels after 0 are dummies, since Direct3D wants us to allocate them in order for automatic mip map generation to work.
			// we'll upload the top mip level then tell Direct3D to generate the rest of the mip levels on its own.
			D3D11_SUBRESOURCE_DATA * d3d11_sub_resource_data_description_array = nullptr;
			boolean_c top_mip_level_data_is_required = ( multi_sample_count == 1 && set_data_enable == false && get_data_enable == false && target_enable == false && ( top_mip_level_data == nullptr || top_mip_level_data_size == 0 ) );
			sint32_c top_mip_level_data_size_expected = width * height * video_texture_format_size_get( texture_format ) * array_slice_count;
			char8_c * our_top_mip_level_data = nullptr;
			if ( top_mip_level_data_is_required )
			{
				assert( multi_sample_count == 1 );
				if ( top_mip_level_data == nullptr )
				{
					top_mip_level_data_size = top_mip_level_data_size_expected;
					our_top_mip_level_data = new char8_c[ top_mip_level_data_size ];
					ops::memory_zero( our_top_mip_level_data, top_mip_level_data_size );
					top_mip_level_data = our_top_mip_level_data;
				}
			}
			if ( top_mip_level_data )
			{
				assert( top_mip_level_data_size == top_mip_level_data_size_expected );
				d3d11_sub_resource_data_description_array = new D3D11_SUBRESOURCE_DATA[ array_slice_count * _mip_level_count ];
				char8_c const * top_mip_level_data_for_current_array_slice = reinterpret_cast< char8_c const * >( top_mip_level_data );
				sint32_c k = 0;
				for ( sint32_c i = 0; i < array_slice_count; i++ )
				{
					for ( sint32_c j = 0; j < _mip_level_count; j++ )
					{
						d3d11_sub_resource_data_description_array[ k ].pSysMem = top_mip_level_data_for_current_array_slice; // for the top mip level this will be correct. for subsequent mip levels this will be garbage, but we need to set something because D3D expects it. this is okay though because subsequent mips will be generated from the top level mip later via a call to GenerateMips().
						d3d11_sub_resource_data_description_array[ k ].SysMemPitch = video_texture_format_size_get( texture_format ) * width;
						d3d11_sub_resource_data_description_array[ k ].SysMemSlicePitch = 0;
						width /= 2;
						if ( width < 1 )
						{
							width = 1;
						}
						k++;
					}
					width = _width;
					top_mip_level_data_for_current_array_slice += ( width * height );
				}
			}

			// create the texture.
			assert( SUCCEEDED( d3d11_device->CreateTexture2D( &d3d11_texture2d_description, d3d11_sub_resource_data_description_array, &_d3d11_texture2d ) ) );

			// release memory
			if ( d3d11_sub_resource_data_description_array )
			{
				delete[] d3d11_sub_resource_data_description_array;
				d3d11_sub_resource_data_description_array = nullptr;
			}
			if ( our_top_mip_level_data )
			{
				delete[] our_top_mip_level_data;
				our_top_mip_level_data = nullptr;
				top_mip_level_data_size = 0;
			}

			if ( d3d11_texture2d_description.BindFlags &D3D11_BIND_SHADER_RESOURCE )
			{
				// create Texture2D or Texture2DMS shader resource views for the first slice.
				// it is not possible to create one Texture2D or Texture2DMS shader resource view for each slice.
				// it is possible to create one Texture2DArray or Texture2DMSArray shader resource view for each slice, but it's kind of pointless, each one would be 1 slice long and the hlsl would need to supply an array slice index each time it samples.
				// it's possible but it's a lot of effort.
				{
					if ( _multi_sample_count == 1 )
					{
						// Texture2D.
						D3D11_SHADER_RESOURCE_VIEW_DESC d3d11_shader_resource_view_description = {};
						d3d11_shader_resource_view_description.Format = d3d11_texture2d_description.Format;
						d3d11_shader_resource_view_description.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
						d3d11_shader_resource_view_description.Texture2D.MipLevels = d3d11_texture2d_description.MipLevels;
						d3d11_shader_resource_view_description.Texture2D.MostDetailedMip = 0;
						assert( SUCCEEDED( d3d11_device->CreateShaderResourceView( _d3d11_texture2d, &d3d11_shader_resource_view_description, &_d3d11_texture2d_srv ) ) );
					}
					else
					{
						// Texture2DMS.
						D3D11_SHADER_RESOURCE_VIEW_DESC d3d11_shader_resource_view_description = {};
						d3d11_shader_resource_view_description.Format = d3d11_texture2d_description.Format;
						d3d11_shader_resource_view_description.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
						assert( SUCCEEDED( d3d11_device->CreateShaderResourceView( _d3d11_texture2d, &d3d11_shader_resource_view_description, &_d3d11_texture2d_srv ) ) );
					}

					if ( _multi_sample_count == 1 )
					{
						// Texture2DArray.
						D3D11_SHADER_RESOURCE_VIEW_DESC d3d11_shader_resource_view_description = {};
						d3d11_shader_resource_view_description.Format = d3d11_texture2d_description.Format;
						d3d11_shader_resource_view_description.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
						d3d11_shader_resource_view_description.Texture2DArray.MipLevels = d3d11_texture2d_description.MipLevels;
						d3d11_shader_resource_view_description.Texture2DArray.MostDetailedMip = 0;
						d3d11_shader_resource_view_description.Texture2DArray.FirstArraySlice = 0;
						d3d11_shader_resource_view_description.Texture2DArray.ArraySize = array_slice_count;
						assert( SUCCEEDED( d3d11_device->CreateShaderResourceView( _d3d11_texture2d, &d3d11_shader_resource_view_description, &_d3d11_texture2darray_srv ) ) );
					}
					else
					{
						// Texture2DMSArray.
						D3D11_SHADER_RESOURCE_VIEW_DESC d3d11_shader_resource_view_description = {};
						d3d11_shader_resource_view_description.Format = d3d11_texture2d_description.Format;
						d3d11_shader_resource_view_description.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
						d3d11_shader_resource_view_description.Texture2DMSArray.FirstArraySlice = 0;
						d3d11_shader_resource_view_description.Texture2DMSArray.ArraySize = array_slice_count;
						assert( SUCCEEDED( d3d11_device->CreateShaderResourceView( _d3d11_texture2d, &d3d11_shader_resource_view_description, &_d3d11_texture2darray_srv ) ) );
					}
				}

				if ( _multi_sample_count == 1 && _array_slice_count % 6 == 0 )
				{
					{
						// TextureCube.
						D3D11_SHADER_RESOURCE_VIEW_DESC d3d11_shader_resource_view_description = {};
						d3d11_shader_resource_view_description.Format = d3d11_texture2d_description.Format;
						d3d11_shader_resource_view_description.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
						d3d11_shader_resource_view_description.TextureCube.MostDetailedMip = 0;
						d3d11_shader_resource_view_description.TextureCube.MipLevels = d3d11_texture2d_description.MipLevels;
						assert( SUCCEEDED( d3d11_device->CreateShaderResourceView( _d3d11_texture2d, &d3d11_shader_resource_view_description, &_d3d11_texturecube_srv ) ) );
					}
					{
						// TextureCubeArray.
						D3D11_SHADER_RESOURCE_VIEW_DESC d3d11_shader_resource_view_description = {};
						d3d11_shader_resource_view_description.Format = d3d11_texture2d_description.Format;
						d3d11_shader_resource_view_description.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
						d3d11_shader_resource_view_description.TextureCubeArray.MostDetailedMip = 0;
						d3d11_shader_resource_view_description.TextureCubeArray.MipLevels = d3d11_texture2d_description.MipLevels;
						d3d11_shader_resource_view_description.TextureCubeArray.First2DArrayFace = 0;
						d3d11_shader_resource_view_description.TextureCubeArray.NumCubes = array_slice_count / 6;
						assert( SUCCEEDED( d3d11_device->CreateShaderResourceView( _d3d11_texture2d, &d3d11_shader_resource_view_description, &_d3d11_texturecubearray_srv ) ) );
					}
				}
			}

			if ( target_enable )
			{
				{
					D3D11_RENDER_TARGET_VIEW_DESC d3d11_render_target_view_description = {};
					d3d11_render_target_view_description.Format = d3d11_texture2d_description.Format;
					if ( multi_sample_count == 1 )
					{
						// Texture2D.
						d3d11_render_target_view_description.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
						d3d11_render_target_view_description.Texture2D.MipSlice = 0;
					
					}
					else
					{
						// Texture2DMS.
						d3d11_render_target_view_description.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
					}
					assert( SUCCEEDED( d3d11_device->CreateRenderTargetView( _d3d11_texture2d, &d3d11_render_target_view_description, &_d3d11_texture2d_rtv ) ) );
				}

				{
					D3D11_RENDER_TARGET_VIEW_DESC d3d11_render_target_view_description = {};
					d3d11_render_target_view_description.Format = d3d11_texture2d_description.Format;
					if ( multi_sample_count == 1 )
					{
						// Texture2DArray.
						d3d11_render_target_view_description.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
						d3d11_render_target_view_description.Texture2DArray.MipSlice = 0;
						d3d11_render_target_view_description.Texture2DArray.FirstArraySlice = 0;
						d3d11_render_target_view_description.Texture2DArray.ArraySize = _array_slice_count;
					}
					else
					{
						// Texture2DMSArray.
						d3d11_render_target_view_description.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
						d3d11_render_target_view_description.Texture2DMSArray.FirstArraySlice = 0;
						d3d11_render_target_view_description.Texture2DMSArray.ArraySize = _array_slice_count;
					}
					assert( SUCCEEDED( d3d11_device->CreateRenderTargetView( _d3d11_texture2d, &d3d11_render_target_view_description, &_d3d11_texture2darray_rtv ) ) );
				}

				_d3d11_texture2darray_slice_rtvs = new ID3D11RenderTargetView * [ _array_slice_count ];
				for ( sint32_c i = 0; i < _array_slice_count; i++ )
				{
					_d3d11_texture2darray_slice_rtvs[ i ] = nullptr;
					D3D11_RENDER_TARGET_VIEW_DESC d3d11_render_target_view_description = {};
					d3d11_render_target_view_description.Format = d3d11_texture2d_description.Format;
					if ( multi_sample_count == 1 )
					{
						// Texture2DArray.
						d3d11_render_target_view_description.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
						d3d11_render_target_view_description.Texture2DArray.MipSlice = 0;
						d3d11_render_target_view_description.Texture2DArray.FirstArraySlice = i;
						d3d11_render_target_view_description.Texture2DArray.ArraySize = 1;
					}
					else
					{
						// Texture2DMSArray.
						d3d11_render_target_view_description.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
						d3d11_render_target_view_description.Texture2DMSArray.FirstArraySlice = i;
						d3d11_render_target_view_description.Texture2DMSArray.ArraySize = 1;
					}
					assert( SUCCEEDED( d3d11_device->CreateRenderTargetView( _d3d11_texture2d, &d3d11_render_target_view_description, &_d3d11_texture2darray_slice_rtvs[ i ] ) ) );
				}
			}

			// generate mip maps
			if ( mip_enable && top_mip_level_data )
			{
				ID3D11DeviceContext * d3d11_device_context = nullptr;
				d3d11_device->GetImmediateContext( &d3d11_device_context );
				d3d11_device_context->GenerateMips( _d3d11_texture2d_srv );
				d3d11_device_context->Release();
			}
		}

		virtual video_texture_format_e get_texture_format() const override
		{
			return _texture_format;
		}

		virtual sint32_c get_width() const override
		{
			return _width;
		}

		virtual sint32_c get_height() const override
		{
			return _height;
		}

		virtual sint32_c get_multi_sample_count() const override
		{
			return _multi_sample_count;
		}

		virtual sint32_c get_array_slice_count() const override
		{
			return _array_slice_count;
		}

		virtual sint32_c get_mip_level_count() const override
		{
			return _mip_level_count;
		}

		virtual void_c set_data( void_c * top_mip_level_data, sint32_c top_mip_level_data_size ) const override
		{
			assert( _set_data_enable == true );
			assert( top_mip_level_data );
			assert( top_mip_level_data_size == _width * _height * video_texture_format_size_get( _texture_format ) );
			ID3D11Device * d3d11_device = nullptr;
			_d3d11_texture2d->GetDevice( &d3d11_device );
			ID3D11DeviceContext * d3d11_device_context = nullptr;
			d3d11_device->GetImmediateContext( &d3d11_device_context );
			D3D11_MAPPED_SUBRESOURCE d3d11_mapped_subresource = {};
			assert( SUCCEEDED( d3d11_device_context->Map( _d3d11_texture2d, 0, _array_slice_count == 1 ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE, 0, &d3d11_mapped_subresource ) ) );
			ops::memory_copy( top_mip_level_data, d3d11_mapped_subresource.pData, top_mip_level_data_size );
			d3d11_device_context->Unmap( _d3d11_texture2d, 0 );
			d3d11_device_context->Release();
			d3d11_device->Release();
		}

		virtual void_c get_data( void_c * top_mip_level_data, sint32_c top_mip_level_data_size ) const override
		{
			assert( _get_data_enable == true );
			assert( top_mip_level_data );
			assert( top_mip_level_data_size == _width * _height * video_texture_format_size_get( _texture_format ) );
			ID3D11Device * d3d11_device = nullptr;
			_d3d11_texture2d->GetDevice( &d3d11_device );
			ID3D11DeviceContext * d3d11_device_context = nullptr;
			d3d11_device->GetImmediateContext( &d3d11_device_context );
			D3D11_MAPPED_SUBRESOURCE d3d11_mapped_subresource = {};
			assert( SUCCEEDED( d3d11_device_context->Map( _d3d11_texture2d, 0, D3D11_MAP_READ, 0, &d3d11_mapped_subresource ) ) );
			ops::memory_copy( d3d11_mapped_subresource.pData, top_mip_level_data, top_mip_level_data_size );
			d3d11_device_context->Unmap( _d3d11_texture2d, 0 );
			d3d11_device_context->Release();
			d3d11_device->Release();
		}

		virtual void_c generate_mips() const
		{
			assert( _d3d11_texture2d );
			assert( _mip_enable == true );
			assert( _target_enable == true );
			ID3D11Device * d3d11_device = nullptr;
			_d3d11_texture2d->GetDevice( reinterpret_cast< ID3D11Device * * >( &d3d11_device ) );
			ID3D11DeviceContext * d3d11_device_context = nullptr;
			d3d11_device_context->GenerateMips( _d3d11_texture2d_srv );
			d3d11_device_context->Release();
			d3d11_device->Release();
		}

		virtual void_c * get_base_interface() const override
		{
			return _d3d11_texture2d;
		}

	};

	class video_depth_stencil_d3d11_c : public video_depth_stencil_c
	{
	protected:
		friend class video_interface_d3d11_c;
		video_depth_stencil_format_e _depth_stencil_format;
		sint32_c _width;
		sint32_c _height;
		sint32_c _multi_sample_count;
		sint32_c _array_slice_count;
		sint32_c _size;
		ID3D11Texture2D * _d3d11_texture2d;
		ID3D11DepthStencilView * _d3d11_texture2d_dsv;
		ID3D11DepthStencilView * _d3d11_texture2darray_dsv;
		ID3D11DepthStencilView * * _d3d11_texture2darray_slice_dsvs;

	protected:
		video_depth_stencil_d3d11_c( video_interface_c * video_interface, ID3D11Device * d3d11_device, video_depth_stencil_format_e depth_stencil_format, sint32_c width, sint32_c height, sint32_c multi_sample_count, sint32_c array_slice_count )
			: video_depth_stencil_c( video_interface )
			, _depth_stencil_format( video_depth_stencil_format_e_none )
			, _width( 0 )
			, _height( 0 )
			, _multi_sample_count( 0 )
			, _array_slice_count( 0 )
			, _size( 0 )
			, _d3d11_texture2d( nullptr )
			, _d3d11_texture2d_dsv( nullptr )
			, _d3d11_texture2darray_dsv( nullptr )
			, _d3d11_texture2darray_slice_dsvs( nullptr )
		{
			// check stuff
			assert( depth_stencil_format > video_depth_stencil_format_e_none && depth_stencil_format < video_depth_stencil_format_e_count_ );
			assert( width > 0 );
			assert( height > 0 );
			assert( multi_sample_count == 1 || multi_sample_count == 2 || multi_sample_count == 4 || multi_sample_count == 8 );
			assert( array_slice_count > 0 );

			// copy stuff
			_depth_stencil_format = depth_stencil_format;
			_width = width;
			_height = height;
			_multi_sample_count = multi_sample_count;
			_array_slice_count = array_slice_count;
			_size = width * height * video_depth_stencil_format_size_get( depth_stencil_format );

			// create texture description
			D3D11_TEXTURE2D_DESC d3d11_texture2d_description = {};
			d3d11_texture2d_description.Width = width;
			d3d11_texture2d_description.Height = height;
			d3d11_texture2d_description.MipLevels = 1;
			d3d11_texture2d_description.ArraySize = array_slice_count;
			d3d11_texture2d_description.Format = d3d11_helpers::get_format( depth_stencil_format );
			d3d11_texture2d_description.SampleDesc.Count = multi_sample_count;
			d3d11_texture2d_description.SampleDesc.Quality = multi_sample_count == 1 ? 0 : D3D11_STANDARD_MULTISAMPLE_PATTERN;
			d3d11_texture2d_description.Usage = D3D11_USAGE_DEFAULT;
			d3d11_texture2d_description.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			d3d11_texture2d_description.CPUAccessFlags = 0;
			d3d11_texture2d_description.MiscFlags = 0;

			// create texture
			HRESULT h_result = d3d11_device->CreateTexture2D( &d3d11_texture2d_description, NULL, &_d3d11_texture2d );
			assert( SUCCEEDED( h_result ) );

			// create Texture2D depth stencil view
			if ( true )
			{
				D3D11_DEPTH_STENCIL_VIEW_DESC d3d11_depth_stencil_view_description = {};
				d3d11_depth_stencil_view_description.Format = d3d11_texture2d_description.Format;
				if ( multi_sample_count == 1 )
				{
					d3d11_depth_stencil_view_description.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
					d3d11_depth_stencil_view_description.Texture2D.MipSlice = 0;
				}
				else
				{
					d3d11_depth_stencil_view_description.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
				}
				h_result = d3d11_device->CreateDepthStencilView( _d3d11_texture2d, &d3d11_depth_stencil_view_description, &_d3d11_texture2d_dsv );
				assert( SUCCEEDED( h_result ) );
			}

			// create texture depth stencil view
			{
				D3D11_DEPTH_STENCIL_VIEW_DESC d3d11_depth_stencil_view_description = {};
				d3d11_depth_stencil_view_description.Format = d3d11_texture2d_description.Format;
				if ( multi_sample_count > 1 )
				{
					d3d11_depth_stencil_view_description.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
					d3d11_depth_stencil_view_description.Texture2DArray.MipSlice = 0;
					d3d11_depth_stencil_view_description.Texture2DArray.FirstArraySlice = 0;
					d3d11_depth_stencil_view_description.Texture2DArray.ArraySize = array_slice_count;
				}
				else
				{
					d3d11_depth_stencil_view_description.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
					d3d11_depth_stencil_view_description.Texture2DMSArray.FirstArraySlice = 0;
					d3d11_depth_stencil_view_description.Texture2DMSArray.ArraySize = array_slice_count;
				}
				h_result = d3d11_device->CreateDepthStencilView( _d3d11_texture2d, &d3d11_depth_stencil_view_description, &_d3d11_texture2darray_dsv );
				assert( SUCCEEDED( h_result ) );
			}

			_d3d11_texture2darray_slice_dsvs = new ID3D11DepthStencilView * [ array_slice_count ];
			for ( sint32_c i = 0; i < array_slice_count; i++ )
			{
				D3D11_DEPTH_STENCIL_VIEW_DESC d3d11_depth_stencil_view_description = {};
				d3d11_depth_stencil_view_description.Format = d3d11_texture2d_description.Format;
				if ( multi_sample_count == 1 )
				{
					d3d11_depth_stencil_view_description.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
					d3d11_depth_stencil_view_description.Texture2DArray.MipSlice = 0;
					d3d11_depth_stencil_view_description.Texture2DArray.FirstArraySlice = i;
					d3d11_depth_stencil_view_description.Texture2DArray.ArraySize = 1;
				}
				else
				{
					d3d11_depth_stencil_view_description.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
					d3d11_depth_stencil_view_description.Texture2DMSArray.FirstArraySlice = i;
					d3d11_depth_stencil_view_description.Texture2DMSArray.ArraySize = 1;
				}
				h_result = d3d11_device->CreateDepthStencilView( _d3d11_texture2d, &d3d11_depth_stencil_view_description, &_d3d11_texture2darray_slice_dsvs[ i ] );
				assert( SUCCEEDED( h_result ) );
			}
		}

	public:
		virtual ~video_depth_stencil_d3d11_c() override
		{
			if ( _d3d11_texture2darray_slice_dsvs )
			{
				for ( sint32_c i = 0; i < _array_slice_count; i++ )
				{
					com_safe_release( _d3d11_texture2darray_slice_dsvs[ i ] );
				}
				delete[] _d3d11_texture2darray_slice_dsvs;
				_d3d11_texture2darray_slice_dsvs = nullptr;
			}
			com_safe_release( _d3d11_texture2darray_dsv );
			com_safe_release( _d3d11_texture2d_dsv );
			com_safe_release( _d3d11_texture2d );
		}

		virtual video_depth_stencil_format_e get_depth_stencil_format() const override
		{
			return _depth_stencil_format;
		}

		virtual sint32_c get_width() const override
		{
			return _width;
		}

		virtual sint32_c get_height() const override
		{
			return _height;
		}

		virtual sint32_c get_multi_sample_count() const override
		{
			return _multi_sample_count;
		}

		virtual sint32_c get_array_count() const override
		{
			return _array_slice_count;
		}

	};

	class video_vertex_shader_d3d11_c : public video_vertex_shader_c
	{
	protected:
		friend class video_interface_d3d11_c;
		string16_c _name; // so we can track this shader when debugging.
		video_vertex_layout_c _input_vertex_layout;
		video_vertex_format_c _output_vertex_format;
		ID3D11InputLayout * _d3d11_input_layout;
		ID3DBlob * _d3d11_compiled_program;
		ID3D11VertexShader * _d3d11_vertex_shader;
		ID3D11GeometryShader * _d3d11_geometry_shader_output; // when using the stream output stage, a geometry shader needs to be bound. this geometry shader is created from the vertex shader using CreateGeometryShaderWithStreamOutput. this geometry shader will effectively pass-through the vertices from the vertex shader to the stream-output stage.
		string16_c _error_messages;

	protected:
		void_c _finish_construction( ID3D11Device * d3d11_device )
		{
			// create vertex shader.
			HRESULT h_result = d3d11_device->CreateVertexShader( _d3d11_compiled_program->GetBufferPointer(), _d3d11_compiled_program->GetBufferSize(), NULL, &_d3d11_vertex_shader );
			assert( SUCCEEDED( h_result ) );

			// create input layout.
			D3D11_INPUT_ELEMENT_DESC D3D11InputElementDescArray[ 16 ] = {};
			sint32_c k = 0;
			for ( sint32_c i = 0; i < _input_vertex_layout.vertex_formats_count; i++ )
			{
				sint32_c offset = 0;
				video_vertex_format_c const * const vertex_format = _input_vertex_layout.vertex_formats[ i ];
				for ( sint32_c j = 0; j < vertex_format->elements_count; j++ )
				{
					video_vertex_format_c::element_c const * vertex_format_element = &vertex_format->elements[ j ];
					assert( vertex_format_element->format_count >= 1 && vertex_format_element->format_count <= 4 );
					D3D11_INPUT_ELEMENT_DESC & input_element_desc = D3D11InputElementDescArray[ k ];
					input_element_desc.SemanticName = d3d11_helpers::get_semantic_name( vertex_format_element->semantic_name );
					input_element_desc.SemanticIndex = vertex_format_element->semantic_index;
					input_element_desc.Format = d3d11_helpers::get_format( vertex_format_element->format_type, vertex_format_element->format_count );
					input_element_desc.InputSlot = i;
					input_element_desc.AlignedByteOffset = offset;
					input_element_desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
					input_element_desc.InstanceDataStepRate = 0;
					offset += video_get_vertex_format_element_type_size( vertex_format_element->format_type ) * vertex_format_element->format_count;
					k++;
					assert( k < 16 );
				}
			}
			h_result = d3d11_device->CreateInputLayout( D3D11InputElementDescArray, k, _d3d11_compiled_program->GetBufferPointer(), _d3d11_compiled_program->GetBufferSize(), &_d3d11_input_layout );
			assert( SUCCEEDED( h_result ) );
		}

	protected:
		// creates vertex shader from source code. looks for the entry point "vs_main".
		video_vertex_shader_d3d11_c( video_interface_c * video_interface, ID3D11Device * d3d11_device, char16_c const * name, char8_c const * source_code, sint32_c source_code_size, char8_c const * source_code_file_path, video_shader_includer_c * shader_includer, video_shader_defines_c const * shader_defines, video_vertex_layout_c const * input_vertex_layout )
			: video_vertex_shader_c( video_interface )
			, _name( core_list_mode_e_dynamic, name )
			, _input_vertex_layout()
			, _output_vertex_format()
			, _d3d11_input_layout( nullptr )
			, _d3d11_compiled_program( nullptr )
			, _d3d11_vertex_shader( nullptr )
			, _d3d11_geometry_shader_output( nullptr )
			, _error_messages()
		{
			// check stuff.
			assert( _D3DCompile );
			assert( source_code );
			assert( source_code_size );
			assert( input_vertex_layout );

			// translate defines.
			D3D_SHADER_MACRO * d3d_shader_macros = nullptr;
			if ( shader_defines && shader_defines->elements_count > 0 )
			{
				d3d_shader_macros = new D3D_SHADER_MACRO[ shader_defines->elements_count + 1 ];
				D3D_SHADER_MACRO * d3d_shader_macro = d3d_shader_macros;
				for ( sint32_c i = 0; i < shader_defines->elements_count; i++ )
				{
					d3d_shader_macro->Name = shader_defines->elements[ i ].name;
					d3d_shader_macro->Definition = shader_defines->elements[ i ].value;
					d3d_shader_macro++;
				}
				d3d_shader_macro->Name = nullptr;
				d3d_shader_macro->Definition = nullptr;
			}

			// compile vertex shader source code.
			CD3DInclude d3d_include( shader_includer );
			ID3DBlob * d3d11_error_messages = nullptr;
			HRESULT h_result = reinterpret_cast< pD3DCompile >( _D3DCompile )( source_code, source_code_size, source_code_file_path, d3d_shader_macros, &d3d_include, "vs_main", "vs_4_1", D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0, &_d3d11_compiled_program, &d3d11_error_messages );
			if ( d3d_shader_macros )
			{
				delete[] d3d_shader_macros;
				d3d_shader_macros = nullptr;
			}
			if ( FAILED( h_result ) )
			{
				_error_messages = reinterpret_cast< char8_c * >( d3d11_error_messages->GetBufferPointer() );
				debug_log( log_type_e_error, _error_messages.character_list.get_internal_array() );
				d3d11_error_messages->Release();
				return;
			}

			// finish.
			_input_vertex_layout = *input_vertex_layout;
			_finish_construction( d3d11_device );
		}

		// creates vertex shader from pre-compiled code.
		video_vertex_shader_d3d11_c( video_interface_c * video_interface, ID3D11Device * d3d11_device, char16_c const * name, char8_c const * compiled_code, sint32_c compiled_code_size, video_vertex_layout_c const * vertex_layout )
			: video_vertex_shader_c( video_interface )
			, _name( core_list_mode_e_dynamic, name )
			, _input_vertex_layout()
			, _output_vertex_format()
			, _d3d11_input_layout( nullptr )
			, _d3d11_compiled_program( nullptr )
			, _d3d11_vertex_shader( nullptr )
			, _d3d11_geometry_shader_output( nullptr )
			, _error_messages()
		{
			// create blob.
			HRESULT h_result = D3DCreateBlob( compiled_code_size, &_d3d11_compiled_program );
			assert( SUCCEEDED( h_result ) );
			CopyMemory( _d3d11_compiled_program->GetBufferPointer(), compiled_code, compiled_code_size );

			// finish.
			_input_vertex_layout = *vertex_layout;
			_finish_construction( d3d11_device );
		}

	public:
		virtual ~video_vertex_shader_d3d11_c() override
		{
			com_safe_release( _d3d11_input_layout );
			com_safe_release( _d3d11_compiled_program );
			com_safe_release( _d3d11_vertex_shader );
			com_safe_release( _d3d11_geometry_shader_output );
		}

		virtual void_c enable_stream_out( video_vertex_format_c const * output_vertex_format ) override
		{
			assert( _d3d11_vertex_shader );
			assert( _d3d11_geometry_shader_output == nullptr );
			assert( output_vertex_format );

			_output_vertex_format = *output_vertex_format;

			// create output layout.
			D3D11_SO_DECLARATION_ENTRY d3d11_so_declaration_entry_array[ 8 ] = {};
			sint32_c d3d11_declaration_entry_array_length = 0;
			for ( sint32_c i = 0; i < _output_vertex_format.elements_count; i++ )
			{
				video_vertex_format_c::element_c const & vertex_format_element = _output_vertex_format.elements[ i ];
				assert( vertex_format_element.format_type == video_vertex_format_c::element_c::type_e_float32 ); // stream output can only output float32s.
				D3D11_SO_DECLARATION_ENTRY & so_declaration_entry = d3d11_so_declaration_entry_array[ i ];
				so_declaration_entry.SemanticName = d3d11_helpers::get_semantic_name( _output_vertex_format.elements[ i ].semantic_name );
				so_declaration_entry.SemanticIndex = _output_vertex_format.elements[ i ].semantic_index;
				so_declaration_entry.StartComponent = 0;
				so_declaration_entry.ComponentCount = _output_vertex_format.elements[ i ].format_count;
				so_declaration_entry.OutputSlot = 0;
				d3d11_declaration_entry_array_length++;
			}
			// create the geometry shader.
			ID3D11Device * d3d11_device = nullptr;
			_d3d11_vertex_shader->GetDevice( reinterpret_cast< ID3D11Device * * >( &d3d11_device ) );
			UINT buffer_strides[ 1 ] = { static_cast< UINT >( _output_vertex_format.stride ) };
			assert( SUCCEEDED( d3d11_device->CreateGeometryShaderWithStreamOutput( _d3d11_compiled_program->GetBufferPointer(), _d3d11_compiled_program->GetBufferSize(), d3d11_so_declaration_entry_array, d3d11_declaration_entry_array_length, buffer_strides, 1, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &_d3d11_geometry_shader_output ) ) );
			d3d11_device->Release();
		}

		virtual void_c get_compiled_code( void_c const * & compiled_code, sint32_c & compiled_code_size ) override
		{
			assert( _d3d11_compiled_program->GetBufferSize() < static_cast< SIZE_T >( constants< sint32_c >::maximum() ) );
			compiled_code = _d3d11_compiled_program->GetBufferPointer();
			assert( _d3d11_compiled_program->GetBufferSize() < static_cast< SIZE_T >( constants< sint32_c >::maximum() ) );
			compiled_code_size = static_cast< sint32_c >( _d3d11_compiled_program->GetBufferSize() );
		}

	};

	class video_geometry_shader_d3d11_c : public video_geometry_shader_c
	{
	protected:
		friend class video_interface_d3d11_c;
		string16_c _name; // so we can track this shader when debugging.
		ID3DBlob * _d3d11_compiled_program;
		ID3D11GeometryShader * _d3d11_geometry_shader;
		string16_c _error_messages;

	protected:
		// creates geometry shader from source code. looks for the entry point "gs_main".
		video_geometry_shader_d3d11_c( video_interface_c * video_interface, ID3D11Device * d3d11_device, char16_c const * name, char8_c const * source_code, sint32_c source_code_size, char8_c const * source_code_file_path, video_shader_includer_c * shader_includer, video_shader_defines_c const * shader_defines )
			: video_geometry_shader_c( video_interface )
			, _name( core_list_mode_e_dynamic, name )
			, _d3d11_compiled_program( nullptr )
			, _d3d11_geometry_shader( nullptr )
			, _error_messages()
		{
			assert( _D3DCompile );

			// translate defines
			D3D_SHADER_MACRO * d3d_shader_macros = nullptr;
			if ( shader_defines && shader_defines->elements_count > 0 )
			{
				d3d_shader_macros = new D3D_SHADER_MACRO[ shader_defines->elements_count + 1 ];
				D3D_SHADER_MACRO * d3d_shader_macro = d3d_shader_macros;
				for ( sint32_c i = 0; i < shader_defines->elements_count; i++ )
				{
					d3d_shader_macro->Name = shader_defines->elements[ i ].name;
					d3d_shader_macro->Definition = shader_defines->elements[ i ].value;
					d3d_shader_macro++;
				}
				d3d_shader_macro->Name = nullptr;
				d3d_shader_macro->Definition = nullptr;
			}

			// compile shader
			CD3DInclude d3d_include( shader_includer );
			ID3DBlob * d3d11_error_messages = nullptr;
			HRESULT h_result = reinterpret_cast< pD3DCompile >( _D3DCompile )( source_code, source_code_size, source_code_file_path, d3d_shader_macros, &d3d_include, "gs_main", "gs_4_1", D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0, &_d3d11_compiled_program, &d3d11_error_messages );
			if ( d3d_shader_macros )
			{
				delete[] d3d_shader_macros;
				d3d_shader_macros = nullptr;
			}
			if ( FAILED( h_result ) )
			{
				_error_messages = reinterpret_cast< char8_c * >( d3d11_error_messages->GetBufferPointer() );
				debug_log( log_type_e_error, _error_messages.character_list.get_internal_array() );
				d3d11_error_messages->Release();
				return;
			}

			// finish.
			h_result = d3d11_device->CreateGeometryShader( _d3d11_compiled_program->GetBufferPointer(), _d3d11_compiled_program->GetBufferSize(), NULL, &_d3d11_geometry_shader );
			assert( SUCCEEDED( h_result ) );
		}

		// creates geometry shader from pre-compiled code.
		video_geometry_shader_d3d11_c( video_interface_c * video_interface, ID3D11Device * d3d11_device, char16_c const * name, char8_c const * compiled_code, sint32_c compiled_code_size )
			: video_geometry_shader_c( video_interface )
			, _name( core_list_mode_e_dynamic, name )
			, _d3d11_compiled_program( nullptr )
			, _d3d11_geometry_shader( nullptr )
			, _error_messages()
		{
			// create blob.
			HRESULT h_result = D3DCreateBlob( compiled_code_size, &_d3d11_compiled_program );
			assert( SUCCEEDED( h_result ) );
			CopyMemory( _d3d11_compiled_program->GetBufferPointer(), compiled_code, compiled_code_size );

			// finish.
			h_result = d3d11_device->CreateGeometryShader( _d3d11_compiled_program->GetBufferPointer(), _d3d11_compiled_program->GetBufferSize(), NULL, &_d3d11_geometry_shader );
			assert( SUCCEEDED( h_result ) );
		}

	public:
		virtual ~video_geometry_shader_d3d11_c() override
		{
			com_safe_release( _d3d11_geometry_shader );
			com_safe_release( _d3d11_compiled_program );
		}

		virtual void_c get_compiled_code( void_c const * & compiled_code, sint32_c & compiled_code_size ) override
		{
			assert( _d3d11_compiled_program->GetBufferSize() < static_cast< SIZE_T >( constants< sint32_c >::maximum() ) );
			compiled_code = _d3d11_compiled_program->GetBufferPointer();
			assert( _d3d11_compiled_program->GetBufferSize() < static_cast< SIZE_T >( constants< sint32_c >::maximum() ) );
			compiled_code_size = static_cast< sint32_c >( _d3d11_compiled_program->GetBufferSize() );
		}

	};

	class video_pixel_shader_d3d11_c : public video_pixel_shader_c
	{
	protected:
		friend class video_interface_d3d11_c;
		string16_c _name; // so we can track this shader when debugging.
		ID3DBlob * _d3d11_compiled_program;
		ID3D11PixelShader * _d3d11_pixel_shader;
		string16_c _error_messages;

	protected:
		video_pixel_shader_d3d11_c( video_interface_c * video_interface, ID3D11Device * d3d11_device, char16_c const * name, char8_c const * source_code, sint32_c source_code_size, char8_c const * source_code_file_path, video_shader_includer_c * shader_includer, video_shader_defines_c const * shader_defines )
			: video_pixel_shader_c( video_interface )
			, _name( core_list_mode_e_dynamic, name )
			, _d3d11_compiled_program( nullptr )
			, _d3d11_pixel_shader( nullptr )
			, _error_messages()
		{
			assert( _D3DCompile );

			// translate defines.
			D3D_SHADER_MACRO * d3d_shader_macros = nullptr;
			if ( shader_defines && shader_defines->elements_count > 0 )
			{
				d3d_shader_macros = new D3D_SHADER_MACRO[ shader_defines->elements_count + 1 ];
				D3D_SHADER_MACRO * d3d_shader_macro = d3d_shader_macros;
				for ( sint32_c i = 0; i < shader_defines->elements_count; i++ )
				{
					d3d_shader_macro->Name = shader_defines->elements[ i ].name;
					d3d_shader_macro->Definition = shader_defines->elements[ i ].value;
					d3d_shader_macro++;
				}
				d3d_shader_macro->Name = nullptr;
				d3d_shader_macro->Definition = nullptr;
			}

			// compile shader.
			CD3DInclude include( shader_includer );
			ID3DBlob * d3d11_error_messages = nullptr;
			HRESULT h_result = reinterpret_cast< pD3DCompile >( _D3DCompile )( source_code, source_code_size, source_code_file_path, d3d_shader_macros, &include, "ps_main", "ps_4_1", D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0, &_d3d11_compiled_program, &d3d11_error_messages );
			if ( d3d_shader_macros )
			{
				delete[] d3d_shader_macros;
				d3d_shader_macros = nullptr;
			}
			if ( FAILED( h_result ) )
			{
				_error_messages = reinterpret_cast< char8_c * >( d3d11_error_messages->GetBufferPointer() );
				debug_log( log_type_e_error, _error_messages.character_list.get_internal_array() );
				d3d11_error_messages->Release();
				return;
			}

			// finish.
			h_result = d3d11_device->CreatePixelShader( _d3d11_compiled_program->GetBufferPointer(), _d3d11_compiled_program->GetBufferSize(), NULL, &_d3d11_pixel_shader );
			assert( SUCCEEDED( h_result ) );
		}

		// creates pixel shader from pre-compiled code.
		video_pixel_shader_d3d11_c( video_interface_c * video_interface, ID3D11Device * d3d11_device, char16_c const * name, char8_c const * compiled_code, sint32_c compiled_code_size )
			: video_pixel_shader_c( video_interface )
			, _name( core_list_mode_e_dynamic, name )
			, _d3d11_compiled_program( nullptr )
			, _d3d11_pixel_shader( nullptr )
			, _error_messages()
		{
			// create blob.
			HRESULT h_result = D3DCreateBlob( compiled_code_size, &_d3d11_compiled_program );
			assert( SUCCEEDED( h_result ) );
			CopyMemory( _d3d11_compiled_program->GetBufferPointer(), compiled_code, compiled_code_size );

			// finish.
			h_result = d3d11_device->CreatePixelShader( _d3d11_compiled_program->GetBufferPointer(), _d3d11_compiled_program->GetBufferSize(), NULL, &_d3d11_pixel_shader );
			assert( SUCCEEDED( h_result ) );
		}

	public:
		virtual ~video_pixel_shader_d3d11_c() override
		{
			com_safe_release( _d3d11_pixel_shader );
			com_safe_release( _d3d11_compiled_program );
		}

		virtual void_c get_compiled_code( void_c const * & compiled_code, sint32_c & compiled_code_size ) override
		{
			assert( _d3d11_compiled_program->GetBufferSize() < static_cast< SIZE_T >( constants< sint32_c >::maximum() ) );
			compiled_code = _d3d11_compiled_program->GetBufferPointer();
			assert( _d3d11_compiled_program->GetBufferSize() < static_cast< SIZE_T >( constants< sint32_c >::maximum() ) );
			compiled_code_size = static_cast< sint32_c >( _d3d11_compiled_program->GetBufferSize() );
		}

	};

	class video_output_d3d11_c : public video_output_c
	{
	protected:
		friend class video_interface_d3d11_c;
		ID3D11Device * _d3d11_device;
		ID3D11DeviceContext * _d3d11_device_context;
		IDXGIFactory * _dxgi_factory;
		HWND _window_handle;
		video_texture_format_e _texture_format;
		IDXGISwapChain * _dxgi_swap_chain;
		video_texture_d3d11_c * _texture; // this video texture interface wraps the swap chain surfaces.

	protected:
		video_output_d3d11_c( video_interface_c * video_interface, ID3D11Device * d3d11_device, IDXGIFactory * dxgi_factory, void_c * window_handle, video_texture_format_e texture_format )
			: video_output_c( video_interface )
			, _d3d11_device( d3d11_device )
			, _d3d11_device_context( nullptr )
			, _dxgi_factory( dxgi_factory )
			, _window_handle( reinterpret_cast< HWND >( window_handle ) )
			, _texture_format( texture_format )
			, _dxgi_swap_chain( nullptr )
			, _texture( nullptr )
		{
			_d3d11_device->AddRef();
			_d3d11_device->GetImmediateContext( &_d3d11_device_context );
			_dxgi_factory->AddRef();
			_texture = new video_texture_d3d11_c( _video_interface );
			_texture->_multi_sample_count = 1;
		}

	public:
		virtual ~video_output_d3d11_c() override
		{
			com_safe_release( _dxgi_swap_chain );
			com_safe_release( _texture->_d3d11_texture2d_rtv );
			com_safe_release( _texture->_d3d11_texture2d );
			com_safe_release( _dxgi_factory );
			com_safe_release( _d3d11_device_context );
			com_safe_release( _d3d11_device );
			delete _texture;
			_texture = nullptr;
		}

		virtual boolean_c update() override
		{
			RECT client_rectangle;
			GetClientRect( _window_handle, &client_rectangle );
			sint32_c client_size_a = client_rectangle.right - client_rectangle.left;
			sint32_c client_size_b = client_rectangle.bottom - client_rectangle.top;
			if ( client_size_a == 0 || client_size_b == 0 )
			{
				return false;
			}

			HRESULT h_result;
			if ( _dxgi_swap_chain )
			{
				if ( client_size_a == _texture->_width && client_size_b == _texture->_height )
				{
					return true;
				}

				// release existing.
				com_safe_release( _texture->_d3d11_texture2d );
				com_safe_release( _texture->_d3d11_texture2d_rtv );

				// copy parameters.
				_texture->_width = client_size_a;
				_texture->_height = client_size_b;

				// resize.
				//void_c * zeros[ 4 ] = { nullptr, nullptr, nullptr, nullptr };
				//_d3d11_device_context->OMSetRenderTargets( 4, reinterpret_cast< ID3D11RenderTargetView * * >( &zeros ), 0 ); // release any currently bound render targets

				DXGI_SWAP_CHAIN_DESC dxgi_swap_chain_description;
				_dxgi_swap_chain->GetDesc( &dxgi_swap_chain_description );
				_dxgi_swap_chain->ResizeBuffers( dxgi_swap_chain_description.BufferCount, client_size_a, client_size_b, dxgi_swap_chain_description.BufferDesc.Format, 0 );
			}
			else
			{
				// create swap chain.
				DXGI_SWAP_CHAIN_DESC dxgi_swap_chain_description = {};
				dxgi_swap_chain_description.BufferDesc.Width = client_size_a;
				dxgi_swap_chain_description.BufferDesc.Height = client_size_b;
				dxgi_swap_chain_description.BufferDesc.RefreshRate.Numerator = 60; // if set to 60/1, FPS will degrade over time and get really choppy even though fraps reports it as over 200.
				dxgi_swap_chain_description.BufferDesc.RefreshRate.Denominator = 1;
				dxgi_swap_chain_description.BufferDesc.Format = d3d11_helpers::get_format( _texture_format );
				dxgi_swap_chain_description.SampleDesc.Count = 1;
				dxgi_swap_chain_description.SampleDesc.Quality = 0;
				dxgi_swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				dxgi_swap_chain_description.BufferCount = 2;
				dxgi_swap_chain_description.OutputWindow = _window_handle;
				dxgi_swap_chain_description.Windowed = true;
				dxgi_swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //DXGI_SWAP_EFFECT_DISCARD;
				dxgi_swap_chain_description.Flags = 0;
				h_result = _dxgi_factory->CreateSwapChain( _d3d11_device, &dxgi_swap_chain_description, &_dxgi_swap_chain );
				if ( FAILED( h_result ) )
				{
					assert( false );
					return false;
				}

				h_result = _dxgi_factory->MakeWindowAssociation( _window_handle, DXGI_MWA_NO_WINDOW_CHANGES );
				if ( FAILED( h_result ) )
				{
					assert( false );
					return false;
				}

				// create video texture to wrap and hold texture pointers from swap buffer.
				_texture->_size = client_size_a * client_size_b * video_texture_format_size_get( _texture_format );
				_texture->_width = client_size_a;
				_texture->_height = client_size_b;
				_texture->_texture_format = _texture_format;
				_texture->_set_data_enable = false;
				_texture->_get_data_enable = false;
				_texture->_target_enable = true;
				_texture->_mip_enable = false;
			}

			// aquire swap chain buffer.
			h_result = _dxgi_swap_chain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< void_c * * >( &_texture->_d3d11_texture2d ) );
			assert( SUCCEEDED( h_result ) );
			if ( !SUCCEEDED( h_result ) )
			{
				return false;
			}

			// create render target view for swap chain buffer.
			h_result = _d3d11_device->CreateRenderTargetView( _texture->_d3d11_texture2d, NULL, &_texture->_d3d11_texture2d_rtv );
			assert( SUCCEEDED( h_result ) );
			if ( !SUCCEEDED( h_result ) )
			{
				return false;
			}

			return true;
		}

		virtual video_texture_c * get_texture_resource() const override
		{
			return _texture;
		}

		virtual void_c * get_window_handle() const override
		{
			return static_cast< void_c * >( _window_handle );
		}

		virtual void_c present() const override
		{
			assert( _dxgi_swap_chain );
			_dxgi_swap_chain->Present( 1, 0 );
		}

	};

	// declared in this cpp file so that we can avoid including direct3d headers from headers, and adding to intellisense junk.
	class video_interface_members_d3d11_c
	{
	public:
		// base interfaces
		IDXGIFactory * dxgi_factory;
		IDXGIAdapter * dxgi_adapter;
		IDXGIDevice * dxgi_device;
		ID3D11Device * d3d11_device;
		ID3D11DeviceContext * d3d11_device_context;
		ID3D11Debug * d3d11_debug;

		// rasterizer states for cull modes
		ID3D11RasterizerState * d3d11_rasterizer_state_cull_none_fill_face;
		ID3D11RasterizerState * d3d11_rasterizer_state_cull_front_fill_face;
		ID3D11RasterizerState * d3d11_rasterizer_state_cull_back_fill_face;
		ID3D11RasterizerState * d3d11_rasterizer_state_cull_none_fill_edge;
		ID3D11RasterizerState * d3d11_rasterizer_state_cull_front_fill_edge;
		ID3D11RasterizerState * d3d11_rasterizer_state_cull_back_fill_edge;
		ID3D11RasterizerState * d3d11_rasterizer_state_cull_none_fill_point;
		ID3D11RasterizerState * d3d11_rasterizer_state_cull_front_fill_point;
		ID3D11RasterizerState * d3d11_rasterizer_state_cull_back_fill_point;

		// depth stencil states for depth test modes
		ID3D11DepthStencilState * d3d11_depth_stencil_state_disable; // depth stencil disable needs its own state because setting depth stencil state to 0 sets a default depth stencil state instead of disabling the depth stencil test.
		ID3D11DepthStencilState * d3d11_depth_stencil_state_never;
		ID3D11DepthStencilState * d3d11_depth_stencil_state_always;
		ID3D11DepthStencilState * d3d11_depth_stencil_state_equal;
		ID3D11DepthStencilState * d3d11_depth_stencil_state_not_equal;
		ID3D11DepthStencilState * d3d11_depth_stencil_state_less;
		ID3D11DepthStencilState * d3d11_depth_stencil_state_less_equal;
		ID3D11DepthStencilState * d3d11_depth_stencil_state_greater;
		ID3D11DepthStencilState * d3d11_depth_stencil_state_greater_equal;
		ID3D11DepthStencilState * d3d11_depth_stencil_state_never_write_disabled;
		ID3D11DepthStencilState * d3d11_depth_stencil_state_always_write_disabled;
		ID3D11DepthStencilState * d3d11_depth_stencil_state_equal_write_disabled;
		ID3D11DepthStencilState * d3d11_depth_stencil_state_not_equal_write_disabled;
		ID3D11DepthStencilState * d3d11_depth_stencil_state_less_write_disabled;
		ID3D11DepthStencilState * d3d11_depth_stencil_state_less_equal_write_disabled;
		ID3D11DepthStencilState * d3d11_depth_stencil_state_greater_write_disabled;
		ID3D11DepthStencilState * d3d11_depth_stencil_state_greater_equal_write_disabled;

		// make_matrix_blended states for make_matrix_blended modes
		ID3D11BlendState * d3d11_blend_state_set; // overwrite existing.
		ID3D11BlendState * d3d11_blend_state_mix; // mix with existing, using alpha as make_matrix_blended weight.
		ID3D11BlendState * d3d11_blend_state_mix_set_alpha;
		ID3D11BlendState * d3d11_blend_state_add; // add to existing.
		ID3D11BlendState * d3d11_blend_state_sub; // subract from existing.
		ID3D11BlendState * d3d11_blend_state_min; // compare with existing, keep smaller value.
		ID3D11BlendState * d3d11_blend_state_max; // compare with existing, keep smaller value.

		// sampler states
		ID3D11SamplerState * d3d11_sampler_state_point_wrap;
		ID3D11SamplerState * d3d11_sampler_state_linear_wrap;
		ID3D11SamplerState * d3d11_sampler_state_anisotropic_wrap;
		ID3D11SamplerState * d3d11_sampler_state_point_clamp;
		ID3D11SamplerState * d3d11_sampler_state_linear_clamp;
		ID3D11SamplerState * d3d11_sampler_state_anisotropic_clamp;
		ID3D11SamplerState * d3d11_sampler_state_shadow; // linear, clamp, with less than comparison.

		//
		boolean_c bound_vertex_validated;
		video_vertex_buffer_d3d11_c * bound_vertex_buffers[ 3 ]; // 3rd slot is dummy, should always be nullptr, but is there because it simplifies checking how many values are in the array.
		video_vertex_shader_d3d11_c * bound_vertex_shader;

	};

	video_interface_d3d11_c::video_interface_d3d11_c()
		: _members( nullptr )
	{
	}

	video_interface_d3d11_c::~video_interface_d3d11_c()
	{
		com_safe_release( _members->d3d11_rasterizer_state_cull_none_fill_face );
		com_safe_release( _members->d3d11_rasterizer_state_cull_front_fill_face );
		com_safe_release( _members->d3d11_rasterizer_state_cull_back_fill_face );
		com_safe_release( _members->d3d11_rasterizer_state_cull_none_fill_edge );
		com_safe_release( _members->d3d11_rasterizer_state_cull_front_fill_edge );
		com_safe_release( _members->d3d11_rasterizer_state_cull_back_fill_edge );
		com_safe_release( _members->d3d11_rasterizer_state_cull_none_fill_point );
		com_safe_release( _members->d3d11_rasterizer_state_cull_front_fill_point );
		com_safe_release( _members->d3d11_rasterizer_state_cull_back_fill_point );

		com_safe_release( _members->d3d11_depth_stencil_state_disable );
		com_safe_release( _members->d3d11_depth_stencil_state_never );
		com_safe_release( _members->d3d11_depth_stencil_state_always );
		com_safe_release( _members->d3d11_depth_stencil_state_equal );
		com_safe_release( _members->d3d11_depth_stencil_state_not_equal );
		com_safe_release( _members->d3d11_depth_stencil_state_less );
		com_safe_release( _members->d3d11_depth_stencil_state_less_equal );
		com_safe_release( _members->d3d11_depth_stencil_state_greater );
		com_safe_release( _members->d3d11_depth_stencil_state_greater_equal );
		com_safe_release( _members->d3d11_depth_stencil_state_never_write_disabled );
		com_safe_release( _members->d3d11_depth_stencil_state_always_write_disabled );
		com_safe_release( _members->d3d11_depth_stencil_state_equal_write_disabled );
		com_safe_release( _members->d3d11_depth_stencil_state_not_equal_write_disabled );
		com_safe_release( _members->d3d11_depth_stencil_state_less_write_disabled );
		com_safe_release( _members->d3d11_depth_stencil_state_less_equal_write_disabled );
		com_safe_release( _members->d3d11_depth_stencil_state_greater_write_disabled );
		com_safe_release( _members->d3d11_depth_stencil_state_greater_equal_write_disabled );

		com_safe_release( _members->d3d11_blend_state_set );
		com_safe_release( _members->d3d11_blend_state_mix );
		com_safe_release( _members->d3d11_blend_state_mix_set_alpha );
		com_safe_release( _members->d3d11_blend_state_add );
		com_safe_release( _members->d3d11_blend_state_sub );
		com_safe_release( _members->d3d11_blend_state_min );
		com_safe_release( _members->d3d11_blend_state_max );

		com_safe_release( _members->d3d11_sampler_state_point_wrap );
		com_safe_release( _members->d3d11_sampler_state_linear_wrap );
		com_safe_release( _members->d3d11_sampler_state_anisotropic_wrap );
		com_safe_release( _members->d3d11_sampler_state_point_clamp );
		com_safe_release( _members->d3d11_sampler_state_linear_clamp );
		com_safe_release( _members->d3d11_sampler_state_anisotropic_clamp );
		com_safe_release( _members->d3d11_sampler_state_shadow );

		com_safe_release( _members->dxgi_factory );
		com_safe_release( _members->dxgi_adapter );
		com_safe_release( _members->dxgi_device );
		com_safe_release( _members->d3d11_device_context );
		com_safe_release( _members->d3d11_device );

		if ( _members->d3d11_debug )
		{
			_members->d3d11_debug->ReportLiveDeviceObjects( D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL );
		}

		com_safe_release( _members->d3d11_debug );
	}

	boolean_c video_interface_d3d11_c::start()
	{
		_members = new video_interface_members_d3d11_c();
		ops::memory_zero( _members, sizeof( video_interface_members_d3d11_c ) );

		assert( _D3DCompile == nullptr );
		assert( _members->d3d11_device == nullptr );

		//
		// load the embedded d3dcompiler dll file.
		// this code is currently not used but i am keeping it here for reference, because it took a while for me to figure out how to do it, and i may want to do it again one day if a truly standalone exe is desirable.
		//if ( System.is_in_editor == false )
		//{
		//	HRSRC ResourceHandle = FindResource( NULL, "D3DCompiler", "file" );
		//	assert( ResourceHandle);
		//	HGLOBAL ResourceDataHandle = LoadResource( NULL, ResourceHandle );
		//	assert( ResourceDataHandle );
		//	LPVOID ResourceData = LockResource( ResourceDataHandle );
		//	assert( ResourceData );
		//	HMEMORYMODULE ModuleHandle = MemoryLoadLibrary( ResourceData );
		//	assert( ModuleHandle );
		//	_D3DCompile = MemoryGetProcAddress( ModuleHandle, "D3DCompile" );
		//	assert( _D3DCompile );
		//}

		//
		// load the d3dcompiler dll file and aquire the pointer to the D3DCompile function.
		string16_c library_file_path;
		library_file_path = engine.get_content_manager()->get_engine_data_folder_path();
		library_file_path += "d3dcompiler_47.dll";
		HMODULE module_handle = LoadLibraryW( library_file_path.character_list.get_internal_array() );
		if ( !module_handle )
		{
			debug_annoy( L"error", L"d3dcompiler_47.dll not loaded." );
			return false;
		}

		_D3DCompile = GetProcAddress( module_handle, "D3DCompile" );
		if ( !_D3DCompile )
		{
			debug_annoy( L"error", L"D3DCompile not found." );
			return false;
		}

		//
		// create d3d11_device.
		UINT creation_flags = 0;
		#if defined( _DEBUG )
		creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
		#endif
		HRESULT h_result = 0;
		D3D_FEATURE_LEVEL feature_levels[ 3 ] = {
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0 };

		h_result = D3D11CreateDevice( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creation_flags, feature_levels, 3, D3D11_SDK_VERSION, &_members->d3d11_device, NULL, &_members->d3d11_device_context );
		if ( FAILED( h_result ) )
		{
			debug_annoy( L"error", L"D3D11CreateDevice failed." );
			return false;
		}

		//
		// aquire parent DXGI interfaces.
		h_result = _members->d3d11_device->QueryInterface( __uuidof( IDXGIDevice ), reinterpret_cast< void_c * * >( &_members->dxgi_device ) );
		if ( FAILED( h_result ) )
		{
			debug_annoy( L"error", L"IDXGIDevice could not be queried." );
			return false;
		}
		h_result = _members->dxgi_device->GetParent( __uuidof( IDXGIAdapter ), reinterpret_cast< void_c * * >( &_members->dxgi_adapter ) );
		if ( FAILED( h_result ) )
		{
			debug_annoy( L"error", L"IDXGIAdapter could not be queried." );
			return false;
		}
		h_result = _members->dxgi_adapter->GetParent( __uuidof( IDXGIFactory ), reinterpret_cast< void_c * * >( &_members->dxgi_factory ) );
		if ( FAILED( h_result ) )
		{
			debug_annoy( L"error", L"IDXGIFactory could not be queried." );
			return false;
		}

#if defined( _DEBUG )
		h_result = _members->d3d11_device->QueryInterface( __uuidof( ID3D11Debug ), reinterpret_cast< void * * >( &_members->d3d11_debug ) );
		if ( FAILED( h_result ) )
		{
			debug_annoy( L"error", L"ID3D11Debug could not be queried." );
			return false;
		}
#endif

		// initialize rasterizer states.
		if ( !d3d11_helpers::create_rasterizer_state( _members->d3d11_device, &_members->d3d11_rasterizer_state_cull_none_fill_face, video_cull_type_e_none, video_fill_type_e_face ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_rasterizer_state( _members->d3d11_device, &_members->d3d11_rasterizer_state_cull_none_fill_edge, video_cull_type_e_none, video_fill_type_e_edge ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_rasterizer_state( _members->d3d11_device, &_members->d3d11_rasterizer_state_cull_none_fill_point, video_cull_type_e_none, video_fill_type_e_point ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_rasterizer_state( _members->d3d11_device, &_members->d3d11_rasterizer_state_cull_back_fill_face, video_cull_type_e_back, video_fill_type_e_face ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_rasterizer_state( _members->d3d11_device, &_members->d3d11_rasterizer_state_cull_back_fill_edge, video_cull_type_e_back, video_fill_type_e_edge ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_rasterizer_state( _members->d3d11_device, &_members->d3d11_rasterizer_state_cull_back_fill_point, video_cull_type_e_back, video_fill_type_e_point ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_rasterizer_state( _members->d3d11_device, &_members->d3d11_rasterizer_state_cull_front_fill_face, video_cull_type_e_front, video_fill_type_e_face ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_rasterizer_state( _members->d3d11_device, &_members->d3d11_rasterizer_state_cull_front_fill_edge, video_cull_type_e_front, video_fill_type_e_edge ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_rasterizer_state( _members->d3d11_device, &_members->d3d11_rasterizer_state_cull_front_fill_point, video_cull_type_e_front, video_fill_type_e_point ) )
		{
			return false;
		}

		// initialize depth stencil states.
		if ( !d3d11_helpers::create_depth_stencil_state( _members->d3d11_device, &_members->d3d11_depth_stencil_state_disable, false, false, video_compare_function_e_never ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_depth_stencil_state( _members->d3d11_device, &_members->d3d11_depth_stencil_state_never, true, true, video_compare_function_e_never ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_depth_stencil_state( _members->d3d11_device, &_members->d3d11_depth_stencil_state_always, true, true, video_compare_function_e_always ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_depth_stencil_state( _members->d3d11_device, &_members->d3d11_depth_stencil_state_equal, true, true, video_compare_function_e_equal ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_depth_stencil_state( _members->d3d11_device, &_members->d3d11_depth_stencil_state_not_equal, true, true, video_compare_function_e_not_equal ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_depth_stencil_state( _members->d3d11_device, &_members->d3d11_depth_stencil_state_less, true, true, video_compare_function_e_less ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_depth_stencil_state( _members->d3d11_device, &_members->d3d11_depth_stencil_state_less_equal, true, true, video_compare_function_e_less_equal ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_depth_stencil_state( _members->d3d11_device, &_members->d3d11_depth_stencil_state_greater, true, true, video_compare_function_e_greater ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_depth_stencil_state( _members->d3d11_device, &_members->d3d11_depth_stencil_state_greater_equal, true, true, video_compare_function_e_greater_equal ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_depth_stencil_state( _members->d3d11_device, &_members->d3d11_depth_stencil_state_never_write_disabled, true, false, video_compare_function_e_never ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_depth_stencil_state( _members->d3d11_device, &_members->d3d11_depth_stencil_state_always_write_disabled, true, false, video_compare_function_e_always ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_depth_stencil_state( _members->d3d11_device, &_members->d3d11_depth_stencil_state_equal_write_disabled, true, false, video_compare_function_e_equal ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_depth_stencil_state( _members->d3d11_device, &_members->d3d11_depth_stencil_state_not_equal_write_disabled, true, false, video_compare_function_e_not_equal ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_depth_stencil_state( _members->d3d11_device, &_members->d3d11_depth_stencil_state_less_write_disabled, true, false, video_compare_function_e_less ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_depth_stencil_state( _members->d3d11_device, &_members->d3d11_depth_stencil_state_less_equal_write_disabled, true, false, video_compare_function_e_less_equal ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_depth_stencil_state( _members->d3d11_device, &_members->d3d11_depth_stencil_state_greater_write_disabled, true, false, video_compare_function_e_greater ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_depth_stencil_state( _members->d3d11_device, &_members->d3d11_depth_stencil_state_greater_equal_write_disabled, true, false, video_compare_function_e_greater_equal ) )
		{
			return false;
		}

		// initialzie make_matrix_blended states.
		if ( !d3d11_helpers::create_blend_state( _members->d3d11_device, &_members->d3d11_blend_state_set, video_blend_type_e_set ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_blend_state( _members->d3d11_device, &_members->d3d11_blend_state_mix, video_blend_type_e_mix ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_blend_state( _members->d3d11_device, &_members->d3d11_blend_state_add, video_blend_type_e_add ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_blend_state( _members->d3d11_device, &_members->d3d11_blend_state_sub, video_blend_type_e_sub ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_blend_state( _members->d3d11_device, &_members->d3d11_blend_state_min, video_blend_type_e_min ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_blend_state( _members->d3d11_device, &_members->d3d11_blend_state_max, video_blend_type_e_max ) )
		{
			return false;
		}

		// initialize sampler states.
		if ( !d3d11_helpers::create_sampler_state( _members->d3d11_device, &_members->d3d11_sampler_state_point_wrap, video_sample_filter_type_e_point, video_sample_address_type_e_wrap, video_compare_function_e_never ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_sampler_state( _members->d3d11_device, &_members->d3d11_sampler_state_linear_wrap, video_sample_filter_type_e_linear, video_sample_address_type_e_wrap, video_compare_function_e_never ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_sampler_state( _members->d3d11_device, &_members->d3d11_sampler_state_anisotropic_wrap, video_sample_filter_type_e_anisotropic, video_sample_address_type_e_wrap, video_compare_function_e_never ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_sampler_state( _members->d3d11_device, &_members->d3d11_sampler_state_point_clamp, video_sample_filter_type_e_point, video_sample_address_type_e_clamp, video_compare_function_e_never ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_sampler_state( _members->d3d11_device, &_members->d3d11_sampler_state_linear_clamp, video_sample_filter_type_e_linear, video_sample_address_type_e_clamp, video_compare_function_e_never ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_sampler_state( _members->d3d11_device, &_members->d3d11_sampler_state_anisotropic_clamp, video_sample_filter_type_e_anisotropic, video_sample_address_type_e_clamp, video_compare_function_e_never ) )
		{
			return false;
		}
		if ( !d3d11_helpers::create_sampler_state( _members->d3d11_device, &_members->d3d11_sampler_state_shadow, video_sample_filter_type_e_linear, video_sample_address_type_e_clamp, video_compare_function_e_less ) )
		{
			return false;
		}

		// bind sampler states once and forget about them.
		ID3D11SamplerState * d3d11_sampler_states[ 7 ] = {};
		d3d11_sampler_states[ 0 ] = _members->d3d11_sampler_state_point_wrap;
		d3d11_sampler_states[ 1 ] = _members->d3d11_sampler_state_linear_wrap;
		d3d11_sampler_states[ 2 ] = _members->d3d11_sampler_state_anisotropic_wrap;
		d3d11_sampler_states[ 3 ] = _members->d3d11_sampler_state_point_clamp;
		d3d11_sampler_states[ 4 ] = _members->d3d11_sampler_state_linear_clamp;
		d3d11_sampler_states[ 5 ] = _members->d3d11_sampler_state_anisotropic_clamp;
		d3d11_sampler_states[ 6 ] = _members->d3d11_sampler_state_shadow;
		_members->d3d11_device_context->VSSetSamplers( 0, 7, d3d11_sampler_states );
		_members->d3d11_device_context->GSSetSamplers( 0, 7, d3d11_sampler_states );
		_members->d3d11_device_context->PSSetSamplers( 0, 7, d3d11_sampler_states );

		return true;
	}

	video_output_c * video_interface_d3d11_c::create_output( void_c * window_handle, video_texture_format_e texture_format )
	{
		return new video_output_d3d11_c( this, _members->d3d11_device, _members->dxgi_factory, window_handle, texture_format );
	}

	video_vertex_buffer_c * video_interface_d3d11_c::create_vertex_buffer( video_vertex_format_c const * vertex_format, sint32_c vertex_count, void_c * data, sint32_c data_size, boolean_c set_data_enable, boolean_c get_data_enable, boolean_c target_enable )
	{
		return new video_vertex_buffer_d3d11_c( this, _members->d3d11_device, vertex_format, vertex_count, data, data_size, set_data_enable, get_data_enable, target_enable );
	}

	video_index_buffer_c * video_interface_d3d11_c::create_index_buffer( video_index_format_e index_format, sint32_c index_count, void_c * data, sint32_c data_size, boolean_c set_data_enable, boolean_c get_data_enable )
	{
		return new video_index_buffer_d3d11_c( this, _members->d3d11_device, index_format, index_count, data, data_size, set_data_enable, get_data_enable );
	}

	video_constant_buffer_c * video_interface_d3d11_c::create_constant_buffer( sint32_c size )
	{
		return new video_constant_buffer_d3d11_c( this, _members->d3d11_device, size );
	}

	video_texture_buffer_c * video_interface_d3d11_c::create_texture_buffer( sint32_c size )
	{
		return new video_texture_buffer_d3d11_c( this, _members->d3d11_device, size );
	}

	video_depth_stencil_c * video_interface_d3d11_c::create_depth_stencil( video_depth_stencil_format_e depth_stencil_format, sint32_c width, sint32_c height, sint32_c multi_sample_count, sint32_c array_slice_count )
	{
		return new video_depth_stencil_d3d11_c( this, _members->d3d11_device, depth_stencil_format, width, height, multi_sample_count, array_slice_count );
	}

	video_texture_c * video_interface_d3d11_c::create_texture( video_texture_format_e texture_format, sint32_c width, sint32_c height, sint32_c multi_sample_count, sint32_c array_slice_count, void_c const * top_mip_level_data, sint32_c top_mip_level_data_size, boolean_c set_data_enable, boolean_c get_data_enable, boolean_c target_enable, boolean_c mip_enable )
	{
		video_texture_d3d11_c * result = new video_texture_d3d11_c( this );
		result->start( _members->d3d11_device, texture_format, width, height, multi_sample_count, array_slice_count, top_mip_level_data, top_mip_level_data_size, set_data_enable, get_data_enable, target_enable, mip_enable );
		return result;
	}

	video_vertex_shader_c * video_interface_d3d11_c::create_vertex_shader( char16_c const * name, char8_c const * source_code, sint32_c source_code_size, char8_c const * source_code_file_path, video_shader_includer_c * shader_includer, video_shader_defines_c const * shader_defines, video_vertex_layout_c const * vertex_layout )
	{
		video_vertex_shader_d3d11_c * result = new video_vertex_shader_d3d11_c( this, _members->d3d11_device, name, source_code, source_code_size, source_code_file_path, shader_includer, shader_defines, vertex_layout );
		if ( result->_d3d11_vertex_shader )
		{
			return result;
		}
		delete result;
		return nullptr;
	}

	video_vertex_shader_c * video_interface_d3d11_c::create_vertex_shader( char16_c const * name, char8_c const * compiled_code, sint32_c compiled_code_size, video_vertex_layout_c const * vertex_layout )
	{
		video_vertex_shader_d3d11_c * result = new video_vertex_shader_d3d11_c( this, _members->d3d11_device, name, compiled_code, compiled_code_size, vertex_layout );
		if ( result->_d3d11_vertex_shader )
		{
			return result;
		}
		delete result;
		return nullptr;
	}

	video_geometry_shader_c * video_interface_d3d11_c::create_geometry_shader( char16_c const * name, char8_c const * source_code, sint32_c source_code_size, char8_c const * source_code_file_path, video_shader_includer_c * shader_includer, video_shader_defines_c const * shader_defines )
	{
		video_geometry_shader_d3d11_c * result = new video_geometry_shader_d3d11_c( this, _members->d3d11_device, name, source_code, source_code_size, source_code_file_path, shader_includer, shader_defines );
		if ( result->_d3d11_geometry_shader )
		{
			return result;
		}
		delete result;
		return nullptr;
	}

	video_geometry_shader_c * video_interface_d3d11_c::create_geometry_shader( char16_c const * name, char8_c const * compiled_code, sint32_c compiled_code_size )
	{
		video_geometry_shader_d3d11_c * result = new video_geometry_shader_d3d11_c( this, _members->d3d11_device, name, compiled_code, compiled_code_size );
		if ( result->_d3d11_geometry_shader )
		{
			return result;
		}
		delete result;
		return nullptr;
	}

	video_pixel_shader_c * video_interface_d3d11_c::create_pixel_shader( char16_c const * name, char8_c const * source_code, sint32_c source_code_size, char8_c const * source_code_file_path, video_shader_includer_c * shader_includer, video_shader_defines_c const * shader_defines )
	{
		video_pixel_shader_d3d11_c * result = new video_pixel_shader_d3d11_c( this, _members->d3d11_device, name, source_code, source_code_size, source_code_file_path, shader_includer, shader_defines );
		if ( result->_d3d11_pixel_shader )
		{
			return result;
		}
		delete result;
		return nullptr;
	}

	video_pixel_shader_c * video_interface_d3d11_c::create_pixel_shader( char16_c const * name, char8_c const * compiled_code, sint32_c compiled_code_size )
	{
		video_pixel_shader_d3d11_c * result = new video_pixel_shader_d3d11_c( this, _members->d3d11_device, name, compiled_code, compiled_code_size );
		if ( result->_d3d11_pixel_shader )
		{
			return result;
		}
		delete result;
		return nullptr;
	}

	void_c video_interface_d3d11_c::copy_sub_resource( video_texture_c * destination_resource, uint32_c destination_sub_resource_index, video_texture_c * source_resource, uint32_c source_sub_resource_index )
	{
		video_texture_d3d11_c * resolved_destination_resource = dynamic_cast< video_texture_d3d11_c * >( destination_resource );
		assert( resolved_destination_resource );
		assert( resolved_destination_resource->_d3d11_texture2d );
		video_texture_d3d11_c * resolved_source_resource = dynamic_cast< video_texture_d3d11_c * >( source_resource );
		assert( resolved_source_resource );
		assert( resolved_source_resource->_d3d11_texture2d );
		assert( resolved_destination_resource->_width == resolved_source_resource->_width );
		assert( resolved_destination_resource->_height == resolved_source_resource->_height );
		assert( resolved_destination_resource->_texture_format == resolved_source_resource->_texture_format );
		_members->d3d11_device_context->CopySubresourceRegion( resolved_destination_resource->_d3d11_texture2d, destination_sub_resource_index, 0, 0, 0, resolved_source_resource->_d3d11_texture2d, source_sub_resource_index, nullptr );
	}

	void_c video_interface_d3d11_c::copy_sub_resource( video_texture_c * destination_resource, uint32_c destination_sub_resource_index, video_depth_stencil_c * source_resource, uint32_c source_sub_resource_index )
	{
		video_texture_d3d11_c * resolved_destination_resource = dynamic_cast< video_texture_d3d11_c * >( destination_resource );
		assert( resolved_destination_resource );
		assert( resolved_destination_resource->_d3d11_texture2d );
		video_depth_stencil_d3d11_c * resolved_source_resource = dynamic_cast< video_depth_stencil_d3d11_c * >( source_resource );
		assert( resolved_source_resource );
		assert( resolved_source_resource->_d3d11_texture2d );
		assert( resolved_destination_resource->_width == resolved_source_resource->_width );
		assert( resolved_destination_resource->_height == resolved_source_resource->_height );
		assert( resolved_destination_resource->_texture_format == video_texture_format_e_r32_float && resolved_source_resource->_depth_stencil_format == video_depth_stencil_format_e_d32 );
		_members->d3d11_device_context->CopySubresourceRegion( resolved_destination_resource->_d3d11_texture2d, destination_sub_resource_index, 0, 0, 0, resolved_source_resource->_d3d11_texture2d, source_sub_resource_index, nullptr );
	}

	void_c video_interface_d3d11_c::clear_texture( video_output_c * texture, float32_c red, float32_c green, float32_c blue, float32_c alpha )
	{
		video_output_d3d11_c * output_d3d11 = reinterpret_cast< video_output_d3d11_c * >( texture );
		FLOAT color_rgba[ 4 ] = { red, green, blue, alpha };
		_members->d3d11_device_context->ClearRenderTargetView( output_d3d11->_texture->_d3d11_texture2d_rtv, color_rgba );
	}

	void_c video_interface_d3d11_c::clear_texture( video_texture_c * texture, float32_c red, float32_c green, float32_c blue, float32_c alpha )
	{
		video_texture_d3d11_c * texture2d_d3d11 = reinterpret_cast< video_texture_d3d11_c * >( texture );
		FLOAT color_rgba[ 4 ] = { red, green, blue, alpha };
		if ( texture2d_d3d11->_d3d11_texture2darray_rtv )
		{
			_members->d3d11_device_context->ClearRenderTargetView( texture2d_d3d11->_d3d11_texture2darray_rtv, color_rgba );
		}
		else
		{
			_members->d3d11_device_context->ClearRenderTargetView( texture2d_d3d11->_d3d11_texture2d_rtv, color_rgba );
		}
	}

	void_c video_interface_d3d11_c::clear_depth_stencil( video_depth_stencil_c * depth_stencil, float32_c depth, sint32_c stencil )
	{
		video_depth_stencil_d3d11_c * depth_stencil_d3d11 = reinterpret_cast< video_depth_stencil_d3d11_c * >( depth_stencil );
		UINT clear_flags = 0;
		if ( depth != -1.0f )
		{
			clear_flags |= D3D11_CLEAR_DEPTH;
		}
		if ( stencil != -1 )
		{
			clear_flags |= D3D11_CLEAR_STENCIL;
		}
		if ( depth_stencil_d3d11->_d3d11_texture2darray_dsv )
		{
			_members->d3d11_device_context->ClearDepthStencilView( depth_stencil_d3d11->_d3d11_texture2darray_dsv, clear_flags, depth, static_cast< uint8_c >( stencil ) );
		}
		else
		{
			_members->d3d11_device_context->ClearDepthStencilView( depth_stencil_d3d11->_d3d11_texture2d_dsv, clear_flags, depth, static_cast< uint8_c >( stencil ) );
		}
	}

	void_c video_interface_d3d11_c::clear_depth_stencil_slice( video_depth_stencil_c * depth_stencil, float32_c depth, sint32_c stencil, sint32_c slice_index )
	{
	}

	void_c video_interface_d3d11_c::bind_target_vertex_buffer( video_vertex_buffer_c * vertex_buffer )
	{
		ID3D11Buffer * buffers[] = { nullptr };
		UINT offsets[] = { 0 };
		if ( vertex_buffer )
		{
			video_vertex_buffer_d3d11_c * vertex_buffer_d3d11 = reinterpret_cast< video_vertex_buffer_d3d11_c * >( vertex_buffer );
			buffers[ 0 ] = vertex_buffer_d3d11->_d3d11_buffer;
		}
		_members->d3d11_device_context->SOSetTargets( 1, buffers, offsets );
	}

	void_c video_interface_d3d11_c::bind_target_textures( sint32_c textures_count, video_texture_c * * textures, video_depth_stencil_c * depth_stencil, video_texture_type_e texture_type )
	{
		assert( textures_count >= 0 && textures_count <= 8 );
		sint32_c width = -1;
		sint32_c height = -1;
		ID3D11RenderTargetView * d3d11_render_target_views[ 8 ] = {};
		if ( textures_count > 0 && depth_stencil )
		{
			assert( texture_type == video_texture_type_e_texture2d || texture_type == video_texture_type_e_texture2dms || texture_type == video_texture_type_e_texture2darray || texture_type == video_texture_type_e_texture2dmsarray );
		}
		for ( sint32_c i = 0; i < textures_count; i++ )
		{
			if ( textures )
			{
				assert( textures[ i ] );
				video_texture_d3d11_c * texture2d_d3d11 = dynamic_cast< video_texture_d3d11_c * >( textures[ i ] );
				assert( texture2d_d3d11 );
				assert( texture2d_d3d11->_target_enable );
				if ( width == -1 )
				{
					width = texture2d_d3d11->_width;
					height = texture2d_d3d11->_height;
				}
				else
				{
					assert( texture2d_d3d11->_width == width );
					assert( texture2d_d3d11->_height == height );
				}
				if ( texture_type == video_texture_type_e_texture2d )
				{
					assert( texture2d_d3d11->_d3d11_texture2d_rtv );
					assert( texture2d_d3d11->_multi_sample_count == 1 );
					d3d11_render_target_views[ i ] = texture2d_d3d11->_d3d11_texture2d_rtv;
				}
				else if ( texture_type == video_texture_type_e_texture2dms )
				{
					assert( texture2d_d3d11->_d3d11_texture2d_rtv );
					assert( texture2d_d3d11->_multi_sample_count > 1 );
					d3d11_render_target_views[ i ] = texture2d_d3d11->_d3d11_texture2d_rtv;
				}
				else if ( texture_type == video_texture_type_e_texture2darray )
				{
					assert( texture2d_d3d11->_d3d11_texture2darray_rtv );
					assert( texture2d_d3d11->_multi_sample_count == 1 );
					d3d11_render_target_views[ i ] = texture2d_d3d11->_d3d11_texture2darray_rtv;
				}
				else if ( texture_type == video_texture_type_e_texture2dmsarray )
				{
					assert( texture2d_d3d11->_d3d11_texture2darray_rtv );
					assert( texture2d_d3d11->_multi_sample_count > 1 );
					d3d11_render_target_views[ i ] = texture2d_d3d11->_d3d11_texture2darray_rtv;
				}
			}
		}
		ID3D11DepthStencilView * d3d11_depth_stencil_view = nullptr;
		if ( depth_stencil )
		{
			video_depth_stencil_d3d11_c * depth_stencil_d3d11 = dynamic_cast< video_depth_stencil_d3d11_c * >( depth_stencil );
			assert( depth_stencil_d3d11 );
			if ( width == -1 )
			{
				width = depth_stencil_d3d11->_width;
				height = depth_stencil_d3d11->_height;
			}
			else
			{
				assert( depth_stencil_d3d11->_width == width );
				assert( depth_stencil_d3d11->_height == height );
			}
			if ( texture_type == video_texture_type_e_texture2d )
			{
				assert( depth_stencil_d3d11->_d3d11_texture2d_dsv );
				assert( depth_stencil_d3d11->_multi_sample_count == 1 );
				d3d11_depth_stencil_view = depth_stencil_d3d11->_d3d11_texture2d_dsv;
			}
			else if ( texture_type == video_texture_type_e_texture2dms )
			{
				assert( depth_stencil_d3d11->_d3d11_texture2d_dsv );
				assert( depth_stencil_d3d11->_multi_sample_count > 1 );
				d3d11_depth_stencil_view = depth_stencil_d3d11->_d3d11_texture2d_dsv;
			}
			else if ( texture_type == video_texture_type_e_texture2darray )
			{
				assert( depth_stencil_d3d11->_d3d11_texture2darray_dsv );
				assert( depth_stencil_d3d11->_multi_sample_count == 1 );
				d3d11_depth_stencil_view = depth_stencil_d3d11->_d3d11_texture2darray_dsv;
			}
			else if ( texture_type == video_texture_type_e_texture2dmsarray )
			{
				assert( depth_stencil_d3d11->_d3d11_texture2darray_dsv );
				assert( depth_stencil_d3d11->_multi_sample_count > 1 );
				d3d11_depth_stencil_view = depth_stencil_d3d11->_d3d11_texture2darray_dsv;
			}
		}
		_members->d3d11_device_context->OMSetRenderTargets( textures_count, d3d11_render_target_views, d3d11_depth_stencil_view );

		if ( width != -1 )
		{
			D3D11_VIEWPORT view_port;
			view_port.TopLeftX = 0.0f;
			view_port.TopLeftY = 0.0f;
			view_port.Width = static_cast< FLOAT >( width );
			view_port.Height = static_cast< FLOAT >( height );
			view_port.MinDepth = 0.0f;
			view_port.MaxDepth = 1.0f;
			_members->d3d11_device_context->RSSetViewports( 1, &view_port );
		}
	}

	void_c video_interface_d3d11_c::bind_target_textures_slices( sint32_c textures_count, video_texture_c * * textures, sint32_c * textures_slices, video_depth_stencil_c * depth_stencil, sint32_c depth_stencil_slice )
	{
		assert( textures_count >= 0 && textures_count <= 4 );
		sint32_c width = -1;
		sint32_c height = -1;
		ID3D11RenderTargetView * d3d11_render_target_views[ 4 ] = {};
		for ( sint32_c i = 0; i < textures_count; i++ )
		{
			if ( textures )
			{
				assert( textures[ i ] );
				video_texture_d3d11_c * texture2d_d3d11 = dynamic_cast< video_texture_d3d11_c * >( textures[ i ] );
				assert( texture2d_d3d11 );
				assert( texture2d_d3d11->_target_enable );
				assert( texture2d_d3d11->_d3d11_texture2darray_slice_rtvs );
				assert( textures_slices[ i ] >= 0 && textures_slices[ i ] < texture2d_d3d11->_array_slice_count );
				if ( width == -1 )
				{
					width = texture2d_d3d11->_width;
					height = texture2d_d3d11->_height;
				}
				else
				{
					assert( texture2d_d3d11->_width == width );
					assert( texture2d_d3d11->_height == height );
				}
				d3d11_render_target_views[ i ] = texture2d_d3d11->_d3d11_texture2darray_slice_rtvs[ textures_slices[ i ] ];
			}
		}
		ID3D11DepthStencilView * d3d11_depth_stencil_view = nullptr;
		if ( depth_stencil )
		{
			video_depth_stencil_d3d11_c * depth_stencil_d3d11 = dynamic_cast< video_depth_stencil_d3d11_c * >( depth_stencil );
			assert( depth_stencil_d3d11 );
			assert( depth_stencil_d3d11->_d3d11_texture2darray_slice_dsvs );
			assert( depth_stencil_slice >= 0 && depth_stencil_slice < depth_stencil_d3d11->_array_slice_count );
			if ( width == -1 )
			{
				width = depth_stencil_d3d11->_width;
				height = depth_stencil_d3d11->_height;
			}
			else
			{
				assert( depth_stencil_d3d11->_width == width );
				assert( depth_stencil_d3d11->_height == height );
			}
			d3d11_depth_stencil_view = depth_stencil_d3d11->_d3d11_texture2darray_slice_dsvs[ depth_stencil_slice ];
		}
		_members->d3d11_device_context->OMSetRenderTargets( 4, d3d11_render_target_views, d3d11_depth_stencil_view );

		if ( width != -1 )
		{
			D3D11_VIEWPORT view_port;
			view_port.TopLeftX = 0.0f;
			view_port.TopLeftY = 0.0f;
			view_port.Width = static_cast< FLOAT >( width );
			view_port.Height = static_cast< FLOAT >( height );
			view_port.MinDepth = 0.0f;
			view_port.MaxDepth = 1.0f;
			_members->d3d11_device_context->RSSetViewports( 1, &view_port );
		}
	}

	/*
	void_c video_interface_d3d11_c::bind_target( sint32_c textures_count, video_texture_c * * textures, video_depth_stencil_c * depth_stencil )
	{
		assert( textures_count >= 0 && textures_count <= 4 );

		ID3D11RenderTargetView * d3d11_render_target_views[ 4 ] = {};
		for ( sint32_c i = 0; i < textures_count; i++ )
		{
			if ( textures && textures[ i ] )
			{
				video_texture_d3d11_c * texture2d_d3d11 = reinterpret_cast< video_texture_d3d11_c * >( textures[ i ] );
				assert( texture2d_d3d11->_target_enable );
				d3d11_render_target_views[ i ] = texture2d_d3d11->_d3d11_texture2d_rtv;
			}
		}

		ID3D11DepthStencilView * d3d11_depth_stencil_view = nullptr;
		if ( depth_stencil )
		{
			video_depth_stencil_d3d11_c * depth_stencil_d3d11 = reinterpret_cast< video_depth_stencil_d3d11_c * >( depth_stencil );
			d3d11_depth_stencil_view = depth_stencil_d3d11->_d3d11_texture2d_dsv;
		}

		_members->d3d11_device->OMSetRenderTargets( textures_count, d3d11_render_target_views, d3d11_depth_stencil_view );
	}

	void_c video_interface_d3d11_c::bind_target_slice( video_texture_c * texture, sint32_c array_slice_index, video_depth_stencil_c * depth_stencil, sint32_c depth_stencil_slice_index )
	{
		ID3D11RenderTargetView * d3d11_render_target_views[ 1 ] = {};
		if ( texture )
		{
			video_texture_d3d11_c * texture2d_d3d11 = reinterpret_cast< video_texture_d3d11_c * >( texture );
			if ( array_slice_index >= 0 )
			{
				assert( array_slice_index < texture2d_d3d11->get_array_slice_count() );
				d3d11_render_target_views[ 0 ] = texture2d_d3d11->_d3d11_texture2darray_slice_rtvs[ array_slice_index ];
			}
			else
			{
				assert( texture2d_d3d11->get_array_slice_count() == 1 );
				d3d11_render_target_views[ 0 ] = texture2d_d3d11->_d3d11_texture2d_rtv;
			}
		}

		ID3D11DepthStencilView * d3d11_depth_stencil_view = nullptr;
		if ( depth_stencil )
		{
			video_depth_stencil_d3d11_c * depth_stencil_d3d11 = reinterpret_cast< video_depth_stencil_d3d11_c * >( depth_stencil );
			if ( depth_stencil_slice_index >= 0 )
			{
				assert( depth_stencil_slice_index < depth_stencil_d3d11->get_array_count() );
				d3d11_depth_stencil_view = depth_stencil_d3d11->_d3d11_texture2d_array_slice_dsvs[ depth_stencil_slice_index ];
			}
			else
			{
				assert( depth_stencil_d3d11->get_array_count() == 1 );
				d3d11_depth_stencil_view = depth_stencil_d3d11->_d3d11_texture2d_dsv;
			}
			d3d11_depth_stencil_view = depth_stencil_d3d11->_d3d11_texture2d_array_dsv;
		}

		_members->d3d11_device->OMSetRenderTargets( 1, d3d11_render_target_views, d3d11_depth_stencil_view );
	}

	void_c video_interface_d3d11_c::bind_target_arrays( sint32_c textures_count, video_texture_c * * textures, video_depth_stencil_c * depth_stencil )
	{
		assert( textures_count >= 0 && textures_count <= 4 );

		ID3D11RenderTargetView * d3d11_render_target_views[ 4 ] = { nullptr, nullptr, nullptr, nullptr };
		for ( sint32_c i = 0; i < textures_count; i++ )
		{
			if ( textures && textures[ i ] )
			{
				video_texture_d3d11_c * texture2d_d3d11 = reinterpret_cast< video_texture_d3d11_c * >( textures[ i ] );
				assert( texture2d_d3d11->_target_enable );
				d3d11_render_target_views[ i ] = texture2d_d3d11->_d3d11_texture2darray_rtv;
			}
		}

		ID3D11DepthStencilView * d3d11_depth_stencil_view = nullptr;
		if ( depth_stencil )
		{
			video_depth_stencil_d3d11_c * depth_stencil_d3d11 = reinterpret_cast< video_depth_stencil_d3d11_c * >( depth_stencil );
			d3d11_depth_stencil_view = depth_stencil_d3d11->_d3d11_texture2d_array_dsv;
		}

		_members->d3d11_device->OMSetRenderTargets( textures_count, d3d11_render_target_views, d3d11_depth_stencil_view );
	}

	void_c video_interface_d3d11_c::bind_target_slice( video_texture_c * texture, sint32_c slice_index, video_depth_stencil_c * depth_stencil )
	{
		sint32_c TargetWidth = -1;
		sint32_c TargetHeight = -1;
		ID3D11RenderTargetView * rtvs[] = { nullptr };
		if ( texture )
		{
			video_texture_d3d11_c * TextureDirect3D10Instance = reinterpret_cast< video_texture_d3d11_c * >( texture );
			assert( TextureDirect3D10Instance->_target_enable );
			if ( TargetWidth == -1 )
			{
				TargetWidth = TextureDirect3D10Instance->get_width();
				TargetHeight = TextureDirect3D10Instance->get_height();
			}
			else
			{
				assert( TargetWidth == TextureDirect3D10Instance->get_width() );
				assert( TargetHeight == TextureDirect3D10Instance->get_height() );
			}
			assert( slice_index >= 0 && slice_index < TextureDirect3D10Instance->get_array_count() );
			rtvs[ 0 ] = TextureDirect3D10Instance->_Texture2DRtvSlices[ slice_index ];
		}

		ID3D11DepthStencilView * dsv = nullptr;
		video_depth_stencil_d3d11_c * DepthStencilDirect3D10Instance = reinterpret_cast< video_depth_stencil_d3d11_c * >( depth_stencil );
		if ( DepthStencilDirect3D10Instance != 0 )
		{
			dsv = DepthStencilDirect3D10Instance->_Texture2DDSV;
			if ( TargetWidth == -1 )
			{
				TargetWidth = DepthStencilDirect3D10Instance->get_width();
				TargetHeight = DepthStencilDirect3D10Instance->get_height();
			}
			else
			{
				assert( TargetWidth == DepthStencilDirect3D10Instance->get_width() );
				assert( TargetHeight == DepthStencilDirect3D10Instance->get_height() );
			}
		}

		members->d3d11_device->OMSetRenderTargets( 1, rtvs, dsv );
	}
	*/

	void_c video_interface_d3d11_c::bind_rasterizer_view_port_rectangle( sint32_c left, sint32_c top, sint32_c right, sint32_c bottom, float32_c depth_minimum, float32_c depth_maximum )
	{
		D3D11_VIEWPORT view_port;
		view_port.TopLeftX = static_cast< FLOAT >( left );
		view_port.TopLeftY = static_cast< FLOAT >( top );
		view_port.Width = static_cast< FLOAT >( right - left );
		view_port.Height = static_cast< FLOAT >( bottom - top );
		view_port.MinDepth = static_cast< FLOAT >( depth_minimum );
		view_port.MaxDepth = static_cast< FLOAT >( depth_maximum );
		_members->d3d11_device_context->RSSetViewports( 1, &view_port );
	}

	void_c video_interface_d3d11_c::bind_rasterizer_cull_fill_state( video_cull_type_e cull_mode, video_fill_type_e fill_mode )
	{
		switch ( cull_mode )
		{
			case video_cull_type_e_back:
				switch ( fill_mode )
				{
					case video_fill_type_e_face:
						_members->d3d11_device_context->RSSetState( _members->d3d11_rasterizer_state_cull_back_fill_face );
						break;
					case video_fill_type_e_edge:
						_members->d3d11_device_context->RSSetState( _members->d3d11_rasterizer_state_cull_back_fill_edge );
						break;
					case video_fill_type_e_point:
						_members->d3d11_device_context->RSSetState( _members->d3d11_rasterizer_state_cull_back_fill_point );
						break;
				}
				break;
			case video_cull_type_e_none:
				switch ( fill_mode )
				{
					case video_fill_type_e_face:
						_members->d3d11_device_context->RSSetState( _members->d3d11_rasterizer_state_cull_none_fill_face );
						break;
					case video_fill_type_e_edge:
						_members->d3d11_device_context->RSSetState( _members->d3d11_rasterizer_state_cull_none_fill_edge );
						break;
					case video_fill_type_e_point:
						_members->d3d11_device_context->RSSetState( _members->d3d11_rasterizer_state_cull_none_fill_point );
						break;
				}
				break;
			case video_cull_type_e_front:
				switch ( fill_mode )
				{
					case video_fill_type_e_face:
						_members->d3d11_device_context->RSSetState( _members->d3d11_rasterizer_state_cull_front_fill_face );
						break;
					case video_fill_type_e_edge:
						_members->d3d11_device_context->RSSetState( _members->d3d11_rasterizer_state_cull_front_fill_edge );
						break;
					case video_fill_type_e_point:
						_members->d3d11_device_context->RSSetState( _members->d3d11_rasterizer_state_cull_front_fill_point );
						break;
				}
				break;
		}
	}

	void_c video_interface_d3d11_c::bind_rasterizer_depth_stencil_state( video_compare_function_e depth_compare_mode, boolean_c depth_write_enable )
	{
		switch ( depth_compare_mode )
		{
			case video_compare_function_e_disable:
				_members->d3d11_device_context->OMSetDepthStencilState( _members->d3d11_depth_stencil_state_disable, 0 );
				break;
			case video_compare_function_e_never:
				_members->d3d11_device_context->OMSetDepthStencilState( depth_write_enable ? _members->d3d11_depth_stencil_state_never : _members->d3d11_depth_stencil_state_never_write_disabled, 0 );
				break;
			case video_compare_function_e_always:
				_members->d3d11_device_context->OMSetDepthStencilState( depth_write_enable ? _members->d3d11_depth_stencil_state_always : _members->d3d11_depth_stencil_state_always_write_disabled, 0 );
				break;
			case video_compare_function_e_equal:
				_members->d3d11_device_context->OMSetDepthStencilState( depth_write_enable ? _members->d3d11_depth_stencil_state_equal : _members->d3d11_depth_stencil_state_equal_write_disabled, 0 );
				break;
			case video_compare_function_e_not_equal:
				_members->d3d11_device_context->OMSetDepthStencilState( depth_write_enable ? _members->d3d11_depth_stencil_state_not_equal : _members->d3d11_depth_stencil_state_not_equal_write_disabled, 0 );
				break;
			case video_compare_function_e_less:
				_members->d3d11_device_context->OMSetDepthStencilState( depth_write_enable ? _members->d3d11_depth_stencil_state_less : _members->d3d11_depth_stencil_state_less_write_disabled, 0 );
				break;
			case video_compare_function_e_less_equal:
				_members->d3d11_device_context->OMSetDepthStencilState( depth_write_enable ? _members->d3d11_depth_stencil_state_less_equal : _members->d3d11_depth_stencil_state_less_equal_write_disabled, 0 );
				break;
			case video_compare_function_e_greater:
				_members->d3d11_device_context->OMSetDepthStencilState( depth_write_enable ? _members->d3d11_depth_stencil_state_greater : _members->d3d11_depth_stencil_state_greater_write_disabled, 0 );
				break;
			case video_compare_function_e_greater_equal:
				_members->d3d11_device_context->OMSetDepthStencilState( depth_write_enable ? _members->d3d11_depth_stencil_state_greater_equal : _members->d3d11_depth_stencil_state_greater_equal_write_disabled, 0 );
				break;
		}
	}

	void_c video_interface_d3d11_c::bind_rasterizer_blend_state( video_blend_type_e blend_type )
	{
		static const FLOAT blend_factor[ 4 ] = { 1.0f, 1.0f, 1.0f, 1.0f };
		static const UINT sample_mask = 0xFFFFFFFF;
		switch ( blend_type )
		{
			case video_blend_type_e_set:
				_members->d3d11_device_context->OMSetBlendState( _members->d3d11_blend_state_set, blend_factor, sample_mask );
				break;
			case video_blend_type_e_mix:
				_members->d3d11_device_context->OMSetBlendState( _members->d3d11_blend_state_mix, blend_factor, sample_mask );
				break;
			case video_blend_type_e_add:
				_members->d3d11_device_context->OMSetBlendState( _members->d3d11_blend_state_add, blend_factor, sample_mask );
				break;
			case video_blend_type_e_sub:
				_members->d3d11_device_context->OMSetBlendState( _members->d3d11_blend_state_sub, blend_factor, sample_mask );
				break;
			case video_blend_type_e_min:
				_members->d3d11_device_context->OMSetBlendState( _members->d3d11_blend_state_min, blend_factor, sample_mask );
				break;
			case video_blend_type_e_max:
				_members->d3d11_device_context->OMSetBlendState( _members->d3d11_blend_state_max, blend_factor, sample_mask );
				break;
		}
	}

	void_c video_interface_d3d11_c::bind_vertex_buffers( sint32_c vertex_buffers_count, video_vertex_buffer_c * * vertex_buffers )
	{
		assert( vertex_buffers_count >= 0 && vertex_buffers_count <= 2 );
		_members->bound_vertex_validated = false;
		_members->bound_vertex_buffers[ 0 ] = nullptr;
		_members->bound_vertex_buffers[ 1 ] = nullptr;
		ID3D11Buffer * buffers[ 2 ] = {};
		UINT strides[ 2 ] = {};
		UINT offsets[ 2 ] = {};
		if ( vertex_buffers )
		{
			for ( sint32_c i = 0; i < vertex_buffers_count; i++ )
			{
				video_vertex_buffer_d3d11_c * vertex_buffer_d3d11 = dynamic_cast< video_vertex_buffer_d3d11_c * >( vertex_buffers[ i ] );
				assert( vertex_buffer_d3d11 );
				_members->bound_vertex_buffers[ i ] = vertex_buffer_d3d11;
				buffers[ i ] = vertex_buffer_d3d11->_d3d11_buffer;
				strides[ i ] = vertex_buffer_d3d11->_vertex_format.stride;
				offsets[ i ] = 0;
			}
		}
		_members->d3d11_device_context->IASetVertexBuffers( 0, 2, buffers, strides, offsets );
	}

	void_c video_interface_d3d11_c::bind_index_buffer( video_index_buffer_c * index_buffer )
	{
		ID3D11Buffer * buffer = nullptr;
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		if ( index_buffer )
		{
			video_index_buffer_d3d11_c * index_buffer_d3d11 = reinterpret_cast< video_index_buffer_d3d11_c * >( index_buffer );
			buffer = index_buffer_d3d11->_d3d11_buffer;
			format = index_buffer_d3d11->_dxgi_format;
		}
		_members->d3d11_device_context->IASetIndexBuffer( buffer, format, 0 );
	}

	void_c video_interface_d3d11_c::bind_primitive_type( video_primitive_type_e primitive_type )
	{
		_members->d3d11_device_context->IASetPrimitiveTopology( d3d11_helpers::get_primitive_topology( primitive_type ) );
	}

	void_c video_interface_d3d11_c::bind_vertex_shader( video_vertex_shader_c * vertex_shader )
	{
		_members->bound_vertex_validated = false;
		_members->bound_vertex_shader = nullptr;
		if ( vertex_shader )
		{
			video_vertex_shader_d3d11_c * vertex_shader_d3d11 = reinterpret_cast< video_vertex_shader_d3d11_c * >( vertex_shader );
			assert( vertex_shader_d3d11 );
			_members->bound_vertex_shader = vertex_shader_d3d11;
			_members->d3d11_device_context->IASetInputLayout( vertex_shader_d3d11->_d3d11_input_layout );
			_members->d3d11_device_context->VSSetShader( vertex_shader_d3d11->_d3d11_vertex_shader, NULL, 0 );
			_members->d3d11_device_context->GSSetShader( vertex_shader_d3d11->_d3d11_geometry_shader_output, NULL, 0 );
		}
		else
		{
			_members->d3d11_device_context->IASetInputLayout( nullptr );
			_members->d3d11_device_context->VSSetShader( nullptr, NULL, 0 );
			_members->d3d11_device_context->GSSetShader( nullptr, NULL, 0 );
		}
	}

	void_c video_interface_d3d11_c::bind_vertex_shader_constant_buffers( sint32_c bind_index, sint32_c constant_buffers_count, video_constant_buffer_c * * constant_buffers )
	{
		assert( constant_buffers_count <= 14 );
		assert( bind_index + constant_buffers_count <= 14 );
		ID3D11Buffer * temp[ 14 ] = {};

		for ( sint32_c i = 0; i < constant_buffers_count; i++ )
		{
			if ( constant_buffers && constant_buffers[ i ] )
			{
				temp[ i ] = reinterpret_cast<video_constant_buffer_d3d11_c *>( constant_buffers[ i ] )->_d3d11_buffer;
			}
		}

		_members->d3d11_device_context->VSSetConstantBuffers( bind_index, constant_buffers_count, temp );
	}

	void_c video_interface_d3d11_c::bind_vertex_shader_texture_buffers( sint32_c bind_index, sint32_c texture_buffers_count, video_texture_buffer_c * * texture_buffers )
	{
		assert( texture_buffers_count <= 16 );
		assert( bind_index + texture_buffers_count <= 128 );
		ID3D11ShaderResourceView * temp[ 16 ] = {};

		for ( sint32_c i = 0; i < texture_buffers_count; i++ )
		{
			if ( texture_buffers && texture_buffers[ i ] )
			{
				temp[ i ] = reinterpret_cast< video_texture_buffer_d3d11_c * >( texture_buffers[ i ] )->_d3d11_buffer_srv;
			}
		}

		_members->d3d11_device_context->VSSetShaderResources( bind_index, texture_buffers_count, temp );
	}


	//void_c video_interface_d3d11_c::bind_vertex_shader_textures( sint32_c bind_index, sint32_c textures_count, video_texture_c * * textures )
	//{
	//	assert( textures_count <= 16 );
	//	assert( bind_index + textures_count <= 128 );
	//	ID3D11ShaderResourceView * temp[ 16 ];

	//	for ( sint32_c i = 0; i < textures_count; i++ )
	//	{
	//		temp[ i ] = 0;
	//		if ( textures && textures[ i ] )
	//		{
	//			temp[ i ] = reinterpret_cast< video_texture_d3d11_c * >( textures[ i ] )->_d3d11_texture2d_srv;
	//		}
	//	}

	//	_members->d3d11_device->VSSetShaderResources( bind_index, textures_count, temp );
	//}

	void_c video_interface_d3d11_c::bind_geometry_shader( video_geometry_shader_c * geometry_shader )
	{
		video_geometry_shader_d3d11_c * geometry_shader_d3d11 = reinterpret_cast< video_geometry_shader_d3d11_c * >( geometry_shader );
		_members->d3d11_device_context->GSSetShader( geometry_shader_d3d11 ? geometry_shader_d3d11->_d3d11_geometry_shader : 0, NULL, 0 );
	}

	void_c video_interface_d3d11_c::bind_geometry_shader_constant_buffers( sint32_c bind_index, sint32_c constant_buffers_count, video_constant_buffer_c * * constant_buffers )
	{
		assert( constant_buffers_count <= 14 );
		assert( bind_index + constant_buffers_count <= 14 );
		ID3D11Buffer * temp[ 14 ] = {};

		for ( sint32_c i = 0; i < constant_buffers_count; i++ )
		{
			if ( constant_buffers && constant_buffers[ i ] )
			{
				temp[ i ] = reinterpret_cast< video_constant_buffer_d3d11_c * >( constant_buffers[ i ] )->_d3d11_buffer;
			}
		}

		_members->d3d11_device_context->GSSetConstantBuffers( bind_index, constant_buffers_count, temp );
	}

	void_c video_interface_d3d11_c::bind_geometry_shader_texture_buffers( sint32_c bind_index, sint32_c texture_buffers_count, video_texture_buffer_c * * texture_buffers )
	{
		assert( texture_buffers_count <= 16 );
		assert( bind_index + texture_buffers_count <= 128 );
		ID3D11ShaderResourceView * temp[ 16 ] = {};

		for ( sint32_c i = 0; i < texture_buffers_count; i++ )
		{
			if ( texture_buffers && texture_buffers[ i ] )
			{
				temp[ i ] = reinterpret_cast< video_texture_buffer_d3d11_c * >( texture_buffers[ i ] )->_d3d11_buffer_srv;
			}
		}

		_members->d3d11_device_context->GSSetShaderResources( bind_index, texture_buffers_count, temp );
	}


	//void_c video_interface_d3d11_c::bind_geometry_shader_textures( sint32_c bind_index, sint32_c textures_count, video_texture_c * * textures )
	//{
	//	assert( textures_count <= 16 );
	//	assert( bind_index + textures_count <= 128 );
	//	ID3D11ShaderResourceView * temp[ 16 ];

	//	for ( sint32_c i = 0; i < textures_count; i++ )
	//	{
	//		temp[ i ] = nullptr;
	//		if ( textures && textures[ i ] )
	//		{
	//			temp[ i ] = reinterpret_cast< video_texture_d3d11_c * >( textures[ i ] )->_d3d11_texture2d_srv;
	//		}
	//	}

	//	_members->d3d11_device->GSSetShaderResources( bind_index, textures_count, temp );
	//}

	void_c video_interface_d3d11_c::bind_pixel_shader( video_pixel_shader_c * pixel_shader )
	{
		video_pixel_shader_d3d11_c * pixel_shader_d3d11 = dynamic_cast< video_pixel_shader_d3d11_c * >( pixel_shader );
		assert( pixel_shader_d3d11 );
		_members->d3d11_device_context->PSSetShader( pixel_shader_d3d11 ? pixel_shader_d3d11->_d3d11_pixel_shader : 0, NULL, 0 );
	}

	void_c video_interface_d3d11_c::bind_pixel_shader_constant_buffers( sint32_c bind_index, sint32_c constant_buffers_count, video_constant_buffer_c * * constant_buffers )
	{
		assert( constant_buffers_count <= 14 );
		assert( bind_index + constant_buffers_count <= 14 );
		ID3D11Buffer * temp[ 14 ] = {};

		for ( sint32_c i = 0; i < constant_buffers_count; i++ )
		{
			if ( constant_buffers && constant_buffers[ i ] )
			{
				temp[ i ] = reinterpret_cast< video_constant_buffer_d3d11_c * >( constant_buffers[ i ] )->_d3d11_buffer;
			}
		}

		_members->d3d11_device_context->PSSetConstantBuffers( bind_index, constant_buffers_count, temp );
	}

	void_c video_interface_d3d11_c::bind_pixel_shader_texture_buffers( sint32_c bind_index, sint32_c texture_buffers_count, video_texture_buffer_c * * texture_buffers )
	{
		assert( texture_buffers_count <= 16 );
		assert( bind_index + texture_buffers_count <= 128 );
		ID3D11ShaderResourceView * temp[ 16 ] = {};

		for ( sint32_c i = 0; i < texture_buffers_count; i++ )
		{
			if ( texture_buffers && texture_buffers[ i ] )
			{
				temp[ i ] = reinterpret_cast< video_texture_buffer_d3d11_c * >( texture_buffers[ i ] )->_d3d11_buffer_srv;
			}
		}

		_members->d3d11_device_context->PSSetShaderResources( bind_index, texture_buffers_count, temp );
	}

	void_c video_interface_d3d11_c::bind_pixel_shader_textures( sint32_c bind_index, sint32_c textures_count, video_texture_c * * textures, video_texture_type_e * textures_types )
	{
		assert( textures_count > 0 && textures_count <= 32 );
		static ID3D11ShaderResourceView * buffer[ 32 ] = {};

		for ( sint32_c i = 0; i < textures_count; i++ )
		{
			buffer[ i ] = nullptr;
			if ( textures && textures[ i ] )
			{
				assert( textures_types[ i ] > video_texture_type_e_none && textures_types[ i ] <= video_texture_type_e_texturecubearray );
				video_texture_d3d11_c * texture2 = dynamic_cast< video_texture_d3d11_c * >( textures[ i ] );
				assert( texture2 );
				if ( textures_types[ i ] == video_texture_type_e_texture2d )
				{
					assert( texture2->_d3d11_texture2d_srv );
					assert( texture2->_multi_sample_count == 1 );
					buffer[ i ] = texture2->_d3d11_texture2d_srv;
				}
				else if ( textures_types[ i ] == video_texture_type_e_texture2dms )
				{
					assert( texture2->_d3d11_texture2d_srv );
					assert( texture2->_multi_sample_count > 1 );
					buffer[ i ] = texture2->_d3d11_texture2d_srv;
				}
				else if ( textures_types[ i ] == video_texture_type_e_texture2darray )
				{
					assert( texture2->_d3d11_texture2darray_srv );
					assert( texture2->_multi_sample_count == 1 );
					buffer[ i ] = texture2->_d3d11_texture2darray_srv;
				}
				else if ( textures_types[ i ] == video_texture_type_e_texture2dmsarray )
				{
					assert( texture2->_d3d11_texture2darray_srv );
					assert( texture2->_multi_sample_count > 1 );
					buffer[ i ] = texture2->_d3d11_texture2darray_srv;
				}
				else if ( textures_types[ i ] == video_texture_type_e_texturecube )
				{
					assert( texture2->_d3d11_texturecube_srv );
					buffer[ i ] = texture2->_d3d11_texturecube_srv;
				}
				else if ( textures_types[ i ] == video_texture_type_e_texturecubearray )
				{
					assert( texture2->_d3d11_texturecubearray_srv );
					buffer[ i ] = texture2->_d3d11_texturecubearray_srv;
				}
			}
		}
		_members->d3d11_device_context->PSSetShaderResources( bind_index, textures_count, buffer );
	}

	void_c video_interface_d3d11_c::draw( sint32_c vertex_start, sint32_c vertex_count )
	{
		if ( _members->bound_vertex_validated == false )
		{
			_validate_vertex_bindings();
		}
		_members->d3d11_device_context->Draw( vertex_count, vertex_start );
	}

	void_c video_interface_d3d11_c::draw_indexed( sint32_c index_start, sint32_c index_count, sint32_c vertex_base )
	{
		if ( _members->bound_vertex_validated == false )
		{
			_validate_vertex_bindings();
		}
		_members->d3d11_device_context->DrawIndexed( index_count, index_start, vertex_base );
	}

	void_c video_interface_d3d11_c::_validate_vertex_bindings()
	{
		// validate that input layouts of bound vertex buffers and vertex shader match.
		assert( _members->bound_vertex_buffers[ _members->bound_vertex_shader->_input_vertex_layout.vertex_formats_count - 1 ] );
		assert( _members->bound_vertex_buffers[ _members->bound_vertex_shader->_input_vertex_layout.vertex_formats_count ] == nullptr );
		for ( sint32_c i = 0; i < _members->bound_vertex_shader->_input_vertex_layout.vertex_formats_count; i++ )
		{
			assert( *_members->bound_vertex_shader->_input_vertex_layout.vertex_formats[ i ] == _members->bound_vertex_buffers[ i ]->_vertex_format );
		}
		_members->bound_vertex_validated = true;
	}

}

#endif
