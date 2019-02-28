// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// RHIContext.h: RHI Context
#pragma once
#include <vector>
#include "../General/TypesAndMacros.h"
#include "RHICommon.h"

struct GLFWwindow;

namespace RHI {
	struct RHIDesc;
	class Queue;
	class CommandListManager;
	class DescriptorSetManager;

	/**
	 * RHI Context, contains data for the creation and use of RHI Objects
	 */
	class RHIContext
	{
	public:
		RHIContext();
		virtual ~RHIContext();

		/**
		 * Initialize the RHI context
		 * @return	True if the context was initialized successfully initialized, false otherwise
		 */
		virtual b8 Init(const RHIDesc& desc, GLFWwindow* pMainWindow) = 0;
		/**
		 * Destroy the RHI context
		 * @return	True if the context was destroyed successfully initialized, false otherwise
		 */
		virtual b8 Destroy() = 0;

		/**
		 * Get a queue from a certain type
		 * @param[in] type	Queue type
		 * @return			Best fitting queue for its type
		 */
		Queue* GetQueue(QueueType type);
		/**
		 * Get the command list manager
		 * @return	Command list manager
		 */
		CommandListManager* GetCommandListManager() { return m_pCommandListManager; }
		/**
		 * Get the descriptor set manager
		 * @return	Descriptor set manager
		 */
		DescriptorSetManager* GetDescriptorSetManager() { return m_pDescriptorSetManager; }

	protected:
		CommandListManager* m_pCommandListManager;		/**< Command list manager */
		DescriptorSetManager* m_pDescriptorSetManager;	/**< Descriptor set manager */
		std::vector<Queue*> m_Queues;						/**< Device queues */

	};

}
