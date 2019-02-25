// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// Framebuffer.h: framebuffer
#pragma once
#include <vector>
#include "RHICommon.h"

namespace RHI {
	class RHIContext;

	class RenderTarget;
	class RenderPass;

	class Framebuffer
	{
	public:
		Framebuffer();
		virtual ~Framebuffer();

		/**
		 * Create the frame buffer
		 * @param[in] pContext			RHI context
		 * @param[in] renderTargets		Render targets
		 * @param[in] pRenderPass		Associated render pass
		 * @return						True if the framebuffer was created successfully, false otherwise
		 */
		virtual b8 Create(RHIContext* pContext, const std::vector<RenderTarget*>& renderTargets, RenderPass* pRenderPass) = 0;

		/**
		 * Destroy the frame buffer
		 * @return	True if the framebuffer was destroyed successfully, false otherwise
		 */
		virtual b8 Destroy() = 0;

		/**
		 * Get the render targets
		 * @return	Render targets
		 */
		const std::vector<RenderTarget*>& GetRenderTargets() const { return m_RenderTargets; }
		/**
		* Get the framebuffer width
		* @return	Framebuffer width
		*/
		u32 GetWidth() const { return m_Width; }
		/**
		* Get the framebuffer width
		* @return	Framebuffer width
		*/
		u32 GetHeight() const { return m_Height; }

	protected:
		RHIContext* m_pContext;				/**< RHI context */
		std::vector<RenderTarget*> m_RenderTargets;	/**< Render targets */
		RenderPass* m_pRenderPass;					/**< Associated render pass */

		u32 m_Width;								/**< Framebuffer height */
		u32 m_Height;								/**< Framebuffer width */
	};

}