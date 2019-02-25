// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// Fence.h: Fence
#pragma once
#include "RHICommon.h"

namespace RHI {
	class RHIContext;
	
	/**
	 * Fence (GPU-CPU)
	 */
	class Fence
	{
	public:
		Fence();
		virtual ~Fence();

		/**
		 * Create the fence
		 * @param[in] pContext	RHI context
		 * @param[in] signaled	If the fence is set to signalled on creation
		 * @return				True if the semaphore was fence successfully, false otherwise
		 */
		virtual b8 Create(RHIContext* pContext, b8 signaled) = 0;
		/**
		 * Destroy the fence
		 * @return	True if the fence was destroyed successfully, false otherwise
		 */
		virtual b8 Destroy() = 0;
		/**
		 * Update the fence state
		 */
		virtual void Tick() = 0;
		/**
		 * Wait for the fence
		 * @param[in] timeout	Timeout
		 * @return	True if the fence was waited for successfully
		 */
		virtual b8 Wait(u64 timeout = u64(-1)) = 0;
		/**
		 * Reset the fence state
		 * @return	True if the fence was reset successfully, false otherwise
		 */
		virtual b8 Reset() = 0;
		/**
		 * Update and get the current fence status
		 * @return	Fence status
		 */
		FenceStatus GetStatus() { Tick(); return m_Status; }
		/**
		* Check if the fence is signalled
		* @return	True if the fence is signaled, false otherwise
		*/
		b8 IsSignaled() { Tick(); return m_Status == FenceStatus::Signaled; }

		/**
		 * Set the fence state to submitted
		 */
		void SetSubmitted() { m_Status = FenceStatus::Submitted; }


	protected:
		RHIContext* m_pContext;
		FenceStatus m_Status;
		u64 m_FenceValue;
	};

}
