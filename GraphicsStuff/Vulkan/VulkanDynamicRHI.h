#pragma once
#include "../RHI/IDynamicRHI.h"

namespace Vulkan {

	class VulkanContext;
	
	/**
	 * Vulkan Dynamic RHI
	 */
	class VulkanDynamicRHI final : public RHI::IDynamicRHI
	{
	public:
		/**
		 * Create a vulkan dynamic RHI
		 */
		VulkanDynamicRHI();
		~VulkanDynamicRHI();

		/**
		 * Initialize the dynamic RHI
		 * @return	True if the dynamic RHI was initialized successfully, false otherwise
		 */
		b8 Init(const RHI::RHIDesc& desc, sf::Window* pMainWindow) override final;
		/**
		 * Shutdown the dynamic RHI
		 * @return	True if the dynamic RHI was shut down successfully, false otherwise
		 */
		b8 Destroy() override final;

		////////////////////////////////////////////////////////////////////////////////
		// Renderview																  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Create a renderview
		 * @param[in] pWindow	Window to create a renderview for
		 * @param[in] vsync		V-sync mode
		 * @return				Pointer to the renderview, nullptr if the creation failed
		 */
		RHI::SwapChain* CreateSwapChain(sf::Window* pWindow, RHI::VSyncMode vsync) override final;
		/**
		 * Destroy a renderview
		 * @param[in] pRenderView	Renderview to destroy
		 * @return					True if the buffer was destroyed successfully, false otherwise
		 */
		b8 DestroySwapChain(RHI::SwapChain* pRenderView) override final;

		////////////////////////////////////////////////////////////////////////////////
		// Shaders																	  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Create a shader
		 * @param[in] desc	ShaderDescription
		 * @return			Pointer to the shader, nullptr if the creation failed
		 */
		RHI::Shader* CreateShader(const RHI::ShaderDesc& desc) override final;
		/**
		 * Destroy a shader
		 * @param[in] pShader	Shader to destroy
		 * @return				True if the shader was created successfully, false otherwise
		 */
		b8 DestroyShader(RHI::Shader* pShader) override final;

		////////////////////////////////////////////////////////////////////////////////
		// Render pass																  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Create a render pass
		 * @param[in] attachments	Render pass attachments
		 * @param[in] subpasses		Sub render passes
		 * @return					Pointer to the render pass, nullptr if the creation failed
		 */
		RHI::RenderPass* CreateRenderPass(const std::vector<RHI::RenderPassAttachment>& attachments, const std::vector<RHI::SubRenderPass>& subpasses) override final;
		/**
		 * Destroy a render pass
		 * @param[in] pRenderPass	Render pass to destroy
		 * @return					True if the render pass was destroyed successfully, false otherwise
		 */
		b8 DestroyRenderPass(RHI::RenderPass* pRenderPass) override final;

		////////////////////////////////////////////////////////////////////////////////
		// Pipelines																  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Create a graphics pipeline
		 * @param[in] desc	Graphics pipeline desc
		 * @return			Pointer to the pipeline, nullptr if the creation failed
		 */
		RHI::Pipeline* CreatePipeline(const RHI::GraphicsPipelineDesc& desc) override final;
		/**
		 * Create a copute pipeline
		 * @param[in] desc	Compute pipeline desc
		 * @return			Pointer to the pipeline, nullptr if the creation failed
		 */
		RHI::Pipeline* CreatePipeline(const RHI::ComputePipelineDesc& desc) override final;
		/**
		 * Destroy a copute pipeline
		 * @param[in] pPipeline	Pipeline to destroy
		 * @return				True if the pipeline was destroyed successfully, false otherwise
		 */
		b8 DestroyPipeline(RHI::Pipeline* pPipeline) override final;

		////////////////////////////////////////////////////////////////////////////////
		// Framebuffers																  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Create a framebuffer
		 * @param[in] renderTargets		Render targets
		 * @return						Pointer to the framebuffer, nullptr if the creation failed
		 */
		RHI::Framebuffer* CreateFramebuffer(const std::vector<RHI::RenderTarget*>& renderTargets, RHI::RenderPass* pRenderPass) override final;
		/**
		 * Destroy a framebuffer
		 * @param[in] pFramebuffer	Framebuffer to destroy
		 * @return					True if the framebuffer was destroyed successfully, false otherwise
		 */
		b8 DestroyFramebuffer(RHI::Framebuffer* pFramebuffer) override final;

		////////////////////////////////////////////////////////////////////////////////
		// Samplers																	  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Create a sampler
		 * @param[in] desc	Sampler description
		 * @return			Pointer to the sampler, nullptr if the creation failed
		 */
		RHI::Sampler* CreateSampler(const RHI::SamplerDesc& desc) override final;
		/**
		 * Destroy a sampler
		 * @param[in] pSampler	Sampler to destroy
		 * @return				True if the sampler was destroyed successfully, false otherwise
		 */
		b8 DestroySampler(RHI::Sampler* pSampler) override final;

		////////////////////////////////////////////////////////////////////////////////
		// Textures																	  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Create a texture
		 * @param[in] desc			Texture description
		 * @param[in] pCommandList	Command list used for layour transition, if nullptr, default command list will be used
		 * @return					Pointer to the texture, nullptr if the creation failed
		 */
		RHI::Texture* CreateTexture(const RHI::TextureDesc& desc, RHI::CommandList* pCommandList = nullptr) override final;
		/**
		 * Destroy a texture
		 * @param[in] pTexture	Texture to destroy
		 * @return				True if the texture was destroyed successfully, false otherwise
		 */
		b8 DestroyTexture(RHI::Texture* pTexture) override final;

		////////////////////////////////////////////////////////////////////////////////
		// Render targets															  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Create a render target
		 * @param[in] desc	Render target description
		 * @return			Pointer to the render target, nullptr if the creation failed
		 */
		RHI::RenderTarget* CreateRenderTarget(const RHI::RenderTargetDesc& desc) override final;
		/**
		 * Destroy a render target
		 * @param[in] pRenderTarget		Render target to destroy
		 * @return						True if the render target was destroyed successfully, false otherwise
		 */
		b8 DestroyRenderTarget(RHI::RenderTarget* pRenderTarget) override final;

		////////////////////////////////////////////////////////////////////////////////
		// Buffers																	  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Create a GPU buffer
		 * @param[in] type			Buffer type
		 * @param[in] size			Buffer size
		 * @param[in] flags			Buffer flags
		 * @return					Pointer to the buffer, nullptr if the creation failed
		 */
		RHI::Buffer* CreateBuffer(RHI::BufferType type, u64 size, RHI::BufferFlags flags) override final;
		/**
		 * Create a GPU buffer
		 * @param[in] vertexCount	Number of vertices
		 * @param[in] vertexSize	Size of a vertex
		 * @param[in] flags			Buffer flags
		 * @return					Pointer to the buffer, nullptr if the creation failed
		 */
		RHI::Buffer* CreateVertexBuffer(u32 vertexCount, u16 vertexSize, RHI::BufferFlags flags) override final;
		/**
		 * Create a GPU buffer
		 * @param[in] indexCount	Number of indices
		 * @param[in] indexType		Index type
		 * @param[in] flags			Buffer flags
		 * @return					Pointer to the buffer, nullptr if the creation failed
		 */
		RHI::Buffer* CreateIndexBuffer(u32 indexCount, RHI::IndexType indexType, RHI::BufferFlags flags) override final;
		/**
		 * Destroy a GPU buffer
		 * @param[in] pBuffer	Buffer to destroy
		 * @return				True if the buffer was destroyed successfully, false otherwise
		 */
		b8 DestroyBuffer(RHI::Buffer* pBuffer) override final;

		////////////////////////////////////////////////////////////////////////////////
		// Other																	  //
		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Wait for the dynamic RHI to be idle
		 * @return	True if the dynamic RHI is idle
		 */
		b8 WaitIdle() override final;

	private:

	};
}

