
#include "RHIContext.h"
#include "Queue.h"

namespace RHI {

	RHIContext::RHIContext()
		: m_pCommandListManager(nullptr)
		, m_pDescriptorSetManager(nullptr)
	{
	}

	RHIContext::~RHIContext()
	{
	}

	Queue* RHIContext::GetQueue(RHI::QueueType type)
	{
		Queue* pOutQueue = nullptr;
		for (Queue* pQueue : m_Queues)
		{
			QueueType queueType = pQueue->GetQueueType();
			if (pOutQueue)
			{
				if ((queueType & type) != QueueType::Unknown)
					pOutQueue = pQueue;
				if ((queueType & ~type) == RHI::QueueType::Unknown)
					return pOutQueue;
			}
			else
			{
				if ((queueType & ~type) == RHI::QueueType::Unknown)
					return pQueue;
			}

		}
		return pOutQueue;
	}
}
