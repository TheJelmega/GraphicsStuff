#pragma once
#include <vulkan/vulkan.h>
#include "VulkanMemory.h"
#include "../RHI/CommandList.h"

namespace Vulkan {
	
	class VulkanCommandList final : public RHI::CommandList
	{
	public:
		VulkanCommandList();
		~VulkanCommandList();

		/**
		 * Begin the command buffer
		 * @return	True if the command list was begin successfully, false otherwise
		 */
		b8 Begin() override final;
		/**
		 * End the command buffer
		 * @return	True if the command list was ended successfully, false otherwise
		 */
		b8 End() override final;

		/**
		 * Bind a pipeline
		 * @param[in] pPipeline	Pipeline to bind
		 */
		void BindPipeline(RHI::Pipeline* pPipeline) override final;

		/**
		 * Begin a render pass
		 * @param[in] pRenderPass	Render pass to begin
		 * @param[in] pFramebuffer	Framebuffer
		 */
		void BeginRenderPass(RHI::RenderPass* pRenderPass, RHI::Framebuffer* pFramebuffer) override final;
		/**
		 * End the current render pass
		 */
		void EndRenderPass() override final;

		/**
		* Bind a vertex buffer
		* @param[in] inputSlot		Input slot of the buffer
		* @param[in] pBuffer		Vertex buffer to bind
		* @param[in] offset		Offset in buffer
		*/
		void BindVertexBuffer(u16 inputSlot, RHI::Buffer* pBuffer, u64 offset) override final;
		/**
		* Bind multiple vertex buffers
		* @param[in] inputSlot		Input slot of the first buffer
		* @param[in] buffers		Vertex buffer to bind
		* @param[in] offsets	s	Offset in buffer
		*/
		void BindVertexBuffer(u16 inputSlot, const std::vector<RHI::Buffer*>& buffers, const std::vector<u64>& offsets) override final;
		/**
		 * Bind a vertex buffer
		 * @param[in] pBuffer		Vertex buffer to bind
		 * @param[in] offset		Offset in buffer
		 */
		void BindIndexBuffer(RHI::Buffer* pBuffer, u64 offset) override final;
		/**
		 * Bind a vertex buffer
		 * @param[in] pBuffer		Vertex buffer to bind
		 * @param[in] offset		Offset in buffer
		 * @param[in] type			Index type
		 * @note					Use this function when using a combined buffer (vertices, indices, etc. in same buffer)
		 */
		void BindIndexBuffer(RHI::Buffer* pBuffer, u64 offset, RHI::IndexType type) override final;
		/**
		 * Bind a descriptor set
		 * @param[in] firstSet	Index of first descriptor set
		 * @param[in] pSet		Descriptor set
		 * @note				Use this function when using a combined buffer (vertices, indices, etc. in same buffer)
		 */
		void BindDescriptorSets(u32 firstSet, RHI::DescriptorSet* pSet) override final;
		/**
		 * Bind a descriptor set
		 * @param[in] firstSet			Index of first descriptor set
		 * @param[in] pSet				Descriptor set
		 * @param[in] dynamicOffset		Dynamic offset (for dynamic descriptor set type)
		 * @note						Use this function when using a combined buffer (vertices, indices, etc. in same buffer)
		 */
		void BindDescriptorSets(u32 firstSet, RHI::DescriptorSet* pSet, u32 dynamicOffset) override final;
		/**
		 * Bind descriptor sets
		 * @param[in] firstSet			Index of first descriptor set
		 * @param[in] sets				Descriptor sets
		 * @note						Use this function when using a combined buffer (vertices, indices, etc. in same buffer)
		 */
		void BindDescriptorSets(u32 firstSet, const std::vector<RHI::DescriptorSet*>& sets) override final;
		/**
		 * Bind descriptor sets
		 * @param[in] firstSet			Index of first descriptor set
		 * @param[in] sets				Descriptor sets
		 * @param[in] dynamicOffsets	Dynamic offsets (for dynamic descriptor set types)
		 * @note						Use this function when using a combined buffer (vertices, indices, etc. in same buffer)
		 */
		void BindDescriptorSets(u32 firstSet, const std::vector<RHI::DescriptorSet*>& sets, const std::vector<u32>& dynamicOffsets) override final;

		/**
		 * Set the viewport
		 * @param[in] viewport	Viewport
		 */
		void SetViewport(RHI::Viewport& viewport) override final;
		/**
		 * Set the scissor rect
		 * @param[in] scissor	Scissor rect
		 */
		void SetScissor(RHI::ScissorRect& scissor) override final;

		/**
		 * Draw a number of vertices and instances
		 * @param[in] vertexCount		Amount of vertices to draw
		 * @param[in] instanceCount		Amount of instances to draw
		 * @param[in] firstVertex		Index of vertex to start drawing from
		 * @param[in] firstInstance		Index of instance to start drawing from
		 */
		void Draw(u32 vertexCount, u32 instanceCount = 1, u32 firstVertex = 0, u32 firstInstance = 0) override final;
		/**
		 * Draw a number of vertices and instances (indexed)
		 * @param[in] indexCount		Amount of indices to draw
		 * @param[in] instanceCount		Amount of instances to draw
		 * @param[in] firstIndex		Index of index to start drawing from
		 * @param[in] vertexOffset		Offset in vertex buffer (index n == vertex at offset + n)
		 * @param[in] firstInstance		Index of instance to start drawing from
		 */
		void DrawIndexed(u32 indexCount, u32 instanceCount = 1, u32 firstIndex = 0, u32 vertexOffset = 0, u32 firstInstance = 0) override final;

		/**
		* Copy data from one buffer to another
		* @param[in] pSrcBuffer	Buffer to copy from
		* @param[in] srcOffset		Offset in source buffer
		* @param[in] pDstBuffer	Buffer to copy to
		* @param[in] dstOffset		Offset in destination buffer
		* @param[in] size			Amount of bytes to copy
		* @note					While it's possible to use this function, it is preferable to use the buffer copy function, since this functions doesn't do additional checks on the buffers
		*							(make sure you know what you are doing!)
		*/
		void CopyBuffer(RHI::Buffer* pSrcBuffer, u64 srcOffset, RHI::Buffer* pDstBuffer, u64 dstOffset, u64 size) override final;
		/**
		 * Copy data from one buffer to another
		 * @param[in] pSrcBuffer	Buffer to copy from
		 * @param[in] pDstBuffer	Buffer to copy to
		 * @param[in] regions		Buffer copy regions
		 * @note					While it's possible to use this function, it is preferable to use the buffer copy function, since this functions doesn't do additional checks on the buffers
		 *							(make sure you know what you are doing!)
		 */
		void CopyBuffer(RHI::Buffer* pSrcBuffer, RHI::Buffer* pDstBuffer, const std::vector<RHI::BufferCopyRegion>& regions) override final;
		/**
		 * Copy data from one buffer to another
		 *@param[in] pSrcTex		Texture to copy from
		 * @param[in] pDstTex		Texture to copy to
		 * @param[in] region		Texture copy region
		 * @note					While it's possible to use this function, it is preferable to use the texture copy function, since this functions doesn't do additional checks on the buffers
		 *							(make sure you know what you are doing!)
		 */
		void CopyTexture(RHI::Texture* pSrcTex, RHI::Texture* pDstTex, const RHI::TextureCopyRegion& region) override final;
		/**
		 * Copy data from one buffer to another
		 * @param[in] pSrcTex		Texture to copy from
		 * @param[in] pDstTex		Texture to copy to
		 * @param[in] regions		Texture copy regions
		 * @note					While it's possible to use this function, it is preferable to use the texture copy function, since this functions doesn't do additional checks on the buffers
		 *							(make sure you know what you are doing!)
		 */
		void CopyTexture(RHI::Texture* pSrcTex, RHI::Texture* pDstTex, const std::vector<RHI::TextureCopyRegion>& regions) override final;
		/**
		 * Copy data from one buffer to another
		 * @param[in] pBuffer		Buffer to copy from
		 * @param[in] pTexture		Texture to copy to
		 * @param[in] region		Copy region
		 * @note					While it's possible to use this function, it is preferable to use the texure copy function, since this functions doesn't do additional checks on the buffers
		 *							(make sure you know what you are doing!)
		 */
		void CopyBufferToTexture(RHI::Buffer* pBuffer, RHI::Texture* pTexture, const RHI::TextureBufferCopyRegion& region) override final;
		/**
		 * Copy data from one buffer to another
		 * @param[in] pBuffer		Buffer to copy from
		 * @param[in] pTexture		Texture to copy to
		 * @param[in] regions		Copy regions
		 * @note					While it's possible to use this function, it is preferable to use the texure copy function, since this functions doesn't do additional checks on the buffers
		 *							(make sure you know what you are doing!)
		 */
		void CopyBufferToTexture(RHI::Buffer* pBuffer, RHI::Texture* pTexture, const std::vector<RHI::TextureBufferCopyRegion>& regions) override final;
		/**
		 * Copy data from one buffer to another
		 * @param[in] pTexture		Texture to copy from
		 * @param[in] pBuffer		Buffer to copy to
		 * @param[in] region		Copy region
		 * @note					While it's possible to use this function, it is preferable to use the texure copy function, since this functions doesn't do additional checks on the buffers
		 *							(make sure you know what you are doing!)
		 */
		void CopyTextureToBuffer(RHI::Texture* pTexture, RHI::Buffer* pBuffer, const RHI::TextureBufferCopyRegion& region) override final;
		/**
		 * Copy data from one buffer to another
		 * @param[in] pTexture		Texture to copy from
		 * @param[in] pBuffer		Buffer to copy to
		 * @param[in] regions		Copy regions
		 * @note					While it's possible to use this function, it is preferable to use the texure copy function, since this functions doesn't do additional checks on the buffers
		 *							(make sure you know what you are doing!)
		 */
		void CopyTextureToBuffer(RHI::Texture* pTexture, RHI::Buffer* pBuffer, const std::vector<RHI::TextureBufferCopyRegion>& regions) override final;

		/**
		* Transition the layout of a texture
		* @param[in] pTexture		Texture to transition
		* @param[in] transition	Texture layout transition info
		*/
		void TransitionTextureLayout(RHI::PipelineStage srcStage, RHI::PipelineStage dstStage, RHI::Texture* pTexture, const RHI::TextureLayoutTransition& transition) override final;

		/**
		 * Submit the command buffer to its queue
		 * @return	True of the command list was submitted successfully, false otherwise
		 */
		b8 Submit() override final;
		/**
		 * Submit the command buffer to its queue
		 * @param[in] waitSemaphores		Semaphores to wait on before execution
		 * @param[in] signalSemaphores	Semaphores to signal on completion
		 * @return	True of the command list was submitted successfully, false otherwise
		 */
		b8 Submit(const std::vector<RHI::Semaphore*>& waitSemaphores, const std::vector<RHI::PipelineStage>& waitStages, const std::vector<RHI::Semaphore*>& signalSemaphores) override final;
		/**
		 * Wait for the command buffer to finish
		 * @param[in] timeout	Timeout
		 * @return	True of the command list was submitted waited on, false otherwise
		 */
		b8 Wait(u64 timeout = u64(-1)) override final;

	private:
		friend class VulkanCommandListManager;

		/**
		 * Create a command list
		 * @param[in] pContext	RHI context
		 * @param[in] pManager	Command list manager
		 * @param[in] pQueue		Queue
		 * @return	True if the command list was created successfully, false otherwise
		 */
		b8 Create(RHI::RHIContext* pContext, RHI::CommandListManager* pManager, RHI::Queue* pQueue) override final;
		/**
		 * Destroy a command list
		 * @return	True if the command list was destroyed successfully, false otherwise
		 */
		b8 Destroy() override final;

		/**
		 * Update the barriers
		 */
		void UpdateBarriers();

		VkCommandBuffer m_CommandBuffer;	/**< Vulkan command buffer */

		RHI::PipelineStage m_SrcStage;
		RHI::PipelineStage m_DstStage;
		std::vector<VkMemoryBarrier> m_GlobalBarriers;
		std::vector<VkBufferMemoryBarrier> m_BufferBarriers;
		std::vector<VkImageMemoryBarrier> m_ImageBarriers;
	};

}
