// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// DynamicRHI.h: Dynamic RHI (implemented by API specific RHIs)
#pragma once
#include <vector>
#include "RHICommon.h"
#include "Shader.h"
#include "RenderPass.h"
#include "Pipeline.h"
#include "DescriptorSet.h"
#include "RHIContext.h"

namespace sf {
	class Window;
}

namespace RHI {
	struct TextureDesc;
	struct SamplerDesc;
	struct RenderTargetDesc;
	class SwapChain;

	/**
	 * RHI description
	 */
	struct RHIDesc
	{
		::RHI::RHIValidationLevel validationLevel;
		std::vector<QueueInfo> queueInfo;
	};
	
	/**
	 * Dynamic RHI interface
	 */
	class IDynamicRHI
	{
	public:
		/**
		 * Create the dynamic RHI interface
		 */
		IDynamicRHI();
		virtual ~IDynamicRHI();

		/**
		 * Initialize the dynamic RHI
		 * @return	True if the dynamic RHI was initialized successfully, false otherwise
		 */
		virtual b8 Init(const RHIDesc& desc, sf::Window* pMainWindow) = 0;
		/**
		 * Destroy the dynamic RHI
		 * @return	True if the dynamic RHI was shut down successfully, false otherwise
		 */
		virtual b8 Destroy() = 0;

		////////////////////////////////////////////////////////////////////////////////
		// Renderview																  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Create a renderview
		 * @param[in] pWindow	Window to create a renderview for
		 * @param[in] vsync		V-sync mode
		 * @return				Pointer to the renderview, nullptr if the creation failed
		 */
		virtual SwapChain* CreateSwapChain(sf::Window* pWindow, VSyncMode vsync) = 0;
		/**
		 * Destroy a renderview
		 * @param[in] pRenderView	Renderview to destroy
		 * @return					True if the renderview was destroyed successfully, false otherwise
		 */
		virtual b8 DestroySwapChain(SwapChain* pRenderView) = 0;

		////////////////////////////////////////////////////////////////////////////////
		// Shaders																	  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Create a shader
		 * @param[in] desc	Shader description
		 * @return			Pointer to the shader, nullptr if the creation failed
		 */
		virtual Shader* CreateShader(const ShaderDesc& desc) = 0;
		/**
		 * Destroy a shader
		 * @param[in] pShader	Shader to destroy
		 * @return				True if the shader was destroyed successfully, false otherwise
		 */
		virtual b8 DestroyShader(Shader* pShader) = 0;

		////////////////////////////////////////////////////////////////////////////////
		// Render pass																  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Create a render pass
		 * @param[in] attachments	Render pass attachments
		 * @param[in] subpasses		Sub render passes
		 * @return					Pointer to the render pass, nullptr if the creation failed
		 */
		virtual RenderPass* CreateRenderPass(const std::vector<RenderPassAttachment>& attachments, const std::vector<SubRenderPass>& subpasses) = 0;
		/**
		 * Destroy a render pass
		 * @param[in] pRenderPass	Render pass to destroy
		 * @return					True if the render pass was destroyed successfully, false otherwise
		 */
		virtual b8 DestroyRenderPass(RenderPass* pRenderPass) = 0;

		////////////////////////////////////////////////////////////////////////////////
		// Pipelines																  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Create a graphics pipeline
		 * @param[in] desc	Graphics pipeline desc
		 * @return			Pointer to the pipeline, nullptr if the creation failed
		 */
		virtual Pipeline* CreatePipeline(const GraphicsPipelineDesc& desc) = 0;
		/**
		 * Create a copute pipeline
		 * @param[in] desc	Compute pipeline desc
		 * @return			Pointer to the pipeline, nullptr if the creation failed
		 */
		virtual Pipeline* CreatePipeline(const ComputePipelineDesc& desc) = 0;
		/**
		 * Destroy a copute pipeline
		 * @param[in] pPipeline	Pipeline to destroy
		 * @return				True if the pipeline was destroyed successfully, false otherwise
		 */
		virtual b8 DestroyPipeline(Pipeline* pPipeline) = 0;

		////////////////////////////////////////////////////////////////////////////////
		// Framebuffers																  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Create a framebuffer
		 * @param[in] renderTargets		Render targets
		 * @param[in] pRenderPass		Associated render pass
		 * @return						Pointer to the framebuffer, nullptr if the creation failed
		 */
		virtual Framebuffer* CreateFramebuffer(const std::vector<RenderTarget*>& renderTargets, RenderPass* pRenderPass) = 0;
		/**
		 * Destroy a framebuffer
		 * @param[in] pFramebuffer	Framebuffer to destroy
		 * @return					True if the framebuffer was destroyed successfully, false otherwise
		 */
		virtual b8 DestroyFramebuffer(Framebuffer* pFramebuffer) = 0;

		////////////////////////////////////////////////////////////////////////////////
		// Command lists															  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Get the command list manager
		 * @return	Pointer to the command list manager, nullptr if the creation failed
		 */
		CommandListManager* GetCommandListManager() { return m_pContext->GetCommandListManager(); }

		////////////////////////////////////////////////////////////////////////////////
		// Samplers																	  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		* Create a sampler
		* @param[in] desc	Sampler description
		* @return			Pointer to the sampler, nullptr if the creation failed
		*/
		virtual Sampler* CreateSampler(const SamplerDesc& desc) = 0;
		/**
		* Destroy a sampler
		* @param[in] pSampler	Sampler to destroy
		* @return				True if the sampler was destroyed successfully, false otherwise
		*/
		virtual b8 DestroySampler(Sampler* pSampler) = 0;

		////////////////////////////////////////////////////////////////////////////////
		// Textures																	  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Create a texture
		 * @param[in] desc			Texture description
		 * @param[in] pCommandList	Command list used for layout transition, if nullptr, default command list will be used
		 * @return					Pointer to the texture, nullptr if the creation failed
		 */
		virtual Texture* CreateTexture(const TextureDesc& desc, CommandList* pCommandList = nullptr) = 0;
		/**
		 * Destroy a texture
		 * @param[in] pTexture	Texture to destroy
		 * @return				True if the texture was destroyed successfully, false otherwise
		 */
		virtual b8 DestroyTexture(Texture* pTexture) = 0;

		////////////////////////////////////////////////////////////////////////////////
		// Render targets															  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Create a render target
		 * @param[in] desc	Render target description
		 * @return			Pointer to the render target, nullptr if the creation failed
		 */
		virtual RenderTarget* CreateRenderTarget(const RenderTargetDesc& desc) = 0;
		/**
		 * Destroy a render target
		 * @param[in] pRenderTarget		Render target to destroy
		 * @return						True if the render target was destroyed successfully, false otherwise
		 */
		virtual b8 DestroyRenderTarget(RenderTarget* pRenderTarget) = 0;

		////////////////////////////////////////////////////////////////////////////////
		// Buffers																	  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Create a buffer
		 * @param[in] type			Buffer type
		 * @param[in] size			Buffer size
		 * @param[in] flags			Buffer flags
		 * @return					Pointer to the buffer, nullptr if the creation failed
		 */
		virtual Buffer* CreateBuffer(BufferType type, u64 size, BufferFlags flags) = 0;
		/**
		 * Create a buffer
		 * @param[in] vertexCount	Number of vertices
		 * @param[in] vertexSize	Size of a vertex
		 * @param[in] flags			Buffer flags
		 * @return					Pointer to the buffer, nullptr if the creation failed
		 */
		virtual Buffer* CreateVertexBuffer(u32 vertexCount, u16 vertexSize, BufferFlags flags) = 0;
		/**
		 * Create an index buffer
		 * @param[in] indexCount	Number of indices
		 * @param[in] indexType		Index type
		 * @param[in] flags			Buffer flags
		 * @return					Pointer to the buffer, nullptr if the creation failed
		 */
		virtual Buffer* CreateIndexBuffer(u32 indexCount, IndexType indexType, BufferFlags flags) = 0;

		/**
		 * Destroy a GPU buffer
		 * @param[in] pBuffer	Buffer to destroy
		 * @return				True if the buffer was destroyed successfully, false otherwise
		 */
		virtual b8 DestroyBuffer(Buffer* pBuffer) = 0;

		////////////////////////////////////////////////////////////////////////////////
		// Desriptor sets															  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		* Get the command list manager
		* @return	Pointer to the command list manager, nullptr if the creation failed
		*/
		DescriptorSetManager* GetDescriptorSetManager() { return m_pContext->GetDescriptorSetManager(); }


		////////////////////////////////////////////////////////////////////////////////
		// Other																	  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Wait for the dynamic RHI to be idle
		 * @return	True if the dynamic RHI is idle
		 */
		virtual b8 WaitIdle() = 0;

		/**
		 * Get the RHI context
		 * @return RHI context
		 */
		RHIContext* GetContext() { return m_pContext; };

	protected:

		RHIDesc m_Desc;			/**< RHI desc */
		RHIContext* m_pContext;	/**< RHI context */

	};

}
