#pragma once
#include <vector>

#include "RHICommon.h"
#include "RHIContext.h"
#include "PixelFormat.h"

namespace RHI {
	
	class Texture;
	class CommandList;

	class Buffer
	{
	public:
		Buffer();
		virtual ~Buffer();

		/**
		 * Create a buffer
		 * @param[in] pContext		RHI context
		 * @param[in] type			Buffer type
		 * @param[in] size			Buffer size
		 * @param[in] flags			Buffer flags
		 * @param[in] format		Buffer data format (texel buffers only)
		 * @return					True if the buffer was created successfully, false otherwise
		 */
		virtual b8 Create(RHIContext* pContext, BufferType type, u64 size, BufferFlags flags, PixelFormat format = PixelFormat()) = 0;
		/**
		 * Create a vertex buffer
		 * @param[in] pContext		RHI context
		 * @param[in] vertexCount	Number of vertices
		 * @param[in] vertexSize	Size of a vertex
		 * @param[in] flags			Buffer flags
		 * @return					True if the buffer was created successfully, false otherwise
		 */
		virtual b8 CreateVertexBuffer(RHIContext* pContext, u32 vertexCount, u16 vertexSize, BufferFlags flags) = 0;
		/**
		 * Create a vertex buffer
		 * @param[in] pContext		RHI context
		 * @param[in] indexCount	Number of indices
		 * @param[in] indexType		Index type
		 * @param[in] flags			Buffer flags
		 * @return					True if the buffer was created successfully, false otherwise
		 */
		virtual b8 CreateIndexBuffer(RHIContext* pContext, u32 indexCount, IndexType indexType, BufferFlags flags) = 0;

		/**
		 * Detroy a buffer
		 * @return					True if the buffer was destroyed successfully, false otherwise
		 */
		virtual b8 Destroy() = 0;

		/**
		 * Write data to the buffer
		 * @param[in] offset		Offset in buffer to write to
		 * @param[in] size			Size of data in bytes
		 * @param[in] pData			Data to write
		 * @param[in] flags			Buffer write flags
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 * @return
		 */
		virtual b8 Write(u64 offset, u64 size, void* pData, BufferWriteFlags flags = BufferWriteFlags::None, CommandList* pCommandList = nullptr) = 0;
		/**
		 * Read data from the buffer
		 * @param[in] offset		Offset in buffer to read from
		 * @param[in] size			Size of data in bytes
		 * @param[inout] pData		Array to read to
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 * @return
		 */
		virtual b8 Read(u64 offset, u64 size, void* pData, CommandList* pCommandList = nullptr) = 0;
		/**
		 * Copy a buffer to the buffer
		 * @param[in] pBuffer		Buffer to copy from
		 * @param[in] srcOffset		Offset in source buffer
		 * @param[in] dstOffset		Offset in this buffer
		 * @param[in] size			Size of data in bytes
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 * @return
		 */
		virtual b8 Copy(Buffer* pBuffer, u64 srcOffset, u64 dstOffset, u64 size, CommandList* pCommandList = nullptr) = 0;
		/**
		 * Copy a buffer to the buffer
		 * @param[in] pBuffer		Buffer to copy from
		 * @param[in] regions		Buffer copy regions
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 * @return
		 */
		virtual b8 Copy(Buffer* pBuffer, const std::vector<BufferCopyRegion>& regions, CommandList* pCommandList = nullptr) = 0;
		/**
		 * Copy a texture to the buffer
		 * @param[in] pTexture		Texture to copy from
		 * @param[in] region		Copy region
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 * @return
		 */
		virtual b8 Copy(Texture* pTexture, const TextureBufferCopyRegion& region, CommandList* pCommandList = nullptr) = 0;
		/**
		 * Copy a texture to the buffer
		 * @param[in] pTexture		Texture to copy from
		 * @param[in] regions		Copy regions
		 * @param[in] pCommandList	Command list used to write data, if nullptr, data will be copied immediatly using a default command buffer
		 * @return
		 */
		virtual b8 Copy(Texture* pTexture, const std::vector<TextureBufferCopyRegion>& regions, CommandList* pCommandList = nullptr) = 0;

		/**
		 * Get the buffer type
		 * @return Buffer type
		 */
		BufferType GetType() const { return m_Type; }
		/**
		* Get the buffer size
		* @return Buffer size
		*/
		uint64_t GetSize() const { return m_Size; }
		/**
		 * Get the amount of vertices in the buffer
		 * @return	Amount of vertices in the buffer
		 * @note	If the buffer is not a vertex buffer, 0 is returned
		 */
		u32 GetVertexCount() const { return m_Type == BufferType::Vertex ? m_VertexCount : 0; }
		/**
		 * Get the size of a vertex in the buffer
		 * @return	Size of a vertex in the buffer
		 * @note	If the buffer is not a vertex buffer, 0 is returned
		 */
		u32 GetVertexSize() const { return m_Type == BufferType::Vertex ? m_VertexSize : 0; }
		/**
		 * Get the amount of indices in the buffer
		 * @return	Amount of indices in the buffer
		 * @note	If the buffer is not an index buffer, 0 is returned
		 */
		u32 GetIndexCount() const { return m_Type == BufferType::Index ? m_IndexCount : 0; }
		/**
		 * Get the type of an index in the buffer
		 * @return	Type of an index in the buffer
		 * @note	If the buffer is not an index buffer, IndexType::Unknown is returned
		 */
		IndexType GetIndexType() const { return m_Type == BufferType::Index ? m_IndexType : IndexType::Unknown; }
		/**
		* Get the type of an index in the buffer
		* @return	Type of an index in the buffer
		* @note	If the buffer is not an index buffer, IndexType::Unknown is returned
		*/
		PixelFormat GetTexelBufferFormat() const { return m_Type == BufferType::UniformTexel || m_Type == BufferType::StorageTexel ? m_TexelBufferFormat : PixelFormat(); }

		/**
		* Get the size of the texture on the GPU
		* @return	Size on the GPU
		*/
		u64 GetMemorySize() const { return m_MemorySize; }

	protected:

		BufferType m_Type;
		u64 m_Size;
		RHIContext* m_pContext;
		BufferFlags m_Flags;
		u64 m_MemorySize;				/**< Size of the buffer on the GPU */

		// Data for vertex and index buffers
#pragma warning(push)
#pragma warning(disable : 4201) // nonstandard extension used: nameless struct/union
		union
		{
			struct
			{
				u32 m_VertexCount;		/**< Vertex count (vertex buffer) */
				u32 m_VertexSize;		/**< Vertex size (vertex buffer) */
			};
			struct
			{
				u32 m_IndexCount;		/**< Index count (index buffer) */
				IndexType m_IndexType;	/**< Index type (index buffer) */
			};
			PixelFormat m_TexelBufferFormat;
		};
#pragma warning(pop)

	};

}
