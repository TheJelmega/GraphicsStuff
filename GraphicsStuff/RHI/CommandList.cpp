
#include "CommandList.h"
#include "Fence.h"

namespace RHI {


	CommandList::CommandList()
		: m_pContext(nullptr)
		, m_pManager(nullptr)
		, m_pQueue(nullptr)
		, m_Status(CommandListState::Reset)
		, m_pFence(nullptr)
		, m_pPipeline(nullptr)
		, m_pRenderPass(nullptr)
		, m_pFramebuffer(nullptr)
	{
	}

	CommandList::~CommandList()
	{
	}

	CommandListState CommandList::GetState()
	{
		if (m_Status == CommandListState::Submited)
		{
			if (m_pFence->GetStatus() == FenceStatus::Signaled)
				m_Status = CommandListState::Finished;
		}
		return m_Status;
	}
}
