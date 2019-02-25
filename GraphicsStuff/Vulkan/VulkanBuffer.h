#pragma once
#include <vulkan/vulkan.h>
#include "VulkanMemory.h"
#include "../RHI/PixelFormat.h"
#include "../RHI/Buffer.h"

namespace Vulkan {
	
	class VulkanBuffer final : public RHI::Buffer
	{
	public:

		VulkanBuffer();
		~VulkanBuffer();

		/**
		 * Create a buffer
		 * @param[in] pContext		RHI context
		 * @param[in] type			Buffer type
		 * @param[in] size			Buffer size
		 * @param[in] flags			Buffer flags
		 * @param[in] format		Buffer data format (texel buffers only)
		 * @return					True if the buffer was created successfully, false otherwise
		 */
		b8 Create(RHI::RHIContext* pContext, RHI::BufferType type, u64 size, RHI::BufferFlags flags, PixelFormat format = PixelFormat()) override final;
		/**
		 * Create a vertex buffer
		 * @param[in] pContext		RHI context
		 * @param[in] vertexCount	Number of vertices
		 * @param[in] vertexSize	Size of a vertex
		 * @param[in] flags			Buffer flags
		 * @return					True if the buffer was created successfully, false otherwise
		 */
		b8 CreateVertexBuffer(RHI::RHIContext* pContext, u32 vertexCount, u16 vertexSize, RHI::BufferFlags flags) override final;
		/**
		 * Create a vertex buffer
		 * @param[in] pContext		RHI context
		 * @param[in] indexCount	Number of indices
		 * @param[in] indexType		Index type
		 * @param[in] flags			Buffer flags
		 * @return					True if the buffer was created successfully, false otherwise
		 */
		b8 CreateIndexBuffer(RHI::RHIContext* pContext, u32 indexCount, RHI::IndexType indexType, RHI::BufferFlags flags) override final;

		/**
		 * Detroy a buffer
		 * @return					True if the buffer was destroyed successfully, false otherwise
		 */
		virtual b8 Destroy() final;

		/**
		 * Write data to the buffer
		 * @param[in] offset		Offset in buffer to write to
		 * @param[in] size			Size of data in bytes
		 * @param[in] pData			Data to write
		 * @param[in] flags			Buffer write flags
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 * @return
		 */
		b8 Write(u64 offset, u64 size, void* pData, RHI::BufferWriteFlags flags = RHI::BufferWriteFlags::None, RHI::CommandList* pCommandList = nullptr) override final;
		/**
		 * Read data from the buffer
		 * @param[in] offset		Offset in buffer to read from
		 * @param[in] size			Size of data in bytes
		 * @param[inout] pData		Array to read to
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 * @return
		 */
		b8 Read(u64 offset, u64 size, void* pData, RHI::CommandList* pCommandList = nullptr) override final;
		/**
		 * Copy data from another buffer
		 * @param[in] pBuffer		Buffer to copy from
		 * @param[in] srcOffset		Offset in source buffer
		 * @param[in] dstOffset		Offset in this buffer
		 * @param[in] size			Size of data in bytes
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 * @return
		 */
		b8 Copy(Buffer* pBuffer, u64 srcOffset, u64 dstOffset, u64 size, RHI::CommandList* pCommandList = nullptr) override final;
		/**
		* Copy data from another buffer
		* @param[in] pBuffer		Buffer to copy from
		* @param[in] regions		Buffer copy regions
		* @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		* @return
		*/
		b8 Copy(Buffer* pBuffer, const std::vector<RHI::BufferCopyRegion>& regions, RHI::CommandList* pCommandList = nullptr) override final;
		/**
		 * Copy a texture to the buffer
		 * @param[in] pTexture		Texture to copy from
		 * @param[in] region		Copy region
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 * @return
		 */
		b8 Copy(RHI::Texture* pTexture, const RHI::TextureBufferCopyRegion& region, RHI::CommandList* pCommandList = nullptr) override final;
		/**
		 * Copy a texture to the buffer
		 * @param[in] pTexture		Texture to copy from
		 * @param[in] regions		Copy regions
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 * @return
		 */
		b8 Copy(RHI::Texture* pTexture, const std::vector<RHI::TextureBufferCopyRegion>& regions, RHI::CommandList* pCommandList = nullptr) override final;

		/**
		 * Get the vulkan buffer
		 * @return	Vulkan buffer
		 */
		VkBuffer GetBuffer() { return m_Buffer; }
		/**
		* Get the vulkan buffer view
		* @return	Vulkan buffer view
		*/
		VkBufferView GetBufferView() { return m_View; }

	private:
		VkBuffer m_Buffer;					/**< Vulkan buffer */
		VkBufferView m_View;				/**< Vulkan buffer view */
		VulkanAllocation* m_pAllocation;	/**< Memory allocation */
		VulkanBuffer* m_pStagingBuffer;		/**< Staging buffer for non-static/dynamic buffers */
	};

}
