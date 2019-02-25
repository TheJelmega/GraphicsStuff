#pragma once
#include "Texture.h"
#include "GpuInfo.h"

namespace RHI { namespace Helpers {
	
	/**
	* Get the GPU vendor as a string
	* @param[in] vendor	GPU vendor
	* @return				GPU vendor as a sting
	*/
	const char* GetGpuVendorString(GpuVendor vendor);
	/**
	 * Get the texture layout for a rendertarget type
	 * @param[in] type	Rendertarget type
	 * @return			Layout
	 */
	TextureLayout GetTextureLayoutFromRTType(RenderTargetType type);

	// ReSharper disable CppNonInlineVariableDefinitionInHeaderFile
	namespace Tables {
		
		static TextureLayout g_RTLayouts[u8(RenderTargetType::MultisampleResolve) + 1] = {
			TextureLayout::General,
			TextureLayout::ColorAttachment,
			TextureLayout::ColorAttachment,
			TextureLayout::DepthStencil,
			TextureLayout::ColorAttachment
		};

	}
	// ReSharper restore CppNonInlineVariableDefinitionInHeaderFile
} }
