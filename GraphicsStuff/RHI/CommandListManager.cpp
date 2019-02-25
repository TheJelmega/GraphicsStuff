
#include "CommandListManager.h"

namespace RHI {


	CommandListManager::CommandListManager()
		: m_pContext(nullptr)
		, m_pActiveCommandList(nullptr)
	{
	}

	CommandListManager::~CommandListManager()
	{
	}
}
