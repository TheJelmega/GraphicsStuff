// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// CommandListManager.h: Command list manager
#pragma once
#include <vector>
#include "../General/TypesAndMacros.h"

namespace RHI {

	class CommandList;
	class Queue;
	class RHIContext;
	
	class CommandListManager
	{
	public:
		CommandListManager();
		virtual ~CommandListManager();

		/**
		 * Create a command list manager
		 * @param[in] pContext	RHI context
		 * @return				True if the command list manager was created successfully, false otherwise
		 */
		virtual b8 Create(RHIContext* pContext) = 0;
		/**
		 * Destroy the command list manager
		 * @return				True if the command list manager was created successfully, false otherwise
		 */
		virtual b8 Destroy() = 0;

		/**
		 * Create a command list for a certain queue
		 * @param[in] pQueue	Queue
		 * @return				Pointer to a command buffer, nullptr if creation failed
		 */
		virtual CommandList* CreateCommandList(Queue* pQueue) = 0;
		/**
		 * Destroy a command list
		 * @param[in] pCommandList	Command list to destroy
		 * @return					True if the command list was successfully destroyed, false otherwise
		 */
		virtual b8 DestroyCommandList(CommandList* pCommandList) = 0;

		/**
		 * Create and begin a single time command list for a certain queue
		 * @param[in] pQueue	Queue
		 * @return				Pointer to a single time command buffer, nullptr if creation failed
		 */
		virtual CommandList* CreateSingleTimeCommandList(Queue* pQueue) = 0;
		/**
		 * End and estroy a singe time command list
		 * @param[in] pCommandList	Single time command list
		 * @return					True if the single time command list was successfully destroyed, false otherwise
		 */
		virtual b8 EndSingleTimeCommandList(CommandList* pCommandList) = 0;

	protected:
		RHIContext* m_pContext;				/**< RHI Context */
		std::vector<CommandList*> m_CommandLists;		/**< Command lists */
		std::vector<CommandList*> m_STCommandLists;	/**< Single time command lists */
		CommandList* m_pActiveCommandList;			/**< Active command list */
	};

}
