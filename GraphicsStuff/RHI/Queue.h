// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// Queue.h: GPU queue
#pragma once
#include "../General/TypesAndMacros.h"
#include "RHICommon.h"

namespace RHI {
	class RHIContext;

	class Queue
	{
	public:
		Queue();
		virtual ~Queue();

		/**
		 * Initialize the queue
		 * @param[in] pContext	RHI context
		 * @param[in] type		Queue type
		 * @param[in] index		Queue index
		 * @param[in] priority	Queue priority
		 * @return				True if the queue was initialize successfully, false otherwise
		 */
		virtual b8 Init(RHIContext* pContext, QueueType type, u32 index, QueuePriority priority) = 0;

		/**
		 * Wait for the queue to be idle
		 * @return	True if queue is idle, false otherwise
		 */
		virtual b8 WaitIdle() = 0;


		/**
		 * Get the queue type
		 * @return	Queue type
		 */
		QueueType GetQueueType() const { return m_Type; }

	protected:
		RHIContext* m_pContext;
		QueueType m_Type;
		QueuePriority m_Priority;
	};

}
