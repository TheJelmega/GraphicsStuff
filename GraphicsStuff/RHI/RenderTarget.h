// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// RenderTarget.h: Render target
#pragma once
#include "../General/TypesAndMacros.h"
#include "ClearValue.h"
#include "PixelFormat.h"
#include "RHICommon.h"
#include "Texture.h"

namespace RHI {
	class RHIContext;

	struct RenderTargetDesc
	{
		u32 width				= 1;						/**< Eidth */
		u32 height				= 1;						/**< Height */
		PixelFormat format		= PixelFormat();			/**< Format */
		SampleCount samples		= SampleCount::Sample1;		/**< Sample count */
		RenderTargetType type	= RenderTargetType::None;	/**< Type */
	};

	class RenderTarget
	{
	public:
		RenderTarget();
		virtual ~RenderTarget();

		/**
		 * Create a render target
		 * @param[in] pContext	RHI context
		 * @param[in] desc		Render target description
		 * @return				True if the texture was created successfully, false otherwise
		 */
		virtual b8 Create(RHIContext* pContext, const RenderTargetDesc& desc) = 0;

		/**
		 * Destroy the texture
		 */
		virtual b8 Destroy() = 0;


		/**
		 * Get the underlying texture
		 * @return	Underlying texture
		 */
		Texture* GetTexture() { return m_pTexture; }

		/**
		 * Get the render target width
		 * @return	Render target with
		 */
		u32 GetWidth() const { return m_pTexture->GetWidth(); }
		/**
		 * Get the render target height
		 * @return	Render target height
		 */
		u32 GetHeight() const { return m_pTexture->GetHeight(); }
		/**
		 * Get the render target type
		 * @return	Render target type
		 */
		RenderTargetType GetType() const { return m_Type; }

		/**
		* Get the render target clear value
		* @return	Render target clear value
		*/
		ClearValue GetClearValue() const { return m_ClearValue; }
		/**
		* Set the render target clear value
		* @param[in] clearValue		Render target clear value
		*/
		void SetClearColor(ClearValue clearValue) { m_ClearValue = clearValue; }

	protected:
		RHIContext* m_pContext;	/**< RHI context */
		Texture* m_pTexture;			/**< Render target texture */
		RenderTargetType m_Type;		/**< Flags */
		ClearValue m_ClearValue;		/**< Clear value */
	};

}
