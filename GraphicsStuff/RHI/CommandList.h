#pragma once
#include <vector>

#include "../General/TypesAndMacros.h"
#include "RHICommon.h"

namespace RHI {
	class RHIContext;

	class Texture;
	class Buffer;
	class Framebuffer;
	class Pipeline;
	class RenderPass;

	class CommandListManager;
	class Queue;

	class Fence;
	class Semaphore;

	class DescriptorSet;

	struct Viewport;
	struct ScissorRect;

	class CommandList
	{
	public:
		CommandList();
		virtual ~CommandList();

		/**
		 * Begin the command buffer
		 * @return	True if the command list was begin successfully, false otherwise
		 */
		virtual b8 Begin() = 0;
		/**
		 * End the command buffer
		 * @return	True if the command list was ended successfully, false otherwise
		 */
		virtual b8 End() = 0;

		/**
		 * Bind a pipeline
		 * @param[in] pPipeline	Pipeline to bind
		 */
		virtual void BindPipeline(Pipeline* pPipeline) = 0;

		/**
		 * Begin a render pass
		 * @param[in] pRenderPass	Render pass to begin
		 * @param[in] pFramebuffer	Framebuffer
		 */
		virtual void BeginRenderPass(RenderPass* pRenderPass, Framebuffer* pFramebuffer) = 0;
		/**
		 * End the current render pass
		 */
		virtual void EndRenderPass() = 0;

		/**
		 * Bind a vertex buffer
		 * @param[in] inputSlot		Input slot of the buffer
		 * @param[in] pBuffer		Vertex buffer to bind
		 * @param[in] offset		Offset in buffer
		 */
		virtual void BindVertexBuffer(u16 inputSlot, Buffer* pBuffer, u64 offset) = 0;
		/**
		 * Bind multiple vertex buffers
		 * @param[in] inputSlot		Input slot of the first buffer
		 * @param[in] buffers		Vertex buffers to bind
		 * @param[in] offsets		Offsets in buffers
		 */
		virtual void BindVertexBuffer(u16 inputSlot, const std::vector<Buffer*>& buffers, const std::vector<u64>& offsets) = 0;
		/**
		 * Bind a index buffer
		 * @param[in] pBuffer		Vertex buffer to bind
		 * @param[in] offset		Offset in buffer
		 */
		virtual void BindIndexBuffer(Buffer* pBuffer, u64 offset) = 0;
		/**
		 * Bind a index buffer
		 * @param[in] pBuffer		Vertex buffer to bind
		 * @param[in] offset		Offset in buffer
		 * @param[in] type			Index type
		 * @note					Use this function when using a combined buffer (vertices, indices, etc. in same buffer)
		 */
		virtual void BindIndexBuffer(Buffer* pBuffer, u64 offset, IndexType type) = 0;
		/**
		 * Bind a descriptor set
		 * @param[in] firstSet	Index of first descriptor set
		 * @param[in] pSet		Descriptor set
		 * @note				Use this function when using a combined buffer (vertices, indices, etc. in same buffer)
		 */
		virtual void BindDescriptorSets(u32 firstSet, DescriptorSet* pSet) = 0;
		/**
		 * Bind a descriptor set
		 * @param[in] firstSet			Index of first descriptor set
		 * @param[in] pSet				Descriptor set
		 * @param[in] dynamicOffset		Dynamic offset (for dynamic descriptor set type)
		 * @note						Use this function when using a combined buffer (vertices, indices, etc. in same buffer)
		 */
		virtual void BindDescriptorSets(u32 firstSet, DescriptorSet* pSet, u32 dynamicOffset) = 0;
		/**
		 * Bind descriptor sets
		 * @param[in] firstSet	Index of first descriptor set
		 * @param[in] sets		Descriptor sets
		 * @note				Use this function when using a combined buffer (vertices, indices, etc. in same buffer)
		 */
		virtual void BindDescriptorSets(u32 firstSet, const std::vector<DescriptorSet*>& sets) = 0;
		/**
		 * Bind descriptor sets
		 * @param[in] firstSet			Index of first descriptor set
		 * @param[in] sets				Descriptor sets
		 * @param[in] dynamicOffsets	Dynamic offsets (for dynamic descriptor set types)
		 * @note						Use this function when using a combined buffer (vertices, indices, etc. in same buffer)
		 */
		virtual void BindDescriptorSets(u32 firstSet, const std::vector<DescriptorSet*>& sets, const std::vector<u32>& dynamicOffsets) = 0;

		/**
		 * Set the viewport
		 * @param[in] viewport	Viewport
		 */
		virtual void SetViewport(Viewport& viewport) = 0;
		/**
		 * Set the scissor rect
		 * @param[in] scissor	Scissor rect
		 */
		virtual void SetScissor(ScissorRect& scissor) = 0;

		/**
		 * Draw a number of vertices and instances
		 * @param[in] vertexCount		Amount of vertices to draw
		 * @param[in] instanceCount		Amount of instances to draw
		 * @param[in] firstVertex		Index of vertex to start drawing from
		 * @param[in] firstInstance		Index of instance to start drawing from
		 */
		virtual void Draw(u32 vertexCount, u32 instanceCount = 1, u32 firstVertex = 0, u32 firstInstance = 0) = 0;
		/**
		 * Draw a number of vertices and instances (indexed)
		 * @param[in] indexCount		Amount of indices to draw
		 * @param[in] instanceCount		Amount of instances to draw
		 * @param[in] firstIndex		Index of index to start drawing from
		 * @param[in] vertexOffset		Offset in vertex buffer (index n == vertex at offset + n)
		 * @param[in] firstInstance		Index of instance to start drawing from
		 */
		virtual void DrawIndexed(u32 indexCount, u32 instanceCount = 1, u32 firstIndex = 0, u32 vertexOffset = 0, u32 firstInstance = 0) = 0;

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
		virtual void CopyBuffer(Buffer* pSrcBuffer, u64 srcOffset, Buffer* pDstBuffer, u64 dstOffset, u64 size) = 0;
		/**
		 * Copy data from one buffer to another
		 * @param[in] pSrcBuffer	Buffer to copy from
		 * @param[in] pDstBuffer	Buffer to copy to
		 * @param[in] regions		Buffer copy regions
		 * @note					While it's possible to use this function, it is preferable to use the buffer copy function, since this functions doesn't do additional checks on the buffers
		 *							(make sure you know what you are doing!)
		 */
		virtual void CopyBuffer(Buffer* pSrcBuffer, Buffer* pDstBuffer, const std::vector<BufferCopyRegion>& regions) = 0;
		/**
		 * Copy data from one buffer to another
		 *@param[in] pSrcTex		Texture to copy from
		 * @param[in] pDstTex		Texture to copy to
		 * @param[in] region		Texture copy region
		 * @note					While it's possible to use this function, it is preferable to use the texture copy function, since this functions doesn't do additional checks on the buffers
		 *							(make sure you know what you are doing!)
		 */
		virtual void CopyTexture(Texture* pSrcTex, Texture* pDstTex, const TextureCopyRegion& region) = 0;
		/**
		 * Copy data from one buffer to another
		 * @param[in] pSrcTex		Texture to copy from
		 * @param[in] pDstTex		Texture to copy to
		 * @param[in] regions		Texture copy regions
		 * @note					While it's possible to use this function, it is preferable to use the texture copy function, since this functions doesn't do additional checks on the buffers
		 *							(make sure you know what you are doing!)
		 */
		virtual void CopyTexture(Texture* pSrcTex, Texture* pDstTex, const std::vector<TextureCopyRegion>& regions) = 0;
		/**
		 * Copy data from one buffer to another
		 * @param[in] pBuffer		Buffer to copy from
		 * @param[in] pTexture		Texture to copy to
		 * @param[in] region		Copy region
		 * @note					While it's possible to use this function, it is preferable to use the texure copy function, since this functions doesn't do additional checks on the buffers
		 *							(make sure you know what you are doing!)
		 */
		virtual void CopyBufferToTexture(Buffer* pBuffer, Texture* pTexture, const TextureBufferCopyRegion& region) = 0;
		/**
		 * Copy data from one buffer to another
		 * @param[in] pBuffer		Buffer to copy from
		 * @param[in] pTexture		Texture to copy to
		 * @param[in] regions		Copy regions
		 * @note					While it's possible to use this function, it is preferable to use the texure copy function, since this functions doesn't do additional checks on the buffers
		 *							(make sure you know what you are doing!)
		 */
		virtual void CopyBufferToTexture(Buffer* pBuffer, Texture* pTexture, const std::vector<TextureBufferCopyRegion>& regions) = 0;
		/**
		 * Copy data from one buffer to another
		 * @param[in] pTexture		Texture to copy from
		 * @param[in] pBuffer		Buffer to copy to
		 * @param[in] region		Copy region
		 * @note					While it's possible to use this function, it is preferable to use the texure copy function, since this functions doesn't do additional checks on the buffers
		 *							(make sure you know what you are doing!)
		 */
		virtual void CopyTextureToBuffer(Texture* pTexture, Buffer* pBuffer, const TextureBufferCopyRegion& region) = 0;
		/**
		 * Copy data from one buffer to another
		 * @param[in] pTexture		Texture to copy from
		 * @param[in] pBuffer		Buffer to copy to
		 * @param[in] regions		Copy regions
		 * @note					While it's possible to use this function, it is preferable to use the texure copy function, since this functions doesn't do additional checks on the buffers
		 *							(make sure you know what you are doing!)
		 */
		virtual void CopyTextureToBuffer(Texture* pTexture, Buffer* pBuffer, const std::vector<TextureBufferCopyRegion>& regions) = 0;

		/**
		* Transition the layout of a texture
		* @param[in] srcStage		Source stage
		* @param[in] dstStage		Destination stage
		* @param[in] pTexture		Texture to transition
		* @param[in] transition		Texture layout transition info
		* @note						Command buffer automatically batches transitions
		*/
		virtual void TransitionTextureLayout(PipelineStage srcStage, PipelineStage dstStage, Texture* pTexture, const TextureLayoutTransition& transition) = 0;

		/**
		 * Submit the command buffer to its queue
		 * @return	True of the command list was submitted successfully, false otherwise
		 */
		virtual b8 Submit() = 0;
		/**
		 * Submit the command buffer to its queue
		 * @param[in] waitSemaphores		Semaphores to wait on before execution
		 * @param[in] signalSemaphores	Semaphores to signal on completion
		 * @return	True of the command list was submitted successfully, false otherwise
		 */
		virtual b8 Submit(const std::vector<Semaphore*>& waitSemaphores, const std::vector<PipelineStage>& waitStages, const std::vector<Semaphore*>& signalSemaphores) = 0;
		/**
		 * Wait for the command buffer to finish
		 * @param[in] timeout	Timeout
		 * @return	True of the command list was submitted waited on, false otherwise
		 */
		virtual b8 Wait(u64 timeout = u64(-1)) = 0;

		/**
		 * Get the command list status
		 * @return	Command list status
		 */
		CommandListState GetState();

		/**
		 * Get the command list's fence
		 * @return Command list's fence
		 */
		Fence* GetFence() { return m_pFence; }

	protected:

		/**
		 * Create a command list
		 * @param[in] pContext	RHI context
		 * @param[in] pManager	Command list manager
		 * @param[in] pQueue	Queue
		 * @return	True if the command list was created successfully, false otherwise
		 */
		virtual b8 Create(RHIContext* pContext, CommandListManager* pManager, Queue* pQueue) = 0;
		/**
		 * Destroy a command list
		 * @return	True if the command list was destroyed successfully, false otherwise
		 */
		virtual b8 Destroy() = 0;

		RHIContext* m_pContext;				/**< RHI Context */
		CommandListManager* m_pManager;		/**< Command list manager */
		Queue* m_pQueue;					/**< Queue */

		CommandListState m_Status;			/**< Status */
		Fence* m_pFence;					/**< Fence */

		Pipeline* m_pPipeline;				/**< Current pipeline */
		RenderPass* m_pRenderPass;			/**< Current render pass */
		Framebuffer* m_pFramebuffer;		/**< Current framebuffer */

		// Checks
		std::vector<u16> m_BoundInputSlots;	/**< Bound vertex input slots */
	};

	
}
