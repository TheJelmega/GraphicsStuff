// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// Semaphore.h: Semaphore
#pragma once
#include "../General/TypesAndMacros.h"

namespace RHI {
	class RHIContext;
	
	/**
	 * Semaphore (GPU-GPU fence)
	 */
	class Semaphore
	{
	public:
		Semaphore();
		virtual ~Semaphore();

		/**
		 * Create the semaphore
		 * @param[in] pContext	RHI context
		 * @return				True if the semaphore was created successfully, false otherwise
		 */
		virtual b8 Create(RHIContext* pContext) = 0;
		/**
		 * Destroy the semaphore
		 * @return	True if the semaphore was destroyed successfully, false otherwise
		 */
		virtual b8 Destroy() = 0;

	protected:
		RHIContext* m_pContext;
	};

}
