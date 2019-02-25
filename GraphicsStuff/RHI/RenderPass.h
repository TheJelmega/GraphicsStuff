// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// RenderPass.h: render pass
#pragma once
#include <vector>
#include "../General/TypesAndMacros.h"
#include "PixelFormat.h"
#include "RHICommon.h"

namespace RHI {
	class RHIContext;
	class RenderTarget;

	struct RenderPassAttachment
	{
		PixelFormat format		= PixelFormat();
		SampleCount samples		= SampleCount::Sample1;
		RenderTargetType type	= RenderTargetType::None;
		LoadOp loadOp			= LoadOp::DontCare;
		StoreOp storeOp			= StoreOp::DontCare;
		LoadOp stencilLoadOp	= LoadOp::DontCare;
		StoreOp stencilStoreOp	= StoreOp::DontCare;
	};

	struct RenderPassAttachmentRef
	{
		u32 index;				/**< Reference to attachment */
		RenderTargetType type;	/**< As what render target type the render target will be used in the subpass */
	};

	struct SubRenderPass
	{
		std::vector<RenderPassAttachmentRef> attachments;
	};

	class RenderPass
	{
	public:
		RenderPass();
		virtual ~RenderPass();

		/**
		 * Create a render pass
		 * @param[in] pContext		RHI context
		 * @param[in] attachments	Attachments
		 * @param[in] subpasses		Sub passes
		 * @return					True if the renderpass was created successfully, false otherwise
		 */
		virtual b8 Create(RHIContext* pContext, const std::vector<RenderPassAttachment>& attachments, const std::vector<SubRenderPass>& subpasses) = 0;

		/**
		 * Destroy a renderpass
		 * @return	True if the renderpass was desstroyed successfully, false otherwise
		 */
		virtual b8 Destroy() = 0;

	protected:
		RHIContext* m_pContext;					/**< RHI context */
		std::vector<RenderPassAttachment> m_Attachments;	/**< Render pass attachments */
		std::vector<SubRenderPass> m_SubPasses;			/**< Subpasses */
	};

}
