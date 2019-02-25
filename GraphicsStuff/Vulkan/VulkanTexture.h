#pragma once
#include <vulkan/vulkan.h>
#include "../RHI/Texture.h"

namespace Vulkan {
	class VulkanBuffer;
	class VulkanAllocation;

	class VulkanTexture final : public RHI::Texture
	{
	public:

		VulkanTexture();
		~VulkanTexture();

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
		b8 Create(RHI::RHIContext* pContext, const RHI::TextureDesc& desc, RHI::CommandList* pCommandList) override final;

		/**
		 * Create a default rendertarget
		 * @param[in] pContext	RHI context
		 * @param[in] image		Vulkan image
		 * @param[in] layout	Vulkan image layout
		 * @param[in] width		Width
		 * @param[in] height	Height
		 * @param[in] format	Format
		 * @param[in] samples	Sample count
		 * @param[in] flags		Texture flags
		 * @return				True if the texture was created successfully, false otherwise
		 */
		b8 Create(RHI::RHIContext* pContext, VkImage image, VkImageLayout layout, u32 width, u32 height, PixelFormat format, RHI::SampleCount samples, RHI::TextureFlags flags);

		/**
		 * Destroy the texture
		 */
		b8 Destroy() override final;


		/**
		 * Write data to the image
		 * @param[in] region		Region to write to
		 * @param[in] size			Size of data in bytes
		 * @param[in] pData			Raw image data
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 */
		b8 Write(const RHI::TextureRegion& region, u64 size, void* pData, RHI::CommandList* pCommandList = nullptr) override final;
		/**
		 * Read data from the image
		 * @param[in] region		Region to write to
		 * @param[in] size			Size of read buffer in bytes
		 * @param[in] pData			Buffer to read to
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 */
		b8 Read(const RHI::TextureRegion& region, u64 size, void* pData, RHI::CommandList* pCommandList = nullptr) override final;
		/**
		 * Copy a texture to the texture
		 * @param[in] pTexture		Texture to copy from
		 * @param[in] region		Region to copy
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 */
		b8 Copy(Texture* pTexture, const RHI::TextureCopyRegion& region, RHI::CommandList* pCommandList = nullptr) override final;
		/**
		 * Copy a buffer to the texture
		 * @param[in] pBuffer		Buffer to copy from
		 * @param[in] region		Region to copy
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 */
		b8 Copy(RHI::Buffer* pBuffer, const RHI::TextureBufferCopyRegion& region, RHI::CommandList* pCommandList = nullptr) override final;


		/**
		 * Get the vulkan image
		 * @return	Vulkan image
		 */
		VkImage GetImage() { return m_Image; }
		/**
		 * Get the vulkan image view
		 * @return	Vulkan image view
		 */
		VkImageView GetImageView() { return m_View; }
		/**
		 * Get the owning vulkan queue family
		 * @return	Vulkan queue family
		 */
		u32 GetOwningQueueFamily() const { return m_OwningQueueFamily; }
		/**
		 * Set the owning vulkan queue family
		 * @param[in] queueFamily	Vulkan queue family
		 */
		void SetOwningQueueFamily(u32 queueFamily) { m_OwningQueueFamily = queueFamily; }

	private:

		/**
		 * Internal creation function
		 * @param[in] pCommandList	Command list used for layour transition, if nullptr, default command list will be used
		 * @return					True if the texture was created successfully, false otherwise
		 */
		b8 CreateInternal(RHI::CommandList* pCommandList);

		VkImage m_Image;					/**< Vulkan image */
		VkImageView m_View;					/**< Vulkan image view */
		VulkanAllocation* m_pAllocation;	/**< Vulkan memory allocation */
		u32 m_OwningQueueFamily;

		VulkanBuffer* m_pStagingBuffer;		/**< Staging buffer */

		b8 m_OwnsImage;
	};

}
