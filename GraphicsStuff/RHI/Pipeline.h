// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// Pipeline.h: Pipeline
#pragma once
#include "InputDescriptor.h"
#include "RHICommon.h"
#include "MultisampleDesc.h"
#include "BlendStateDesc.h"
#include "DepthStencilDesc.h"
#include "CommandList.h"
#include "RasterizerDesc.h"
#include "TessellationDesc.h"
#include "Viewport.h"
#include "ScissorRect.h"

namespace RHI {
	class DescriptorSetLayout;
	class RHIContext;

	class Shader;
	class RenderPass;
	
	struct GraphicsPipelineDesc
	{
		Shader* pVertexShader;									/**< Vertex shader */
		Shader* pGeometryShader;								/**< Geometry shader */
		Shader* pFragmentShader;								/**< Fragment shader */

		InputDescriptor inputDescriptor;						/**< Vertex input descriptor */

		PrimitiveTopology primitiveTopology;					/**< Primitive topology */
		b8 enablePrimitiveRestart;								/**< Enable primitive restart for strips */

		RasterizerDesc rasterizer;								/**< Rasterizer description */
		MultisampleDesc	multisample;							/**< Multisample description */
		BlendStateDesc blendState;								/**< Blend state description */
		DepthStencilDesc depthStencil;							/**< Depth stencil test */
		TessellationDesc tesellation;							/**< Tessellation descriptor */
		DynamicState dynamicState;								/**< Dynamic state */

		// TODO: Multi viewport support?
		Viewport viewport;										/**< Viewport (ignored when viewport is set as dynamic) */
		ScissorRect scissor;									/**< Scissor rect (ignored when scissor is set as dynamic) */

		RenderPass* pRenderPass;								/**< Render pass */
		std::vector<DescriptorSetLayout*> descriptorSetLayouts;	/**< Descriptor sets */
	};

	struct ComputePipelineDesc
	{
		Shader* pComputeShader;
	};

	class Pipeline
	{
	public:
		Pipeline();
		virtual ~Pipeline();

		/**
		 * Create a graphics pipeline
		 * @param[in] pContext	RHI context
		 * @param[in] desc		Graphics pipeline descriptor
		 * @return				True if the graphics pipeline was created successfully, false otherwise
		 */
		virtual b8 Create(RHIContext* pContext, const GraphicsPipelineDesc& desc) = 0;
		/**
		 * Create a compute pipeline
		 * @param[in] pContext	RHI context
		 * @param[in] desc		Compute pipeline descriptor
		 * @return				True if the compute pipeline was created successfully, false otherwise
		 */
		virtual b8 Create(RHIContext* pContext, const ComputePipelineDesc& desc) = 0;

		/**
		 * Destroy the pipeline
		 * @return	True if the pipeline was destroyed successfully, false otherwise
		 */
		virtual b8 Destroy() = 0;

		/**
		 * Get the pipeline type
		 * @return	Pipeline type
		 */
		PipelineType GetType() const { return m_Type; }

		/**
		 * Get the graphics pipeline description
		 * @return	Graphics pipeline description
		 */
		const GraphicsPipelineDesc& GetGraphicsDesc() const { return m_GraphicsDesc; }
		/**
		* Get the compute pipeline description
		* @return	Compute pipeline description
		*/
		const ComputePipelineDesc& GetComputeDesc() const { return m_ComputeDesc; }

	protected:
		RHIContext* m_pContext;	/**< RHI context */
		PipelineType m_Type;			/**< Pipeline type */

		union
		{
			GraphicsPipelineDesc m_GraphicsDesc;
			ComputePipelineDesc m_ComputeDesc;
		};
	};

}
