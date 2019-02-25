// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// InputDescriptor.h: Input descriptor
#pragma once
#include <vector>
#include "../General/TypesAndMacros.h"

namespace RHI {
	
	enum class InputSemantic : u8
	{
		None,			/**< No semantic */
		Position,		/**< Position */
		Color,			/**< Color */
		Normal,			/**< Normal */
		TexCoord,		/**< Texture coordinate */
		BlendIndices,	/**< Blend indices */
		BlendWeight,	/**< Blend weight */
		Tangent,		/**< Tangent */
		Binormal,		/**< Bi-normal */
		TPosition,		/**< Transformed position */
		PSize			/**< Point size */
	};

	enum class InputElementType : u8
	{
		None,		/**< No type (can't be used to create vertex buffer) */
		Float1,		/**< Float */
		Float2,		/**< 2D float vector */
		Float3,		/**< 3D float vector */
		Float4,		/**< 4D float vector */
		Int1,		/**< Int */
		Int2,		/**< 2D int vector */
		Int3,		/**< 3D int vector */
		Int4,		/**< 4D int vector */
		UInt1,		/**< UInt */
		UInt2,		/**< 2D uint vector */
		UInt3,		/**< 3D uint vector */
		UInt4,		/**< 4D uint vector */
		Short1,		/**< Short */
		Short2,		/**< 2D short vector */
		Short3,		/**< 3D short vector */
		Short4,		/**< 4D short vector */
		UShort1,	/**< UShort */
		UShort2,	/**< 2D ushort vector */
		UShort3,	/**< 3D ushort vector */
		Ushort4,	/**< 4D ushort vector */
		Count,		/**< Amount of values in the enum */
	};

#define HV_GFX_DEF_ELEM_ALIGN u16(-1)

	struct InputElementDesc
	{
		/**
		 * Create an empty input element description
		 */
		InputElementDesc();
		/**
		 * Create a input element desription from values
		 * @param[in] semantic			Semantic
		 * @param[in] semanticIndex		Semantic index
		 * @param[in] type				Element type
		 * @param[in] offset			Element offset in bytes, use HV_GFX_DEF_ELEM_ALIGN
		 * @param[in] inputSlot			Input slot (used for multiple vertex buffers, the amount of slots is dependent on the API used)
		 * @param[in] instanceStepRate	Instance step rate (number of instances to draw using the same per-instance data before advancing the buffer by 1 element)
		 */
		InputElementDesc(InputSemantic semantic, u16 semanticIndex, InputElementType type, u16 offset, u8 inputSlot, u16 instanceStepRate);

		InputSemantic semantic;		/**< Semantic */
		InputElementType type;		/**< Element type */
		u16 semanticIndex;			/**< Semantic index */
		u16 offset;					/**< Element offset in bytes */
		u8 inputSlot;				/**< Input slot (used for multiple vertex buffers, the amount of slots is dependent on the API used) */
		u16 instanceStepRate;		/**< Instance step rate (number of instances to draw using the same per-instance data before advancing the buffer by 1 element) */

		/**
		* Get the size of the described vertex element
		* @return	Vertex element size
		*/
		u16 GetSize() const;

		b8 operator==(const InputElementDesc& desc) const;
		b8 operator!=(const InputElementDesc& desc) const;
	};

	class InputDescriptor
	{
	public:
		/**
		 * Create an empty vertex input description
		 */
		InputDescriptor();
		/**
		 * Create a vertex input description from an array of vertex element descriptions
		 * @param[in] descs	Array of vertex element descriptions
		 * @param[in] count	Amount of vertex element descriptions
		 */
		InputDescriptor(InputElementDesc* descs, u16 count);
		/**
		 * Create a vertex input description from an array of vertex element descriptions
		 * @param[in] descs	Dynamic array of vertex element descriptions
		 */
		InputDescriptor(std::vector<InputElementDesc> descs);


		/**
		 * Append a vertex element description to the vertex input description
		 * @param[in] desc	Input element description
		 */
		void Append(const InputElementDesc& desc);

		/**
		 * Get an element by index
		 * @param[in] index	Index of the element
		 * @return			Pointer to an element, nullptr if no element was found
		 */
		InputElementDesc* GetElementByIndex(u16 index);
		/**
		 * Get an element by index
		 * @param[in] index	Index of the element
		 * @return			Pointer to an element, nullptr if no element was found
		 */
		const InputElementDesc* GetElementByIndex(u16 index) const;
		/**
		 * Get an element by index
		 * @param[in] semantic	Semantic of the element
		 * @param[in] index		Semantic index of the element
		 * @return				Pointer to an element, nullptr if no element was found
		 */
		InputElementDesc* GetElementBySemantic(InputSemantic semantic, u16 index);
		/**
		 * Get an element by index
		 * @param[in] semantic	Semantic of the element
		 * @param[in] index		Semantic index of the element
		 * @return				Pointer to an element, nullptr if no element was found
		 */
		const InputElementDesc* GetElementBySemantic(InputSemantic semantic, u16 index) const;
		/**
		 * Get all elements in an input slot
		 * @param[in] inputSlot	Input slot of the elements
		 * @return				Dynamic array with pointers to the elements in an input slot
		 */
		std::vector<InputElementDesc*> GetElementsByInputSlot(u16 inputSlot);
		/**
		 * Get all elements in an input slot
		 * @param[in] inputSlot	Input slot of the elements
		 * @return				Dynamic array with pointers to the elements in an input slot
		 */
		std::vector<const InputElementDesc*> GetElementsByInputSlot(u16 inputSlot) const;

		/**
		 * Get the size of a vertex at a specific input slot
		 * @param[in] inputSlot	Input slot of vertex
		 * @return				Size of vertex in a slot
		 */
		u16 GetInputSize(u16 inputSlot) const;

		/**
		 * Check if this input layout is compatible with another input layout (e.g. vertex buffer vs shader input)
		 * @param[in] desc	Description to check compatibility with
		 * @return			True if the descriptions are compatible, false otherwise
		 */
		b8 IsCompatible(const InputDescriptor& desc) const;
		/**
		 * Check if the input descriptor has a certain sementic
		 * @param[in] semantic	Semantic to check
		 * @return				True if the input descriptor has a semantic
		 */
		b8 HasSemantic(InputSemantic semantic);

		/**
		 * Get the used input slots
		 * @return	Used input slots
		 */
		const std::vector<u16>& GetInputSlots() const { return m_InputSlots; }

	private:
		std::vector<InputElementDesc> m_Descs;
		std::vector<u16> m_InputSlots;
	};

}