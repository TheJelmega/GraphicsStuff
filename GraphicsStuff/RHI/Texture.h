// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// Texture.h: Texture
#pragma once
#include "../General/TypesAndMacros.h"
#include "PixelFormat.h"
#include "RHICommon.h"

namespace RHI {
	class RHIContext;
	class Buffer;
	class CommandList;

	/**
	* Texture description
	*/
	struct TextureDesc
	{
		u32 width						= 1;						/**< Image with */
		u32 height						= 1;						/**< Image height */
		u32 depth						= 1;						/**< Image depth */
		u32 layerCount					= 1;						/**< Array layers */
		u8 mipLevels					= 1;						/**< Mip levels */
		PixelFormat format				= PixelFormat();			/**< Image format */
		TextureType type				= TextureType::Tex1D;		/**< Texture type */
		TextureFlags flags				= TextureFlags::None;		/**< Texture flags */
		SampleCount samples				= SampleCount::Sample1;		/**< Sample count */
		TextureLayout layout			= TextureLayout::Unknown;	/**< Texture layout */
	};

	/**
	 * Texture
	 * @note To be able to use a texture as a rendertarget, you need to use RenderTarget
	 * @see RenderTarget
	 */
	class Texture
	{
	public:
		Texture();
		virtual ~Texture();

		/**
		 * Create a 2D texture
		 * @param[in] pContext		RHI context
		 * @param[in] desc			Texture description
		 * @param[in] pCommandList	Command list used for layour transition, if nullptr, default command list will be used
		 * @return					True if the texture was created successfully, false otherwise
		 * @note					A dynamic texture has some restrictions:
		 *							- Only 1 array layer is supported
		 *							- Only 1 mip level is supported
		 */
		virtual b8 Create(RHI::RHIContext* pContext, const TextureDesc& desc, CommandList* pCommandList) = 0;

		/**
		 * Destroy the texture
		 */
		virtual b8 Destroy() = 0;

		// TODO: Add write flags
		/**
		 * Write data to the image
		 * @param[in] region		Region to write to
		 * @param[in] size			Size of data in bytes
		 * @param[in] pData			Raw image data
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 */
		virtual b8 Write(const TextureRegion& region, u64 size, void* pData, CommandList* pCommandList = nullptr) = 0;
		/**
		 * Read data from the image
		 * @param[in] region		Region to write to
		 * @param[in] size			Size of read buffer in bytes
		 * @param[in] pData			Buffer to read to
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 */
		virtual b8 Read(const TextureRegion& region, u64 size, void* pData, CommandList* pCommandList = nullptr) = 0;
		/**
		 * Copy a texture to the texture
		 * @param[in] pTexture		Texture to copy from
		 * @param[in] region		Region to copy
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 */
		virtual b8 Copy(Texture* pTexture, const TextureCopyRegion& region, CommandList* pCommandList = nullptr) = 0;
		/**
		 * Copy a buffer to the texture
		 * @param[in] pBuffer		Buffer to copy from
		 * @param[in] region		Region to copy
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 */
		virtual b8 Copy(Buffer* pBuffer, const TextureBufferCopyRegion& region, CommandList* pCommandList = nullptr) = 0;

		/**
		 * Get the texture width
		 * @return	Texture with
		 */
		u32 GetWidth() const { return m_Desc.width; }
		/**
		 * Get the texture height
		 * @return	Texture height
		 */
		u32 GetHeight() const { return m_Desc.height; }
		/**
		 * Get the texture depth
		 * @return	Texture depth
		 */
		u32 GetDepth() const { return m_Desc.depth; }
		/**
		 * Get the number of array layers
		 * @return	Number of array layers
		 */
		u32 GetLayerCount() const { return m_Desc.layerCount; }
		/**
		 * Get the number of mip levels
		 * @return	Number of mip levels
		 */
		u32 GetMipLevels() const { return m_Desc.mipLevels; }
		/**
		 * Get the texture format
		 * @return	Texture format
		 */
		PixelFormat GetFormat() const { return m_Desc.format; }
		/**
		 * Get the texture flags
		 * @return	Texture flags
		 */
		TextureFlags GetFlags() const { return m_Desc.flags; }
		/**
		* Get the texture type
		* @return	Texture type
		*/
		TextureType GetType() const { return m_Desc.type; }
		/**
		* Get the sample count
		* @return	Sample count
		*/
		SampleCount GetSampleCount() const { return m_Desc.samples;  }
		/**
		* Get the sample count
		* @return	Sample count
		*/
		TextureLayout GetLayout() const { return m_Desc.layout; }

		/**
		* Check if the texture is a texture array
		* @return	True if the texture is a texture array, false otherwise
		*/
		b8 IsTextureArray() const { return m_Desc.layerCount > 1; }

		/**
		* [Internal] Set the texture layout
		* @param[in] layout		New layout
		* @note					This function is meant to only be used internally, changing this manually can cause issues in the RHI
		*/
		void SetLayout(TextureLayout layout) { m_Desc.layout = layout; }

		/**
		 * Get the size of the texture on the GPU
		 * @return	Size on the GPU
		 */
		u64 GetMemorySize() const { return m_MemorySize; }

	protected:
		RHI::RHIContext* m_pContext;		/**< RHI context */
		TextureDesc m_Desc;					/**< Texture description */
		u64 m_MemorySize;					/**< Size of the texture on the GPU */
	};

}