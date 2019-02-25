
#include "Queue.h"

namespace RHI {

	Queue::Queue()
		: m_pContext(nullptr)
		, m_Type(QueueType::Unknown)
		, m_Priority(QueuePriority::High)
	{
	}

	Queue::~Queue()
	{
	}
}
