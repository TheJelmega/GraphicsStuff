
#include <algorithm>
#include "VulkanHelpers.h"
#include "../RHI/GpuInfo.h"
#undef max
#undef min

namespace Vulkan { namespace Helpers {

	////////////////////////////////////////////////////////////////////////////////
	// Debug/Error/etc info														  //
	////////////////////////////////////////////////////////////////////////////////
	const char* GetResultString(VkResult result)
	{
		// Descriptions: https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkResult.html
		if (result >= 0)
			return Tables::g_VulkanResult[result];

		if (result >= VK_ERROR_FRAGMENTED_POOL)
			return Tables::g_VulkanErrors[-result];

		switch (result)
		{
		case VK_ERROR_OUT_OF_POOL_MEMORY:			return "VK_ERROR_OUT_OF_POOL_MEMORY";
		case VK_ERROR_INVALID_EXTERNAL_HANDLE:		return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
		case VK_ERROR_SURFACE_LOST_KHR:				return "VK_ERROR_SURFACE_LOST_KHR";
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:		return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
		case VK_SUBOPTIMAL_KHR:						return "VK_SUBOPTIMAL_KHR";
		case VK_ERROR_OUT_OF_DATE_KHR:				return "VK_ERROR_OUT_OF_DATE_KHR";
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:		return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
		case VK_ERROR_VALIDATION_FAILED_EXT:		return "VK_ERROR_VALIDATION_FAILED_EXT";
		case VK_ERROR_INVALID_SHADER_NV:			return "VK_ERROR_INVALID_SHADER_NV";
		case VK_ERROR_FRAGMENTATION_EXT:			return "VK_ERROR_FRAGMENTATION_EXT";
		case VK_ERROR_NOT_PERMITTED_EXT:			return "VK_ERROR_NOT_PERMITTED_EXT";
		default:									return "UNKOWN";
		}
	}

	const char* GetObjectTypeString(VkDebugReportObjectTypeEXT type)
	{
		if (type <= VK_DEBUG_REPORT_OBJECT_TYPE_VALIDATION_CACHE_EXT_EXT)
			return Tables::g_debugObjectTypeName[type];

		switch (type)
		{
		default:
		case VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION_EXT:		return "[Sampler YCBCR conversion]";
		case VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_EXT:	return "[Descriptor update template]";
		}
	}

	/*LogLevel GetDebugReportLogLevel(VkDebugReportFlagsEXT flags)
	{
		switch (flags)
		{
		case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
			return LogLevel::Detail;
		case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
		case VK_DEBUG_REPORT_WARNING_BIT_EXT:
			return LogLevel::Warning;
		case VK_DEBUG_REPORT_ERROR_BIT_EXT:
			return LogLevel::Error;
		case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
		default:
			return LogLevel::Info;
		}
	}*/

	RHI::GpuVendor GetVendor(u32 vendorId)
	{
		switch (vendorId)
		{
		case 0x1002:
			return RHI::GpuVendor::AMD;
		case 0x10DE:
			return RHI::GpuVendor::Nvidia;
		case 0x13B5:
			return RHI::GpuVendor::ARM;
		case 0x5143:
			return RHI::GpuVendor::Qualcomm;
		case 0x8086:
			return RHI::GpuVendor::Intel;
		default:
			return RHI::GpuVendor::Unknown;
		}
	}

	RHI::GpuType GetGpuType(VkPhysicalDeviceType type)
	{
		assert(type < VK_PHYSICAL_DEVICE_TYPE_CPU + 1);
		return Tables::g_GpuTypes[type];
	}

	/*RHI::ApiVersion GetApiVersion(u32 version)
	{
		RHI::ApiVersion apiVersion;
		apiVersion.major = VK_VERSION_MAJOR(version);
		apiVersion.minor = VK_VERSION_MINOR(version);
		return apiVersion;
	}*/

	RHI::DriverVersion GetDriverVersion(u32 version)
	{
		RHI::DriverVersion apiVersion;
		apiVersion.major = VK_VERSION_MAJOR(version);
		apiVersion.minor = VK_VERSION_MINOR(version);
		return apiVersion;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Surface and swapchain													  //
	////////////////////////////////////////////////////////////////////////////////
	VkSurfaceFormatKHR GetOptimalSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
	{
		if (formats.size() == 0 || formats[0].format == VK_FORMAT_UNDEFINED)
		{
			return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}

		for (VkSurfaceFormatKHR format : formats)
		{
			if ( format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return format;
			}
		}

		return formats[0];
	}

	VkPresentModeKHR GetOptimalPresentMode(const std::vector<VkPresentModeKHR>& modes, RHI::VSyncMode& vsync)
	{
		// Try to find the closest mode to the corresponding vsync mode
		// If the mode isn't found, move on to the next
		switch (vsync)
		{
		case RHI::VSyncMode::Off:
		{
			for (VkPresentModeKHR mode : modes)
			{
				if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
				{
					return mode;
				}
			}
		}
		// If immediate mode isn't supported, use tripple buffering, since it is closer to immediate than normal vsync
		case RHI::VSyncMode::Tripple:
		{
			for (VkPresentModeKHR mode : modes)
			{
				if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					vsync = RHI::VSyncMode::Tripple;
					return mode;
				}
			}
		}
		default:
		case RHI::VSyncMode::VSync:
		{
			// All platform support FIFO
			vsync = RHI::VSyncMode::VSync;
			return VK_PRESENT_MODE_FIFO_KHR;
		}
		}
	}

	VkExtent2D GetSwapExtent(glm::uvec2 size, const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width == std::numeric_limits<u32>::max())
		{
			return capabilities.currentExtent;
		}

		VkExtent2D extent = { size.x, size.y };
		extent.width = std::min(std::max(extent.width, capabilities.minImageExtent.width), capabilities.maxImageExtent.width);
		extent.height = std::min(std::max(extent.height, capabilities.minImageExtent.height), capabilities.maxImageExtent.height);

		return extent;
	}

	VkAttachmentLoadOp GetLoadOp(RHI::LoadOp loadOp)
	{
		assert(u8(loadOp) < u8(RHI::LoadOp::Count));
		return Tables::g_LoadOps[u8(loadOp)];
	}

	VkAttachmentStoreOp GetStoreOp(RHI::StoreOp storeOp)
	{
		assert(u8(storeOp) < u8(RHI::StoreOp::Count));
		return Tables::g_StoreOps[u8(storeOp)];
	}

	VkImageLayout GetSubpassAttachmentLayout(RHI::RenderTargetType type)
	{
		assert(u8(type) <= u8(RHI::RenderTargetType::MultisampleResolve));
		return Tables::g_SubpassAttachmentLayouts[u8(type)];
	}

	////////////////////////////////////////////////////////////////////////////////
	// Pipeline																	  //
	////////////////////////////////////////////////////////////////////////////////
	VkPrimitiveTopology GetPrimitiveTopology(RHI::PrimitiveTopology topology)
	{
		assert(u8(topology) < u8(RHI::PrimitiveTopology::Count));
		return Tables::g_primitiveTopology[u8(topology)];
	}

	VkPolygonMode GetPolygonMode(RHI::FillMode fillMode)
	{
		assert(u8(fillMode) < u8(RHI::FillMode::Count));
		return Tables::g_PolygonMode[u8(fillMode)];
	}

	VkCullModeFlagBits GetCullMode(RHI::CullMode cullMode)
	{
		assert(u8(cullMode) < u8(RHI::CullMode::Count));
		return Tables::g_CullMode[u8(cullMode)];
	}

	VkFrontFace GetFrontFace(RHI::FrontFace frontFace)
	{
		assert(u8(frontFace) < u8(RHI::FrontFace::Count));
		return Tables::g_FrontFace[u8(frontFace)];
	}

	VkSampleCountFlagBits GetSampleCount(RHI::SampleCount samples)
	{
		assert(u8(samples) < u8(RHI::SampleCount::Count));
		return Tables::g_SampleCount[u8(samples)];
	}

	VkColorComponentFlags GetColorWriteMask(RHI::ColorComponentMask components)
	{
		assert(u8(components) <= u8(RHI::ColorComponentMask::Max));
		return Tables::g_ColorMask[u8(components)];
	}

	VkBlendOp GetBlendOp(RHI::BlendOp blendOp)
	{
		assert(u8(blendOp) < u8(RHI::BlendOp::Count));
		return Tables::g_BlendOp[u8(blendOp)];
	}

	VkBlendFactor GetBlendFactor(RHI::BlendFactor blendFactor)
	{
		assert(u8(blendFactor) < u8(RHI::BlendFactor::Count));
		return Tables::g_BlendFactor[u8(blendFactor)];
	}

	VkLogicOp GetLogicOp(RHI::LogicOp logicOp)
	{
		assert(u8(logicOp) < u8(RHI::LogicOp::Count));
		return Tables::g_LogicOp[u8(logicOp)];
	}

	VkPipelineBindPoint GetPipelineBindPoint(RHI::PipelineType type)
	{
		assert(u8(type) < u8(RHI::PipelineType::Count));
		return Tables::g_PipelineBindPoint[u8(type)];
	}

	VkPipelineStageFlags GetPipelineStage(RHI::PipelineStage pipelineStage)
	{
		VkPipelineStageFlags stage = (VkPipelineStageFlagBits)0;

		if ((pipelineStage & RHI::PipelineStage::TopOfPipe) != RHI::PipelineStage::None)
			stage |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		if ((pipelineStage & RHI::PipelineStage::DrawIndirect) != RHI::PipelineStage::None)
			stage |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
		if ((pipelineStage & RHI::PipelineStage::VertexInput) != RHI::PipelineStage::None)
			stage |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
		if ((pipelineStage & RHI::PipelineStage::VertexShader) != RHI::PipelineStage::None)
			stage |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
		if ((pipelineStage & RHI::PipelineStage::HullShader) != RHI::PipelineStage::None)
			stage |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
		if ((pipelineStage & RHI::PipelineStage::DomainShader) != RHI::PipelineStage::None)
			stage |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
		if ((pipelineStage & RHI::PipelineStage::GeometryShader) != RHI::PipelineStage::None)
			stage |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
		if ((pipelineStage & RHI::PipelineStage::FragmentShader) != RHI::PipelineStage::None)
			stage |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		if ((pipelineStage & RHI::PipelineStage::EarlyFragmentTest) != RHI::PipelineStage::None)
			stage |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		if ((pipelineStage & RHI::PipelineStage::LastFragmentTest) != RHI::PipelineStage::None)
			stage |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		if ((pipelineStage & RHI::PipelineStage::ColorAttachmentOutput) != RHI::PipelineStage::None)
			stage |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		if ((pipelineStage & RHI::PipelineStage::ComputeShader) != RHI::PipelineStage::None)
			stage |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		if ((pipelineStage & RHI::PipelineStage::Transfer) != RHI::PipelineStage::None)
			stage |= VK_PIPELINE_STAGE_TRANSFER_BIT;
		if ((pipelineStage & RHI::PipelineStage::BottomOfPipe) != RHI::PipelineStage::None)
			stage |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		if ((pipelineStage & RHI::PipelineStage::Host) != RHI::PipelineStage::None)
			stage |= VK_PIPELINE_STAGE_HOST_BIT;
		if ((pipelineStage & RHI::PipelineStage::AllGraphics) != RHI::PipelineStage::None)
			stage |= VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
		if ((pipelineStage & RHI::PipelineStage::AllCommands) != RHI::PipelineStage::None)
			stage |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		return stage;
	}

	VkFormat GetInputFormat(RHI::InputElementType type)
	{
		assert(u8(type) < u8(RHI::InputElementType::Count));
		return Tables::g_inputElementTypeToFormat[u8(type)];
	}

	void GetInputDescription(const RHI::InputDescriptor& inputDescriptor,
		std::vector<VkVertexInputBindingDescription>& bindings, std::vector<VkVertexInputAttributeDescription>& attributes)
	{
		assert(bindings.size() == 0);
		assert(attributes.size() == 0);

		const std::vector<u16>& inputSlots = inputDescriptor.GetInputSlots();
		for (u16 slot : inputSlots)
		{
			// Binding
			VkVertexInputBindingDescription binding;
			binding.binding = slot;
			// TODO
			binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			binding.stride = inputDescriptor.GetInputSize(slot);
			bindings.push_back(binding);

			// Attributes
			auto elements = inputDescriptor.GetElementsByInputSlot(slot);
			for (sizeT i = 0; i < elements.size(); ++i)
			{
				const RHI::InputElementDesc* pElement = elements[i];
				VkVertexInputAttributeDescription attrib;
				attrib.format = GetInputFormat(pElement->type);
				attrib.binding = slot;
				attrib.location = u32(i);
				attrib.offset = pElement->offset;
				attributes.push_back(attrib);
			}
		}
	}

	VkStencilOp GetStencilOp(RHI::StencilOp stencilOp)
	{
		assert(u8(stencilOp) < u8(RHI::StencilOp::Count));
		return Tables::g_stencilOps[u8(stencilOp)];
	}

	std::vector<VkDynamicState> GetDynamicStates(RHI::DynamicState dynamicState)
	{
		std::vector<VkDynamicState> states;

		if ((dynamicState & RHI::DynamicState::Viewport) != RHI::DynamicState::None)
			states.push_back(VK_DYNAMIC_STATE_VIEWPORT);
		if ((dynamicState & RHI::DynamicState::Scissor) != RHI::DynamicState::None)
			states.push_back(VK_DYNAMIC_STATE_SCISSOR);
		if ((dynamicState & RHI::DynamicState::LineWidth) != RHI::DynamicState::None)
			states.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);
		if ((dynamicState & RHI::DynamicState::DepthBias) != RHI::DynamicState::None)
			states.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
		if ((dynamicState & RHI::DynamicState::BlendConstants) != RHI::DynamicState::None)
			states.push_back(VK_DYNAMIC_STATE_BLEND_CONSTANTS);
		if ((dynamicState & RHI::DynamicState::DepthBounds) != RHI::DynamicState::None)
			states.push_back(VK_DYNAMIC_STATE_DEPTH_BOUNDS);
		if ((dynamicState & RHI::DynamicState::StencilCompareMask) != RHI::DynamicState::None)
			states.push_back(VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK);
		if ((dynamicState & RHI::DynamicState::StencilWriteMask) != RHI::DynamicState::None)
			states.push_back(VK_DYNAMIC_STATE_STENCIL_WRITE_MASK);
		if ((dynamicState & RHI::DynamicState::StencilReference) != RHI::DynamicState::None)
			states.push_back(VK_DYNAMIC_STATE_STENCIL_REFERENCE);

		return states;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Buffer																	  //
	////////////////////////////////////////////////////////////////////////////////
	VkBufferUsageFlags GetBufferUsage(RHI::BufferType type, const RHI::BufferFlags& flags)
	{
		VkBufferUsageFlags usage = 0;

		if ((flags & RHI::BufferFlags::NoRead) == RHI::BufferFlags::None)
			usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		if ((flags & RHI::BufferFlags::NoWrite) == RHI::BufferFlags::None)
			usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		if ((type & RHI::BufferType::Vertex) != RHI::BufferType::Default)
			usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		if ((type & RHI::BufferType::Index) != RHI::BufferType::Default)
			usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		if ((type & RHI::BufferType::Uniform) != RHI::BufferType::Default)
			usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		if ((type & RHI::BufferType::UniformTexel) != RHI::BufferType::Default)
			usage |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
		if ((type & RHI::BufferType::Storage) != RHI::BufferType::Default)
			usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		if ((type & RHI::BufferType::StorageTexel) != RHI::BufferType::Default)
			usage |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
		if ((type & RHI::BufferType::Indirect) != RHI::BufferType::Default)
			usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

		return usage;
	}

	VkImageType GetImageType(RHI::TextureType type)
	{
		assert(u8(type) < u8(RHI::TextureType::Count));
		return Tables::g_ImageTypes[u8(type)];
	}

	VkImageViewType GetImageViewType(RHI::TextureType type)
	{
		assert(u8(type) < u8(RHI::TextureType::Count));
		return Tables::g_ImageViewTypes[u8(type)];
	}

	VkImageLayout GetImageLayout(const RHI::TextureLayout layout)
	{
		assert(u8(layout) < u8(RHI::TextureLayout::Count));
		return Tables::g_ImageLayouts[u8(layout)];
	}

	RHI::TextureLayout GetTextureLayout(VkImageLayout layout)
	{
		if (layout < VK_IMAGE_LAYOUT_PREINITIALIZED)
			return Tables::g_TextureLayouts[u8(layout)];

		switch (layout)
		{
		case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:	return RHI::TextureLayout::DepthReadOnlyWithStencil;
		case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:	return RHI::TextureLayout::DepthWithStencilReadOnly;
		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:								return RHI::TextureLayout::Present;
		default:															return RHI::TextureLayout::Count;
		}
	}

	VkAccessFlags GetImageTransitionAccessMode(RHI::PipelineStage stage, RHI::TextureLayout layout, bool src)
	{
		switch (stage) 
		{
			case RHI::PipelineStage::FragmentShader:
				return src ? VK_ACCESS_INPUT_ATTACHMENT_READ_BIT : 0;
			case RHI::PipelineStage::EarlyFragmentTest:
			case RHI::PipelineStage::LastFragmentTest:
				return src ? VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT : VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			case RHI::PipelineStage::ColorAttachmentOutput:
				return src ? VK_ACCESS_COLOR_ATTACHMENT_READ_BIT : VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			case RHI::PipelineStage::Transfer:
				return src ? VK_ACCESS_TRANSFER_READ_BIT : VK_ACCESS_TRANSFER_WRITE_BIT;
			default:
				return 0;
		}
	}

	VkFilter GetFilter(RHI::FilterMode filter)
	{
		assert(u8(filter) < u8(RHI::FilterMode::Count));
		return Tables::g_Filters[u8(filter)];
	}

	VkSamplerMipmapMode GetMipmapMode(RHI::FilterMode magFilter, RHI::FilterMode minFilter)
	{
		if (magFilter == RHI::FilterMode::Trilinear || minFilter == RHI::FilterMode::Trilinear)
			return VK_SAMPLER_MIPMAP_MODE_LINEAR;
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
	}

	VkSamplerAddressMode GetAddressMode(RHI::AddressMode addressMode)
	{
		assert(u8(addressMode) < u8(RHI::AddressMode::Count));
		return Tables::g_samplerAddressModes[u8(addressMode)];
	}

	VkCompareOp GetCompareOp(RHI::CompareOp compareOp)
	{
		assert(u8(compareOp) < u8(RHI::CompareOp::Count));
		return Tables::g_CompareOps[u8(compareOp)];
	}

	VkBorderColor GetBorderColor(const glm::vec4& color, b8 intType)
	{
		if (color.w < .5f)
			return intType ? VK_BORDER_COLOR_INT_TRANSPARENT_BLACK : VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		if (color.x + color.y + color.x < 1.5f)
			return intType ? VK_BORDER_COLOR_INT_OPAQUE_BLACK : VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		return intType ? VK_BORDER_COLOR_INT_OPAQUE_WHITE : VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Format																	  //
	////////////////////////////////////////////////////////////////////////////////
	VkFormat GetFormat(PixelFormat format)
	{
		switch (format.components)
		{
		default:
		case PixelFormatComponents::UNKOWN:				return VK_FORMAT_UNDEFINED;
		case PixelFormatComponents::R8:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_R8_UNORM;
			case PixelFormatTransform::SNORM:			return VK_FORMAT_R8_SNORM;
			case PixelFormatTransform::UINT:			return VK_FORMAT_R8_UINT;
			case PixelFormatTransform::SINT:			return VK_FORMAT_R8_SINT;
			case PixelFormatTransform::USCALED:			return VK_FORMAT_R8_USCALED;
			case PixelFormatTransform::SSCALED:			return VK_FORMAT_R8_SSCALED;
			case PixelFormatTransform::SRGB:			return VK_FORMAT_R8_SRGB;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::R8G8:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_R8G8_UNORM;
			case PixelFormatTransform::SNORM:			return VK_FORMAT_R8G8_SNORM;
			case PixelFormatTransform::UINT:			return VK_FORMAT_R8G8_UINT;
			case PixelFormatTransform::SINT:			return VK_FORMAT_R8G8_SINT;
			case PixelFormatTransform::USCALED:			return VK_FORMAT_R8G8_USCALED;
			case PixelFormatTransform::SSCALED:			return VK_FORMAT_R8G8_SSCALED;
			case PixelFormatTransform::SRGB:			return VK_FORMAT_R8G8_SRGB;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::R8G8B8:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_R8G8B8_UNORM;
			case PixelFormatTransform::SNORM:			return VK_FORMAT_R8G8B8_SNORM;
			case PixelFormatTransform::UINT:			return VK_FORMAT_R8G8B8_UINT;
			case PixelFormatTransform::SINT:			return VK_FORMAT_R8G8B8_SINT;
			case PixelFormatTransform::USCALED:			return VK_FORMAT_R8G8B8_USCALED;
			case PixelFormatTransform::SSCALED:			return VK_FORMAT_R8G8B8_SSCALED;
			case PixelFormatTransform::SRGB:			return VK_FORMAT_R8G8B8_SRGB;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::R8G8B8A8:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_R8G8B8A8_UNORM;
			case PixelFormatTransform::SNORM:			return VK_FORMAT_R8G8B8A8_SNORM;
			case PixelFormatTransform::UINT:			return VK_FORMAT_R8G8B8A8_UINT;
			case PixelFormatTransform::SINT:			return VK_FORMAT_R8G8B8A8_SINT;
			case PixelFormatTransform::USCALED:			return VK_FORMAT_R8G8B8A8_USCALED;
			case PixelFormatTransform::SSCALED:			return VK_FORMAT_R8G8B8A8_SSCALED;
			case PixelFormatTransform::SRGB:			return VK_FORMAT_R8G8B8A8_SRGB;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::B8G8R8A8:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_B8G8R8A8_UNORM;
			case PixelFormatTransform::SNORM:			return VK_FORMAT_B8G8R8A8_SNORM;
			case PixelFormatTransform::UINT:			return VK_FORMAT_B8G8R8A8_UINT;
			case PixelFormatTransform::SINT:			return VK_FORMAT_B8G8R8A8_SINT;
			case PixelFormatTransform::USCALED:			return VK_FORMAT_B8G8R8A8_USCALED;
			case PixelFormatTransform::SSCALED:			return VK_FORMAT_B8G8R8A8_SSCALED;
			case PixelFormatTransform::SRGB:			return VK_FORMAT_B8G8R8A8_SRGB;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::R16:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_R16_UNORM;
			case PixelFormatTransform::SNORM:			return VK_FORMAT_R16_SNORM;
			case PixelFormatTransform::UINT:			return VK_FORMAT_R16_UINT;
			case PixelFormatTransform::SINT:			return VK_FORMAT_R16_SINT;
			case PixelFormatTransform::USCALED:			return VK_FORMAT_R16_USCALED;
			case PixelFormatTransform::SSCALED:			return VK_FORMAT_R16_SSCALED;
			case PixelFormatTransform::SFLOAT:			return VK_FORMAT_R16_SFLOAT;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::R16G16:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_R16G16_UNORM;
			case PixelFormatTransform::SNORM:			return VK_FORMAT_R16G16_SNORM;
			case PixelFormatTransform::UINT:			return VK_FORMAT_R16G16_UINT;
			case PixelFormatTransform::SINT:			return VK_FORMAT_R16G16_SINT;
			case PixelFormatTransform::USCALED:			return VK_FORMAT_R16G16_USCALED;
			case PixelFormatTransform::SSCALED:			return VK_FORMAT_R16G16_SSCALED;
			case PixelFormatTransform::SFLOAT:			return VK_FORMAT_R16G16_SFLOAT;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::R16G16B16A16:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_R16G16B16A16_UNORM;
			case PixelFormatTransform::SNORM:			return VK_FORMAT_R16G16B16A16_SNORM;
			case PixelFormatTransform::UINT:			return VK_FORMAT_R16G16B16A16_UINT;
			case PixelFormatTransform::SINT:			return VK_FORMAT_R16G16B16A16_SINT;
			case PixelFormatTransform::USCALED:			return VK_FORMAT_R16G16B16A16_USCALED;
			case PixelFormatTransform::SSCALED:			return VK_FORMAT_R16G16B16A16_SSCALED;
			case PixelFormatTransform::SFLOAT:			return VK_FORMAT_R16G16B16A16_SFLOAT;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::R32:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UINT:			return VK_FORMAT_R32_UINT;
			case PixelFormatTransform::SINT:			return VK_FORMAT_R32_SINT;
			case PixelFormatTransform::SFLOAT:			return VK_FORMAT_R32_SFLOAT;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::R32G32:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UINT:			return VK_FORMAT_R32G32_UINT;
			case PixelFormatTransform::SINT:			return VK_FORMAT_R32G32_SINT;
			case PixelFormatTransform::SFLOAT:			return VK_FORMAT_R32G32_SFLOAT;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::R32G32B32:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UINT:			return VK_FORMAT_R32G32B32_UINT;
			case PixelFormatTransform::SINT:			return VK_FORMAT_R32G32B32_SINT;
			case PixelFormatTransform::SFLOAT:			return VK_FORMAT_R32G32B32_SFLOAT;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::R32G32B32A32:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UINT:			return VK_FORMAT_R32G32B32A32_UINT;
			case PixelFormatTransform::SINT:			return VK_FORMAT_R32G32B32A32_SINT;
			case PixelFormatTransform::SFLOAT:			return VK_FORMAT_R32G32B32A32_SFLOAT;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::R5G6B5:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_R5G6B5_UNORM_PACK16;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::R11G11B10: // Note: Needs to be used as B10G11R11 for vulkan
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UFLOAT:			return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::A2R10G10B10:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
			case PixelFormatTransform::SNORM:			return VK_FORMAT_A2R10G10B10_SNORM_PACK32;
			case PixelFormatTransform::UINT:			return VK_FORMAT_A2R10G10B10_UINT_PACK32;
			case PixelFormatTransform::SINT:			return VK_FORMAT_A2R10G10B10_SINT_PACK32;
			case PixelFormatTransform::USCALED:			return VK_FORMAT_A2R10G10B10_USCALED_PACK32;
			case PixelFormatTransform::SSCALED:			return VK_FORMAT_A2R10G10B10_SSCALED_PACK32;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::D24:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_X8_D24_UNORM_PACK32;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::D24S8:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_D24_UNORM_S8_UINT;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::D32:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::SFLOAT:			return VK_FORMAT_D32_SFLOAT;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::D32S8:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::SFLOAT:			return VK_FORMAT_D32_SFLOAT_S8_UINT;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::BC1:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
			case PixelFormatTransform::SRGB:			return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::BC2:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_BC2_UNORM_BLOCK;
			case PixelFormatTransform::SRGB:			return VK_FORMAT_BC2_SRGB_BLOCK;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::BC3:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_BC3_UNORM_BLOCK;
			case PixelFormatTransform::SRGB:			return VK_FORMAT_BC3_SRGB_BLOCK;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::BC4:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_BC4_UNORM_BLOCK;
			case PixelFormatTransform::SNORM:			return VK_FORMAT_BC4_SNORM_BLOCK;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::BC5:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_BC5_UNORM_BLOCK;
			case PixelFormatTransform::SNORM:			return VK_FORMAT_BC5_SNORM_BLOCK;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::BC6H:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UFLOAT:			return VK_FORMAT_BC6H_UFLOAT_BLOCK;
			case PixelFormatTransform::SFLOAT:			return VK_FORMAT_BC6H_SFLOAT_BLOCK;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::BC7:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_BC7_UNORM_BLOCK;
			case PixelFormatTransform::SRGB:			return VK_FORMAT_BC7_SRGB_BLOCK;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::ASTC4X4:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
			case PixelFormatTransform::SRGB:			return VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::ASTC6X6:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
			case PixelFormatTransform::SRGB:			return VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::ASTC8X8:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
			case PixelFormatTransform::SRGB:			return VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::ASTC10X10:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
			case PixelFormatTransform::SRGB:			return VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		case PixelFormatComponents::ASTC12X12:
		{
			switch (format.transform)
			{
			case PixelFormatTransform::UNDEFINED:
			case PixelFormatTransform::UNORM:			return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
			case PixelFormatTransform::SRGB:			return VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
			default:									return VK_FORMAT_UNDEFINED;
			}
		}
		}
	}

	PixelFormat GetPixelFormat(VkFormat vkformat)
	{
#define FORMAT(pixFormat, pixTransform) return { PixelFormatComponents::pixFormat, PixelFormatTransform::pixTransform }
#define FORMAT_UNKOWN FORMAT(UNKOWN, UNDEFINED)

		switch (vkformat)
		{
		default:
		case VK_FORMAT_UNDEFINED:									FORMAT_UNKOWN;
		case VK_FORMAT_R4G4_UNORM_PACK8:							FORMAT_UNKOWN;
		case VK_FORMAT_R4G4B4A4_UNORM_PACK16:						FORMAT_UNKOWN;
		case VK_FORMAT_B4G4R4A4_UNORM_PACK16:						FORMAT_UNKOWN;
		case VK_FORMAT_R5G6B5_UNORM_PACK16:							FORMAT(R5G6B5, UNORM);
		case VK_FORMAT_B5G6R5_UNORM_PACK16:							FORMAT_UNKOWN;
		case VK_FORMAT_R5G5B5A1_UNORM_PACK16:						FORMAT_UNKOWN;
		case VK_FORMAT_B5G5R5A1_UNORM_PACK16:						FORMAT_UNKOWN;
		case VK_FORMAT_A1R5G5B5_UNORM_PACK16:						FORMAT_UNKOWN;
		case VK_FORMAT_R8_UNORM:									FORMAT(R8, UNORM);
		case VK_FORMAT_R8_SNORM:									FORMAT(R8, SNORM);
		case VK_FORMAT_R8_USCALED:									FORMAT(R8, USCALED);
		case VK_FORMAT_R8_SSCALED:									FORMAT(R8, SSCALED);
		case VK_FORMAT_R8_UINT:										FORMAT(R8, UINT);
		case VK_FORMAT_R8_SINT:										FORMAT(R8, SINT);
		case VK_FORMAT_R8_SRGB:										FORMAT(R8, SRGB);
		case VK_FORMAT_R8G8_UNORM:									FORMAT(R8G8, UNORM);
		case VK_FORMAT_R8G8_SNORM:									FORMAT(R8G8, SNORM);
		case VK_FORMAT_R8G8_USCALED:								FORMAT(R8G8, USCALED);
		case VK_FORMAT_R8G8_SSCALED:								FORMAT(R8G8, SSCALED);
		case VK_FORMAT_R8G8_UINT:									FORMAT(R8G8, UINT);
		case VK_FORMAT_R8G8_SINT:									FORMAT(R8G8, SINT);
		case VK_FORMAT_R8G8_SRGB:									FORMAT(R8G8, SRGB);
		case VK_FORMAT_R8G8B8_UNORM:								FORMAT(R8G8B8, UNORM);
		case VK_FORMAT_R8G8B8_SNORM:								FORMAT(R8G8B8, SNORM);
		case VK_FORMAT_R8G8B8_USCALED:								FORMAT(R8G8B8, USCALED);
		case VK_FORMAT_R8G8B8_SSCALED:								FORMAT(R8G8B8, SSCALED);
		case VK_FORMAT_R8G8B8_UINT:									FORMAT(R8G8B8, UINT);
		case VK_FORMAT_R8G8B8_SINT:									FORMAT(R8G8B8, SINT);
		case VK_FORMAT_R8G8B8_SRGB:									FORMAT(R8G8B8, SRGB);
		case VK_FORMAT_B8G8R8_UNORM:								FORMAT_UNKOWN;
		case VK_FORMAT_B8G8R8_SNORM:								FORMAT_UNKOWN;
		case VK_FORMAT_B8G8R8_USCALED:								FORMAT_UNKOWN;
		case VK_FORMAT_B8G8R8_SSCALED:								FORMAT_UNKOWN;
		case VK_FORMAT_B8G8R8_UINT:									FORMAT_UNKOWN;
		case VK_FORMAT_B8G8R8_SINT:									FORMAT_UNKOWN;
		case VK_FORMAT_B8G8R8_SRGB:									FORMAT_UNKOWN;
		case VK_FORMAT_R8G8B8A8_UNORM:								FORMAT(R8G8B8A8, UNORM);
		case VK_FORMAT_R8G8B8A8_SNORM:								FORMAT(R8G8B8A8, SNORM);
		case VK_FORMAT_R8G8B8A8_USCALED:							FORMAT(R8G8B8A8, USCALED);
		case VK_FORMAT_R8G8B8A8_SSCALED:							FORMAT(R8G8B8A8, SSCALED);
		case VK_FORMAT_R8G8B8A8_UINT:								FORMAT(R8G8B8A8, UINT);
		case VK_FORMAT_R8G8B8A8_SINT:								FORMAT(R8G8B8A8, SINT);
		case VK_FORMAT_R8G8B8A8_SRGB:								FORMAT(R8G8B8A8, SRGB);
		case VK_FORMAT_B8G8R8A8_UNORM:								FORMAT(B8G8R8A8, UNORM);
		case VK_FORMAT_B8G8R8A8_SNORM:								FORMAT(B8G8R8A8, SNORM);
		case VK_FORMAT_B8G8R8A8_USCALED:							FORMAT(B8G8R8A8, USCALED);
		case VK_FORMAT_B8G8R8A8_SSCALED:							FORMAT(B8G8R8A8, SSCALED);
		case VK_FORMAT_B8G8R8A8_UINT:								FORMAT(B8G8R8A8, UINT);
		case VK_FORMAT_B8G8R8A8_SINT:								FORMAT(B8G8R8A8, SINT);
		case VK_FORMAT_B8G8R8A8_SRGB:								FORMAT(B8G8R8A8, SRGB);
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32:						FORMAT_UNKOWN;
		case VK_FORMAT_A8B8G8R8_SNORM_PACK32:						FORMAT_UNKOWN;
		case VK_FORMAT_A8B8G8R8_USCALED_PACK32:						FORMAT_UNKOWN;
		case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:						FORMAT_UNKOWN;
		case VK_FORMAT_A8B8G8R8_UINT_PACK32:						FORMAT_UNKOWN;
		case VK_FORMAT_A8B8G8R8_SINT_PACK32:						FORMAT_UNKOWN;
		case VK_FORMAT_A8B8G8R8_SRGB_PACK32:						FORMAT_UNKOWN;
		case VK_FORMAT_A2R10G10B10_UNORM_PACK32:					FORMAT_UNKOWN;
		case VK_FORMAT_A2R10G10B10_SNORM_PACK32:					FORMAT_UNKOWN;
		case VK_FORMAT_A2R10G10B10_USCALED_PACK32:					FORMAT_UNKOWN;
		case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:					FORMAT_UNKOWN;
		case VK_FORMAT_A2R10G10B10_UINT_PACK32:						FORMAT_UNKOWN;
		case VK_FORMAT_A2R10G10B10_SINT_PACK32:						FORMAT_UNKOWN;
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32:					FORMAT_UNKOWN;
		case VK_FORMAT_A2B10G10R10_SNORM_PACK32:					FORMAT_UNKOWN;
		case VK_FORMAT_A2B10G10R10_USCALED_PACK32:					FORMAT_UNKOWN;
		case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:					FORMAT_UNKOWN;
		case VK_FORMAT_A2B10G10R10_UINT_PACK32:						FORMAT_UNKOWN;
		case VK_FORMAT_A2B10G10R10_SINT_PACK32:						FORMAT_UNKOWN;
		case VK_FORMAT_R16_UNORM:									FORMAT(R16, UNORM);
		case VK_FORMAT_R16_SNORM:									FORMAT(R16, SNORM);
		case VK_FORMAT_R16_USCALED:									FORMAT(R16, USCALED);
		case VK_FORMAT_R16_SSCALED:									FORMAT(R16, SSCALED);
		case VK_FORMAT_R16_UINT:									FORMAT(R16, UINT);
		case VK_FORMAT_R16_SINT:									FORMAT(R16, SINT);
		case VK_FORMAT_R16_SFLOAT:									FORMAT(R16, SFLOAT);
		case VK_FORMAT_R16G16_UNORM:								FORMAT(R16G16, UNORM);
		case VK_FORMAT_R16G16_SNORM:								FORMAT(R16G16, SNORM);
		case VK_FORMAT_R16G16_USCALED:								FORMAT(R16G16, USCALED);
		case VK_FORMAT_R16G16_SSCALED:								FORMAT(R16G16, SSCALED);
		case VK_FORMAT_R16G16_UINT:									FORMAT(R16G16, UINT);
		case VK_FORMAT_R16G16_SINT:									FORMAT(R16G16, SINT);
		case VK_FORMAT_R16G16_SFLOAT:								FORMAT(R16G16, SFLOAT);
		case VK_FORMAT_R16G16B16_UNORM:								FORMAT_UNKOWN;
		case VK_FORMAT_R16G16B16_SNORM:								FORMAT_UNKOWN;
		case VK_FORMAT_R16G16B16_USCALED:							FORMAT_UNKOWN;
		case VK_FORMAT_R16G16B16_SSCALED:							FORMAT_UNKOWN;
		case VK_FORMAT_R16G16B16_UINT:								FORMAT_UNKOWN;
		case VK_FORMAT_R16G16B16_SINT:								FORMAT_UNKOWN;
		case VK_FORMAT_R16G16B16_SFLOAT:							FORMAT_UNKOWN;
		case VK_FORMAT_R16G16B16A16_UNORM:							FORMAT(R16G16B16A16, UNORM);
		case VK_FORMAT_R16G16B16A16_SNORM:							FORMAT(R16G16B16A16, SNORM);
		case VK_FORMAT_R16G16B16A16_USCALED:						FORMAT(R16G16B16A16, USCALED);
		case VK_FORMAT_R16G16B16A16_SSCALED:						FORMAT(R16G16B16A16, SSCALED);
		case VK_FORMAT_R16G16B16A16_UINT:							FORMAT(R16G16B16A16, UINT);
		case VK_FORMAT_R16G16B16A16_SINT:							FORMAT(R16G16B16A16, SINT);
		case VK_FORMAT_R16G16B16A16_SFLOAT:							FORMAT(R16G16B16A16, SFLOAT);
		case VK_FORMAT_R32_UINT:									FORMAT(R32, UINT);
		case VK_FORMAT_R32_SINT:									FORMAT(R32, SINT);
		case VK_FORMAT_R32_SFLOAT:									FORMAT(R32, SFLOAT);
		case VK_FORMAT_R32G32_UINT:									FORMAT(R32G32, UINT);
		case VK_FORMAT_R32G32_SINT:									FORMAT(R32G32, SINT);
		case VK_FORMAT_R32G32_SFLOAT:								FORMAT(R32G32, SFLOAT);
		case VK_FORMAT_R32G32B32_UINT:								FORMAT(R32G32B32, UINT);
		case VK_FORMAT_R32G32B32_SINT:								FORMAT(R32G32B32, SINT);
		case VK_FORMAT_R32G32B32_SFLOAT:							FORMAT(R32G32B32, SFLOAT);
		case VK_FORMAT_R32G32B32A32_UINT:							FORMAT(R32G32B32A32, UINT);
		case VK_FORMAT_R32G32B32A32_SINT:							FORMAT(R32G32B32A32, SINT);
		case VK_FORMAT_R32G32B32A32_SFLOAT:							FORMAT(R32G32B32A32, SFLOAT);
		case VK_FORMAT_R64_UINT:									FORMAT_UNKOWN;
		case VK_FORMAT_R64_SINT:									FORMAT_UNKOWN;
		case VK_FORMAT_R64_SFLOAT:									FORMAT_UNKOWN;
		case VK_FORMAT_R64G64_UINT:									FORMAT_UNKOWN;
		case VK_FORMAT_R64G64_SINT:									FORMAT_UNKOWN;
		case VK_FORMAT_R64G64_SFLOAT:								FORMAT_UNKOWN;
		case VK_FORMAT_R64G64B64_UINT:								FORMAT_UNKOWN;
		case VK_FORMAT_R64G64B64_SINT:								FORMAT_UNKOWN;
		case VK_FORMAT_R64G64B64_SFLOAT:							FORMAT_UNKOWN;
		case VK_FORMAT_R64G64B64A64_UINT:							FORMAT_UNKOWN;
		case VK_FORMAT_R64G64B64A64_SINT:							FORMAT_UNKOWN;
		case VK_FORMAT_R64G64B64A64_SFLOAT:							FORMAT_UNKOWN;
		case VK_FORMAT_B10G11R11_UFLOAT_PACK32:						FORMAT_UNKOWN;
		case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:						FORMAT_UNKOWN;
		case VK_FORMAT_D16_UNORM:									FORMAT_UNKOWN;
		case VK_FORMAT_X8_D24_UNORM_PACK32:							FORMAT(D24, UNORM);
		case VK_FORMAT_D32_SFLOAT:									FORMAT(D32, SFLOAT);
		case VK_FORMAT_S8_UINT:										FORMAT_UNKOWN;
		case VK_FORMAT_D16_UNORM_S8_UINT:							FORMAT_UNKOWN;
		case VK_FORMAT_D24_UNORM_S8_UINT:							FORMAT(D24S8, UNORM);
		case VK_FORMAT_D32_SFLOAT_S8_UINT:							FORMAT(D32S8, SFLOAT);
		case VK_FORMAT_BC1_RGB_UNORM_BLOCK:							FORMAT_UNKOWN;
		case VK_FORMAT_BC1_RGB_SRGB_BLOCK:							FORMAT_UNKOWN;
		case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:						FORMAT(BC1, UNORM);
		case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:							FORMAT(BC1, SRGB);
		case VK_FORMAT_BC2_UNORM_BLOCK:								FORMAT(BC2, UNORM);
		case VK_FORMAT_BC2_SRGB_BLOCK:								FORMAT(BC2, SRGB);
		case VK_FORMAT_BC3_UNORM_BLOCK:								FORMAT(BC3, UNORM);
		case VK_FORMAT_BC3_SRGB_BLOCK:								FORMAT(BC3, SRGB);
		case VK_FORMAT_BC4_UNORM_BLOCK:								FORMAT(BC4, UNORM);
		case VK_FORMAT_BC4_SNORM_BLOCK:								FORMAT(BC4, SNORM);
		case VK_FORMAT_BC5_UNORM_BLOCK:								FORMAT(BC5, UNORM);
		case VK_FORMAT_BC5_SNORM_BLOCK:								FORMAT(BC5, SNORM);
		case VK_FORMAT_BC6H_UFLOAT_BLOCK:							FORMAT(BC6H, UNORM);
		case VK_FORMAT_BC6H_SFLOAT_BLOCK:							FORMAT(BC6H, SFLOAT);
		case VK_FORMAT_BC7_UNORM_BLOCK:								FORMAT(BC7, UNORM);
		case VK_FORMAT_BC7_SRGB_BLOCK:								FORMAT(BC7, SRGB);
		case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:						FORMAT_UNKOWN;
		case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:						FORMAT_UNKOWN;
		case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:					FORMAT_UNKOWN;
		case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:					FORMAT_UNKOWN;
		case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:					FORMAT_UNKOWN;
		case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:					FORMAT_UNKOWN;
		case VK_FORMAT_EAC_R11_UNORM_BLOCK:							FORMAT_UNKOWN;
		case VK_FORMAT_EAC_R11_SNORM_BLOCK:							FORMAT_UNKOWN;
		case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:						FORMAT_UNKOWN;
		case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:						FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:						FORMAT(ASTC4X4, UNORM);
		case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:							FORMAT(ASTC4X4, SRGB);
		case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:						FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:							FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:						FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:							FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:						FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:							FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:						FORMAT(ASTC6X6, UNORM);
		case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:							FORMAT(ASTC6X6, SRGB);
		case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:						FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:							FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:						FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:							FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:						FORMAT(ASTC8X8, UNORM);
		case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:							FORMAT(ASTC8X8, SRGB);
		case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:						FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:						FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:						FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:						FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:						FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:						FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:						FORMAT(ASTC10X10, UNORM);
		case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:						FORMAT(ASTC10X10, SRGB);
		case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:						FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:						FORMAT_UNKOWN;
		case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:						FORMAT(ASTC12X12, UNORM);
		case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:						FORMAT(ASTC12X12, SRGB);
		case VK_FORMAT_G8B8G8R8_422_UNORM:							FORMAT_UNKOWN;
		case VK_FORMAT_B8G8R8G8_422_UNORM:							FORMAT_UNKOWN;
		case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:					FORMAT_UNKOWN;
		case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:					FORMAT_UNKOWN;
		case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:					FORMAT_UNKOWN;
		case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:					FORMAT_UNKOWN;
		case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:					FORMAT_UNKOWN;
		case VK_FORMAT_R10X6_UNORM_PACK16:							FORMAT_UNKOWN;
		case VK_FORMAT_R10X6G10X6_UNORM_2PACK16:					FORMAT_UNKOWN;
		case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:			FORMAT_UNKOWN;
		case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:		FORMAT_UNKOWN;
		case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:		FORMAT_UNKOWN;
		case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:	FORMAT_UNKOWN;
		case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:	FORMAT_UNKOWN;
		case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:	FORMAT_UNKOWN;
		case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:	FORMAT_UNKOWN;
		case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:	FORMAT_UNKOWN;
		case VK_FORMAT_R12X4_UNORM_PACK16:							FORMAT_UNKOWN;
		case VK_FORMAT_R12X4G12X4_UNORM_2PACK16:					FORMAT_UNKOWN;
		case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:			FORMAT_UNKOWN;
		case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:		FORMAT_UNKOWN;
		case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:		FORMAT_UNKOWN;
		case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:	FORMAT_UNKOWN;
		case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:	FORMAT_UNKOWN;
		case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:	FORMAT_UNKOWN;
		case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:	FORMAT_UNKOWN;
		case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:	FORMAT_UNKOWN;
		case VK_FORMAT_G16B16G16R16_422_UNORM:						FORMAT_UNKOWN;
		case VK_FORMAT_B16G16R16G16_422_UNORM:						FORMAT_UNKOWN;
		case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:				FORMAT_UNKOWN;
		case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:					FORMAT_UNKOWN;
		case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:				FORMAT_UNKOWN;
		case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:					FORMAT_UNKOWN;
		case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:				FORMAT_UNKOWN;
		case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:					FORMAT_UNKOWN;
		case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:					FORMAT_UNKOWN;
		case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:					FORMAT_UNKOWN;
		case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:					FORMAT_UNKOWN;
		case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:					FORMAT_UNKOWN;
		case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:					FORMAT_UNKOWN;
		case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:					FORMAT_UNKOWN;
		case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:					FORMAT_UNKOWN;
		}

#undef FORMAT_UNKOWN
#undef FORMAT
	}

#define AND_FEATURE(feature) requestedFeatures.feature &= availableFeatures.feature;
	////////////////////////////////////////////////////////////////////////////////
	// Other																	  //
	////////////////////////////////////////////////////////////////////////////////
	void DisableUnsupportedDeviceFeatures(const VkPhysicalDeviceFeatures& availableFeatures,
										 VkPhysicalDeviceFeatures& requestedFeatures)
	{
		AND_FEATURE(robustBufferAccess);
		AND_FEATURE(fullDrawIndexUint32);
		AND_FEATURE(imageCubeArray);
		AND_FEATURE(independentBlend);
		AND_FEATURE(geometryShader);
		AND_FEATURE(tessellationShader);
		AND_FEATURE(sampleRateShading);
		AND_FEATURE(dualSrcBlend);
		AND_FEATURE(logicOp);
		AND_FEATURE(multiDrawIndirect);
		AND_FEATURE(drawIndirectFirstInstance);
		AND_FEATURE(depthClamp);
		AND_FEATURE(depthBiasClamp);
		AND_FEATURE(fillModeNonSolid);
		AND_FEATURE(depthBounds);
		AND_FEATURE(wideLines);
		AND_FEATURE(largePoints);
		AND_FEATURE(alphaToOne);
		AND_FEATURE(multiViewport);
		AND_FEATURE(samplerAnisotropy);
		AND_FEATURE(textureCompressionETC2);
		AND_FEATURE(textureCompressionASTC_LDR);
		AND_FEATURE(textureCompressionBC);
		AND_FEATURE(occlusionQueryPrecise);
		AND_FEATURE(pipelineStatisticsQuery);
		AND_FEATURE(vertexPipelineStoresAndAtomics);
		AND_FEATURE(fragmentStoresAndAtomics);
		AND_FEATURE(shaderTessellationAndGeometryPointSize);
		AND_FEATURE(shaderImageGatherExtended);
		AND_FEATURE(shaderStorageImageExtendedFormats);
		AND_FEATURE(shaderStorageImageMultisample);
		AND_FEATURE(shaderStorageImageReadWithoutFormat);
		AND_FEATURE(shaderStorageImageWriteWithoutFormat);
		AND_FEATURE(shaderUniformBufferArrayDynamicIndexing);
		AND_FEATURE(shaderSampledImageArrayDynamicIndexing);
		AND_FEATURE(shaderStorageBufferArrayDynamicIndexing);
		AND_FEATURE(shaderStorageImageArrayDynamicIndexing);
		AND_FEATURE(shaderClipDistance);
		AND_FEATURE(shaderCullDistance);
		AND_FEATURE(shaderFloat64);
		AND_FEATURE(shaderInt64);
		AND_FEATURE(shaderInt16);
		AND_FEATURE(shaderResourceResidency);
		AND_FEATURE(shaderResourceMinLod);
		AND_FEATURE(sparseBinding);
		AND_FEATURE(sparseResidencyBuffer);
		AND_FEATURE(sparseResidencyImage2D);
		AND_FEATURE(sparseResidencyImage3D);
		AND_FEATURE(sparseResidency2Samples);
		AND_FEATURE(sparseResidency4Samples);
		AND_FEATURE(sparseResidency8Samples);
		AND_FEATURE(sparseResidency16Samples);
		AND_FEATURE(sparseResidencyAliased);
		AND_FEATURE(variableMultisampleRate);
		AND_FEATURE(inheritedQueries);
	}
#undef AND_FEATURE

	VkShaderStageFlagBits GetShaderStage(RHI::ShaderType type)
	{
		VkShaderStageFlags stage = 0;
		if ((type & RHI::ShaderType::Vertex) != RHI::ShaderType::None)
			stage |= VK_SHADER_STAGE_VERTEX_BIT;
		if ((type & RHI::ShaderType::Geometry) != RHI::ShaderType::None)
			stage |= VK_SHADER_STAGE_GEOMETRY_BIT;
		if ((type & RHI::ShaderType::Hull) != RHI::ShaderType::None)
			stage |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		if ((type & RHI::ShaderType::Domain) != RHI::ShaderType::None)
			stage |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		if ((type & RHI::ShaderType::Fragment) != RHI::ShaderType::None)
			stage |= VK_SHADER_STAGE_FRAGMENT_BIT;
		if ((type & RHI::ShaderType::Compute) != RHI::ShaderType::None)
			stage |= VK_SHADER_STAGE_COMPUTE_BIT;

		return VkShaderStageFlagBits(stage);
	}

	VkDescriptorType GetDescriptorType(RHI::DescriptorSetBindingType type)
	{
		assert(u8(type) < u8(RHI::DescriptorSetBindingType::Count));
		return Tables::g_DescriptorTypes[u8(type)];
	}

	VkIndexType GetIndexType(RHI::IndexType type)
	{
		return type == RHI::IndexType::UInt ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16;
	}
} }
