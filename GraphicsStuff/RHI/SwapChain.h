// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// RenderView.h: Render view (swapchain)
#pragma once
#include <vector>
#include "../General/TypesAndMacros.h"
#include "RHICommon.h"

namespace sf {
	class Window;
}

namespace RHI {
	class RHIContext;
	class Semaphore;
	class RenderTarget;
	class Queue;

	class SwapChain
	{
	public:

		SwapChain();
		virtual ~SwapChain();

		/**
		 * Create a render view
		 * @param[in] pContext	RHI context
		 * @param[in] pWindow	Window
		 * @param[in] vsync		VSync mode
		 * @param[in] pQueue	Present queue
		 * @return				True if the render view was created successfully, false otherwise
		 */
		virtual b8 Init(RHI::RHIContext* pContext, sf::Window* pWindow, VSyncMode vsync, Queue* pQueue) = 0;

		/**
		* Destroy a render view
		* @return	True if the render view was destroyed successfully, false otherwise
		*/
		virtual b8 Destroy() = 0;

		/**
		 * Present the backbuffer to the screen
		 * @return	True if the backbuffer was presented successfully, false otherwise
		 */
		virtual b8 Present() = 0;

		/**
		 * Get the render view's v-sync mode
		 * @return	V-sync mode
		 */
		VSyncMode GetVSyncMode() const { return m_VSync; }

		/**
		 * Get the current render target
		 * @return	Current render target
		 */
		RenderTarget* GetCurrentRenderTarget() { return m_RenderTargets[m_RenderTargetIndex]; }
		/**
		 * Get the rendertarget to the backbuffer at an index
		 * @param[in] index		Render target index
		 * @return				Render target at the index, nullptr if the index is invalid
		 */
		RenderTarget* GetRenderTarget(u32 index);
		/**
		 * Get the index of the current index of the backbuffer
		 * @return	Index of the current index of the backbuffer
		 */
		u32 GetCurrentIndex() { return m_RenderTargetIndex; }
		/**
		 * Get the current wait semaphore
		 * @return	Current wait semaphore
		 */
		Semaphore* GetWaitSemaphore() { return m_WaitSemaphores[m_SemaphoreIndex]; }
		/**
		 * Get the current wait semaphore
		 * @return	Current wait semaphore
		 */
		Semaphore* GetSignalSemaphore() { return m_SignalSemaphores[m_SemaphoreIndex]; }
		/**
		 * Get the present queue
		 * @return	Present queue
		 */
		Queue* GetPresentQueue() { return m_pPresentQueue; }


	protected:
		RHIContext* m_pContext;				/**< RHI context */
		sf::Window* m_pWindow;							/**< Associated window */
		VSyncMode m_VSync;							/**< VSync mode */
		Queue* m_pPresentQueue;						/**< Present queue */
		std::vector<RenderTarget*> m_RenderTargets;	/**< Render targets */
		std::vector<Semaphore*> m_WaitSemaphores;		/**< Wait semaphores (can present to screen) */
		std::vector<Semaphore*> m_SignalSemaphores;	/**< Signal semaphores (can use backbuffer) */
		u32 m_RenderTargetIndex;					/**< Render target index */
		u32 m_SemaphoreIndex;						/**< Render target index */
	};
	
}