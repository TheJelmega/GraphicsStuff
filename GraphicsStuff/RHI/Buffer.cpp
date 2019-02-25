
#include "Buffer.h"

namespace RHI {


	Buffer::Buffer()
		: m_Type(BufferType::Default)
		, m_Size(0)
		, m_pContext(nullptr)
		, m_VertexCount(0)		/**< Handles m_IndexCount */
		, m_VertexSize(0)		/**< Handles m_IndexType */
		, m_MemorySize(0)
		
	{
	}

	Buffer::~Buffer()
	{
	}
}
