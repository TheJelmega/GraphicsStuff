
#include "InputDescriptor.h"

namespace RHI {
	
	InputElementDesc::InputElementDesc()
		: semantic(InputSemantic::None)
		, type(InputElementType::None)
		, semanticIndex(0)
		, offset(0)
		, inputSlot(0)
		, instanceStepRate(0)
	{
	}

	InputElementDesc::InputElementDesc(InputSemantic semantic, u16 semanticIndex, InputElementType type,
		u16 offset, u8 inputSlot, u16 instanceStepRate)
		: semantic(semantic)
		, semanticIndex(semanticIndex)
		, type(type)
		, offset(offset)
		, inputSlot(inputSlot)
		, instanceStepRate(instanceStepRate)
	{
	}

	u16 InputElementDesc::GetSize() const
	{
		switch (type)
		{
		case InputElementType::Float1:	return sizeof(f32);
		case InputElementType::Float2:	return sizeof(f32) * 2;
		case InputElementType::Float3:	return sizeof(f32) * 3;
		case InputElementType::Float4:	return sizeof(f32) * 4;
		case InputElementType::Int1:	return sizeof(i32);
		case InputElementType::Int2:	return sizeof(i32) * 2;
		case InputElementType::Int3:	return sizeof(i32) * 3;
		case InputElementType::Int4:	return sizeof(i32) * 4;
		case InputElementType::UInt1:	return sizeof(u32);
		case InputElementType::UInt2:	return sizeof(u32) * 2;
		case InputElementType::UInt3:	return sizeof(u32) * 3;
		case InputElementType::UInt4:	return sizeof(u32) * 4;
		case InputElementType::Short1:	return sizeof(i16);
		case InputElementType::Short2:	return sizeof(i16) * 2;
		case InputElementType::Short3:	return sizeof(i16) * 3;
		case InputElementType::Short4:	return sizeof(i16) * 4;
		case InputElementType::UShort1:	return sizeof(u16);
		case InputElementType::UShort2:	return sizeof(u16) * 2;
		case InputElementType::UShort3:	return sizeof(u16) * 3;
		case InputElementType::Ushort4:	return sizeof(u16) * 4;
		case InputElementType::None:
		default:
			return 0;
		}
	}

	b8 InputElementDesc::operator==(const InputElementDesc& desc) const
	{
		return semantic == desc.semantic && semanticIndex == desc.semanticIndex && type == desc.type &&
			offset == desc.offset && inputSlot == desc.inputSlot && desc.instanceStepRate == instanceStepRate;
	}
	b8 InputElementDesc::operator!=(const InputElementDesc& desc) const
	{
		return !(*this == desc);
	}

	InputDescriptor::InputDescriptor()
	{
	}

	InputDescriptor::InputDescriptor(InputElementDesc* descs, u16 count)
	{
		for (u16 i = 0; i < count; ++i)
		{
			Append(descs[i]);
		}
	}

	InputDescriptor::InputDescriptor(std::vector<InputElementDesc> descs)
	{
		for (const auto& desc : descs)
		{
			Append(desc);
		}
	}

	void InputDescriptor::Append(const InputElementDesc& desc)
	{
		if (desc.offset == HV_GFX_DEF_ELEM_ALIGN)
		{
			InputElementDesc elem = desc;
			elem.offset = GetInputSize(elem.inputSlot);

			if (std::find(m_InputSlots.begin(), m_InputSlots.end(), elem.inputSlot) == m_InputSlots.end())
				m_InputSlots.push_back(elem.inputSlot);

			m_Descs.push_back(elem);
		}
		else
		{
			if (std::find(m_InputSlots.begin(), m_InputSlots.end(), desc.inputSlot) == m_InputSlots.end())
				m_InputSlots.push_back(desc.inputSlot);

			m_Descs.push_back(desc);
		}
	}

	InputElementDesc* InputDescriptor::GetElementByIndex(u16 index)
	{
		if (index >= m_Descs.size())
			return nullptr;
		return &m_Descs[index];
	}

	const InputElementDesc* InputDescriptor::GetElementByIndex(u16 index) const
	{
		if (index >= m_Descs.size())
			return nullptr;
		return &m_Descs[index];
	}

	InputElementDesc* InputDescriptor::GetElementBySemantic(InputSemantic semantic, u16 index)
	{
		for (u16 i = 0, count = u16(m_Descs.size()); i < count; ++i)
		{
			InputElementDesc& desc = m_Descs[i];
			if (desc.semantic == semantic && desc.semanticIndex == index)
				return &desc;
		}
		return nullptr;
	}

	const InputElementDesc* InputDescriptor::GetElementBySemantic(InputSemantic semantic, u16 index) const
	{
		for (u16 i = 0, count = u16(m_Descs.size()); i < count; ++i)
		{
			const InputElementDesc& desc = m_Descs[i];
			if (desc.semantic == semantic && desc.semanticIndex == index)
				return &desc;
		}
		return nullptr;
	}

	std::vector<InputElementDesc*> InputDescriptor::GetElementsByInputSlot(u16 inputSlot)
	{
		std::vector<InputElementDesc*> descs;
		for (u16 i = 0, count = u16(m_Descs.size()); i < count; ++i)
		{
			InputElementDesc& desc = m_Descs[i];
			if (desc.inputSlot == inputSlot)
				descs.push_back(&desc);
		}
		return descs;
	}

	std::vector<const InputElementDesc*> InputDescriptor::GetElementsByInputSlot(u16 inputSlot) const
	{
		std::vector<const InputElementDesc*> descs;
		for (u16 i = 0, count = u16(m_Descs.size()); i < count; ++i)
		{
			const InputElementDesc& desc = m_Descs[i];
			if (desc.inputSlot == inputSlot)
				descs.push_back(&desc);
		}
		return descs;
	}

	u16 InputDescriptor::GetInputSize(u16 inputSlot) const
	{
		u16 size = 0;
		for (u16 i = 0, count = u16(m_Descs.size()); i < count; ++i)
		{
			const InputElementDesc& desc = m_Descs[i];
			if (desc.inputSlot == inputSlot)
				size += desc.GetSize();
		}
		return size;
	}

	b8 InputDescriptor::IsCompatible(const InputDescriptor& desc) const
	{
		for (const auto& elem : m_Descs)
		{
			b8 found = false;
			for (const auto& other : desc.m_Descs)
			{
				if (elem == other)
				{
					found = true;
					break;
				}
			}

			if (!found)
				return false;
		}
		return true;
	}

	b8 InputDescriptor::HasSemantic(InputSemantic semantic)
	{
		for (const auto& elem : m_Descs)
		{
			if (elem.semantic == semantic)
				return true;
		}
		return false;
	}

}
