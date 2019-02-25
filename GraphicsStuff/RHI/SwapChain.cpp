
#include "SwapChain.h"

namespace RHI {


	SwapChain::SwapChain()
		: m_pContext(nullptr)
		, m_pWindow(nullptr)
		, m_VSync(VSyncMode::Off)
		, m_pPresentQueue(nullptr)
		, m_RenderTargetIndex(0)
		, m_SemaphoreIndex(0)
	{
	}

	SwapChain::~SwapChain()
	{
	}

	RenderTarget* SwapChain::GetRenderTarget(u32 index)
	{
		if (index >= m_RenderTargets.size())
			return nullptr;

		return m_RenderTargets[index];
	}
}
