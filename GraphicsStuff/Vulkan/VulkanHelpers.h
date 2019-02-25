#pragma once
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "VulkanDescriptorSet.h"
#include "VulkanSampler.h"
#include "VulkanTexture.h"
#include "../RHI/InputDescriptor.h"
#include "../RHI/PixelFormat.h"
#include "../RHI/GpuInfo.h"

namespace Vulkan { namespace Helpers {
	
	////////////////////////////////////////////////////////////////////////////////
	// Debug/Error/etc info														  //
	////////////////////////////////////////////////////////////////////////////////
	/**
	 * Convert a VkResult into a C-string
	 * @param[in] result	Vulkan result
	 * @return				Vulkan result as a string
	 */
	const char* GetResultString(VkResult result);
	/**
	 * Convert a debug report object type into a C-string
	 * @param[in] type	Vulkan debug report object type
	 * @return			Vulkan debug report object type as a string
	 */
	const char* GetObjectTypeString(VkDebugReportObjectTypeEXT type);

	/**
	 * Get the log level for a debug report flag
	 * @param[in] flags		Vulkan debug report flag
	 * @return				Log level
	 */
	//LogLevel GetDebugReportLogLevel(VkDebugReportFlagsEXT flags);

	/**
	 * Get the GPU vendor from its Id
	 * @param[in] vendorId	Vendor Id
	 * @return				Vendor
	 */
	RHI::GpuVendor GetVendor(u32 vendorId);
	/**
	 * Get the GPU type from the a device type
	 * @param[in] type	Device type
	 * @return			GPU type
	 */
	RHI::GpuType GetGpuType(VkPhysicalDeviceType type);

	/**
	 * Convert the vulkan API version to a Hive API version
	 * @param[in] version	Vulkan API version
	 * @return				Hive API version
	 */
	//RHI::ApiVersion GetApiVersion(u32 version);
	/**
	 * Convert the vulkan driver version to a Hive driver version
	 * @param[in] version	Vulkan driver version
	 * @return				Hive driver version
	 */
	RHI::DriverVersion GetDriverVersion(u32 version);

	////////////////////////////////////////////////////////////////////////////////
	// Surface and swapchain													  //
	////////////////////////////////////////////////////////////////////////////////
	/**
	 * Select the most optimal surface format for a swapchain to use
	 * @param[in] formats	All available formats
	 * @return				Optimal surface format
	 * @note				VK_FORMAT_B8G8R8A8_UNORM will be chosen if it exists, since it is the most optimal format
	 */
	VkSurfaceFormatKHR GetOptimalSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
	/**
	 * Find the optimal present mode for the vsync mode
	 * @param[in] modes		All available present modes
	 * @param[inout] vsync	Requested v-sync mode, will be changed depending on present support
	 * @return				Optimal present mode
	 */
	VkPresentModeKHR GetOptimalPresentMode(const std::vector<VkPresentModeKHR>& modes, RHI::VSyncMode& vsync);
	/**
	 * Get the swapchain extent
	 * @param[in] size			Window size
	 * @param[in] capabilities	Vulkan surface capabilities
	 */
	VkExtent2D GetSwapExtent(glm::uvec2 size, const VkSurfaceCapabilitiesKHR& capabilities);

	////////////////////////////////////////////////////////////////////////////////
	// Render pass																  //
	////////////////////////////////////////////////////////////////////////////////
	/**
	 * Convert a load op to a vulkan load op
	 * @param[in] loadOp	Load op
	 * @return				Vulkan load op
	 */
	VkAttachmentLoadOp GetLoadOp(RHI::LoadOp loadOp);
	/**
	 * Convert a store op to a vulkan store op
	 * @param[in] storeOp	Store op
	 * @return				Vulkan store op
	 */
	VkAttachmentStoreOp GetStoreOp(RHI::StoreOp storeOp);VkAttachmentLoadOp GetLoadOp(RHI::LoadOp loadOp);
	/**
	 * Convert a store op to a vulkan store op
	 * @param[in] storeOp	Store op
	 * @return				Vulkan store op
	 */
	VkAttachmentStoreOp GetStoreOp(RHI::StoreOp storeOp);
	/**
	* Get the representatice vulkan image layout from a render target type
	* @param[in] type	Render target type
	* @return			Vulkan image layout
	*/
	VkImageLayout GetSubpassAttachmentLayout(RHI::RenderTargetType type);

	////////////////////////////////////////////////////////////////////////////////
	// Pipeline																	  //
	////////////////////////////////////////////////////////////////////////////////
	/**
	 * Convert a primitive topology to a vulkan primitive topology
	 * @param[in] topology				Primitive topology
	 * @return							Vulkan primitive topology
	 */
	VkPrimitiveTopology GetPrimitiveTopology(RHI::PrimitiveTopology topology);
	/**
	 * Convert a fill mode to a vulkan polygon mode
	 * @param[in] fillMode	Fill mode
	 * @return				Vulkan polygon mode
	 */
	VkPolygonMode GetPolygonMode(RHI::FillMode fillMode);
	/**
	 * Convert a cull mode to a vulkan cull mode
	 * @param[in] cullMode	Cull mode
	 * @return				Vulkan cull mode
	 */
	VkCullModeFlagBits GetCullMode(RHI::CullMode cullMode);
	/**
	 * Convert a front face to a vulkan front face
	 * @param[in] frontFace	Front face
	 * @return				Vulkan front face
	 */
	VkFrontFace GetFrontFace(RHI::FrontFace frontFace);
	/**
	 * Convert a sample count to a vulkan sample count
	 * @param[in] samples	Sample count
	 * @return				Vulkan sample count
	 */
	VkSampleCountFlagBits GetSampleCount(RHI::SampleCount samples);
	/**
	 * Convert a components mask to a vulkan color component mask
	 * @param[in] components		Components mask
	 * @return					Vulkan components mask
	 */
	VkColorComponentFlags GetColorWriteMask(RHI::ColorComponentMask components);
	/**
	 * Convert a blend op to a blend op
	 * @param[in] blendOp	Blend op
	 * @return				Vulkan blend op
	 */
	VkBlendOp GetBlendOp(RHI::BlendOp blendOp);
	/**
	 * Convert a blend factor to a vulkan blend factor
	 * @param[in] blendFactor	Blend factor
	 * @return					Vulkan blend factor
	 */
	VkBlendFactor GetBlendFactor(RHI::BlendFactor blendFactor);
	/**
	 * Convert a logic op to a vulkan logic op
	 * @param[in] logicOp	Logic op
	 * @return				Vulkan logic op
	 */
	VkLogicOp GetLogicOp(RHI::LogicOp logicOp);
	/**
	 * Get the vulkan graphics bind point from the pipeline type
	 * @param[in] type	Pipeline type
	 * @return			Vulkan pipeline bind point
	 */
	VkPipelineBindPoint GetPipelineBindPoint(RHI::PipelineType type);
	/**
	 * Convert a pipeline stage to a vulkan pipeline stage
	 * @param[in] pipelineStage		Pipeline stage
	 * @return						Vulkan pipeline stage
	 */
	VkPipelineStageFlags GetPipelineStage(RHI::PipelineStage pipelineStage);

	/**
	 * Convert aan input element type to a vulkan format
	 * @param[in] type		Pipeline stage
	 * @return						Vulkan pipeline stage
	 */
	VkFormat GetInputFormat(RHI::InputElementType type);
	/**
	 * Convert the input description to vulkan input binding and attrivute descriptions
	 * @param[in] inputDescriptor	Input descriptor
	 * @param[out] bindings			Vulkan vertex input binding descriptions
	 * @param[out] attributes		Vulkan vertex input attrivute descriptions
	 */
	void GetInputDescription(const RHI::InputDescriptor& inputDescriptor, std::vector<VkVertexInputBindingDescription>& bindings, std::vector<VkVertexInputAttributeDescription>& attributes);

	/**
	* Convert a stencil op to a vulkan stencil op
	* @param[in] stencilOp	Stencil op
	* @return				Vulkan stencil op
	*/
	VkStencilOp GetStencilOp(RHI::StencilOp stencilOp);
	/**
	 * Convert dynamic state flags to an array of vulkan dynamic states
	 * @param[in] dynamicState	Dynamic state flags
	 * @return					std::vector of vulkan dynamic states
	 */
	std::vector<VkDynamicState> GetDynamicStates(RHI::DynamicState dynamicState);

	////////////////////////////////////////////////////////////////////////////////
	// Buffer																	  //
	////////////////////////////////////////////////////////////////////////////////
	/**
	 * Get the vulkan buffer usage from the buffer flags
	 * @param[in] flags		Buffer flags
	 * @return				Vulkan buffer usage
	 */
	VkBufferUsageFlags GetBufferUsage(RHI::BufferType type, const RHI::BufferFlags& flags);

	////////////////////////////////////////////////////////////////////////////////
	// Image																	  //
	////////////////////////////////////////////////////////////////////////////////
	/**
	 * Get the vulkan image type from the texture type
	 * @param[in] type		Texture type
	 * @return				Vulkan image type
	 */
	VkImageType GetImageType(RHI::TextureType type);
	/**
	 * Get the vulkan image view type from the texture type
	 * @param[in] type		Texture type
	 * @return				Vulkan image view type
	 */
	VkImageViewType GetImageViewType(RHI::TextureType type);
	/**
	 * Get the vulkan image layout from the texture layout
	 * @param[in] layout	Texture layout
	 * @return				Vulkan image layout
	 */
	VkImageLayout GetImageLayout(RHI::TextureLayout layout);
	/**
	 * Get the hive image texture from the image layout
	 * @param[in] layout	Vulkan image layout
	 * @return				Texture layout
	 */
	RHI::TextureLayout GetTextureLayout(VkImageLayout layout);
	/**
	 * Get the corresponding vulkan access mode from the texture layout for a transition
	 * @param[in] layout	Texture layout
	 * @return				Corresponding vulkan access mode
	 */
	VkAccessFlags GetImageTransitionAccessMode(RHI::TextureLayout layout);

	////////////////////////////////////////////////////////////////////////////////
	// Sampler																	  //
	////////////////////////////////////////////////////////////////////////////////
	/**
	 * Get the vulkan sampler filter mode from the filter mode
	 * @param[in] filter	Filter mode
	 * @return				Vulkan sampler filter mode
	 */
	VkFilter GetFilter(RHI::FilterMode filter);
	/**
	 * Get the vulkan sampler filter mode from the filter mode
	 * @param[in] magFilter		Magnification filter mode
	 * @param[in] minFilter		Minification filter mode
	 * @return					Vulkan sampler mipmap mode
	 */
	VkSamplerMipmapMode GetMipmapMode(RHI::FilterMode magFilter, RHI::FilterMode minFilter);
	/**
	 * Get the vulkan sampler address mode from the address mode
	 * @param[in] addressMode	Address mode
	 * @return					Vulkan sampler address mode
	 */
	VkSamplerAddressMode GetAddressMode(RHI::AddressMode addressMode);
	/**
	 * Get the vulkan compare op from the compare op
	 * @param[in] compareOp		Compare op
	 * @return					Vulkan compare op
	 */
	VkCompareOp GetCompareOp(RHI::CompareOp compareOp);
	/**
	 * Get the best fitting vulkan border color to the border color
	 * @param[in] color		Border color
	 * @param[in] intType	If border color with ints should be used
	 * @return				Vulkan border color
	 */
	VkBorderColor GetBorderColor(const glm::vec4& color, b8 intType);

	////////////////////////////////////////////////////////////////////////////////
	// Format																	  //
	////////////////////////////////////////////////////////////////////////////////
	/**
	 * Convert a Hive pixel format to a Vulkan format
	 * @param[in] format	Pixel format
	 * @return				Vulkan format
	 * @note				If 'format.transform' is 'UNDEFINED', a default format will be returned
	 */
	VkFormat GetFormat(PixelFormat format);
	/**
	 * Convert a Vulkan format to a Hive pixel format
	 * @param[in] vkformat	Vulkan format
	 * @return				Pixel format
	 */
	PixelFormat GetPixelFormat(VkFormat vkformat);

	////////////////////////////////////////////////////////////////////////////////
	// Other																	  //
	////////////////////////////////////////////////////////////////////////////////
	/**
	 * Disable unsupported vulkan device features
	 * @param[in] availableFeatures		Available device features
	 * @param[inout] requestedFeatures	Requested device features
	 */
	void DisableUnsupportedDeviceFeatures(const VkPhysicalDeviceFeatures& availableFeatures, 
										 VkPhysicalDeviceFeatures& requestedFeatures);

	/**
	 * Get the vulkan shader stage flag from the shader type
	 * @param[in] type	Shader type
	 * @return			Vulkan shader stage flags
	 */
	VkShaderStageFlagBits GetShaderStage(RHI::ShaderType type);
	/**
	* Get the vulkan descriptor type from the descriptor type
	* @param[in] type	Descriptor type
	* @return			Vulkan descriptor type
	*/
	VkDescriptorType GetDescriptorType(RHI::DescriptorSetBindingType type);
	/**
	 * Get the vulkan index type from the index type
	 * @param[in] type	Index type
	 * @return			Vulkan index type
	 */
	VkIndexType GetIndexType(RHI::IndexType type);

	////////////////////////////////////////////////////////////////////////////////
	// Look up tables															  //
	////////////////////////////////////////////////////////////////////////////////
	// ReSharper disable CppNonInlineVariableDefinitionInHeaderFile
	namespace Tables {
		
		// Vulkan results
		static const char* g_VulkanResult[VK_INCOMPLETE + 1] = {
			"VK_SUCCES",
			"VK_NOT_READY",
			"VK_TIMEOUT",
			"VK_EVENT_SET",
			"VK_EVENT_RESET",
			"VK_INCOMPLETE",
		};

		// Vulkan errors
		static const char* g_VulkanErrors[-VK_ERROR_FRAGMENTED_POOL + 1] = {
			"VK_ERROR_OUT_OF_HOST_MEMORY",
			"VK_ERROR_OUT_OF_DEVICE_MEMORY",
			"VK_ERROR_INITIALIZATION_FAILED",
			"VK_ERROR_DEVICE_LOST",
			"VK_ERROR_MEMORY_MAP_FAILED",
			"VK_ERROR_LAYER_NOT_PRESENT",
			"VK_ERROR_EXTENSION_NOT_PRESENT",
			"VK_ERROR_FEATURE_NOT_PRESENT",
			"VK_ERROR_INCOMPATIBLE_DRIVER",
			"VK_ERROR_TOO_MANY_OBJECTS",
			"VK_ERROR_FORMAT_NOT_SUPPORTED",
			"VK_ERROR_FRAGMENTED_POOL",
		};
		
		// Debug object type
		static const char* g_debugObjectTypeName[VK_DEBUG_REPORT_OBJECT_TYPE_VALIDATION_CACHE_EXT_EXT + 1] =
		{
			"[Unkown]",
			"[Instance]",
			"[Physical device]",
			"[Device]",
			"[Queue]",
			"[Semaphore]",
			"[Command buffer]",
			"[Fence]",
			"[Device memory]",
			"[Buffer]",
			"[Image]",
			"[Event]",
			"[Query pool]",
			"[Buffer view]",
			"[Image view]",
			"[Shader Module]",
			"[Pipeline cache]",
			"[Pipeline layout]",
			"[Render pass]",
			"[Pipeline]",
			"[Descriptor set layout]",
			"[Sampler]",
			"[Descriptor pool]",
			"[Descriptor]",
			"[Framebuffer]",
			"[Command pool]",
			"[Surface KHR]",
			"[Swapchain KHR]",
			"[Debug report EXT]",
			"[Display KHR]",
			"[Display mode KHR]",
			"[Object table NVX]",
			"[Indirect commands layout NVX]",
			"[Validation cache NVX]",
		};

		// GPU types
		static RHI::GpuType g_GpuTypes[u8(RHI::GpuType::Count)] = {
			RHI::GpuType::Unknown,
			RHI::GpuType::Integrated,
			RHI::GpuType::Discrete,
			RHI::GpuType::Software,
			RHI::GpuType::CPU,
		};

		// Load ops
		static VkAttachmentLoadOp g_LoadOps[u8(RHI::LoadOp::Count)] = {
			VK_ATTACHMENT_LOAD_OP_LOAD,
			VK_ATTACHMENT_LOAD_OP_CLEAR,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		};

		// Store ops
		static VkAttachmentStoreOp g_StoreOps[u8(RHI::StoreOp::Count)] = {
			VK_ATTACHMENT_STORE_OP_STORE,
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
		};

		// Subpass attachment layouts
		static VkImageLayout g_SubpassAttachmentLayouts[u8(RHI::RenderTargetType::MultisampleResolve) + 1] = {
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		};

		// Primitive topology
		static VkPrimitiveTopology g_primitiveTopology[u8(RHI::PrimitiveTopology::Count)] = {
			VK_PRIMITIVE_TOPOLOGY_MAX_ENUM,
			VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
			VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
			VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
			VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
			VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,
			VK_PRIMITIVE_TOPOLOGY_PATCH_LIST,
		};

		// Polygon mode
		static VkPolygonMode g_PolygonMode[u8(RHI::FillMode::Count)] = {
			VK_POLYGON_MODE_FILL,
			VK_POLYGON_MODE_LINE,
			VK_POLYGON_MODE_POINT,
		};

		// Cull mode
		static VkCullModeFlagBits g_CullMode[u8(RHI::CullMode::Count)] = {
			VK_CULL_MODE_NONE,
			VK_CULL_MODE_FRONT_BIT,
			VK_CULL_MODE_BACK_BIT,
			VK_CULL_MODE_FRONT_AND_BACK,
		};

		// Front face
		static VkFrontFace g_FrontFace[u8(RHI::FrontFace::Count)] = {
			VK_FRONT_FACE_COUNTER_CLOCKWISE,
			VK_FRONT_FACE_CLOCKWISE,
		};

		// Sample count
		static VkSampleCountFlagBits g_SampleCount[u8(RHI::SampleCount::Count)] = {
			VK_SAMPLE_COUNT_1_BIT,
			VK_SAMPLE_COUNT_2_BIT,
			VK_SAMPLE_COUNT_4_BIT,
			VK_SAMPLE_COUNT_8_BIT,
			VK_SAMPLE_COUNT_16_BIT,
		};

		// Sample count
		static VkColorComponentFlags g_ColorMask[u8(RHI::ColorComponentMask::Max) + 1] = {
			0,
			VK_COLOR_COMPONENT_R_BIT,
			VK_COLOR_COMPONENT_G_BIT,
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT,
			VK_COLOR_COMPONENT_B_BIT,
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_B_BIT,
			VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT,
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT,
			VK_COLOR_COMPONENT_A_BIT,
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_A_BIT,
			VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_A_BIT,
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_A_BIT,
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
			VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		};

		// Blend op
		static VkBlendOp g_BlendOp[u8(RHI::BlendOp::Count)] = {
			VK_BLEND_OP_ADD,
			VK_BLEND_OP_SUBTRACT,
			VK_BLEND_OP_REVERSE_SUBTRACT,
			VK_BLEND_OP_MIN,
			VK_BLEND_OP_MAX,
		};

		// Blend factor
		static VkBlendFactor g_BlendFactor[u8(RHI::BlendFactor::Count)] = {
			VK_BLEND_FACTOR_ZERO,
			VK_BLEND_FACTOR_ONE,
			VK_BLEND_FACTOR_SRC_COLOR,
			VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
			VK_BLEND_FACTOR_DST_COLOR,
			VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
			VK_BLEND_FACTOR_SRC_ALPHA,
			VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			VK_BLEND_FACTOR_DST_ALPHA,
			VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
			VK_BLEND_FACTOR_CONSTANT_COLOR,
			VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
			VK_BLEND_FACTOR_CONSTANT_ALPHA,
			VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,
			VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,
			VK_BLEND_FACTOR_SRC1_COLOR,
			VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,
		};

		// Logic op
		static VkLogicOp g_LogicOp[u8(RHI::LogicOp::Count)] = {
			VK_LOGIC_OP_NO_OP,
			VK_LOGIC_OP_COPY,
			VK_LOGIC_OP_COPY_INVERTED,
			VK_LOGIC_OP_CLEAR,
			VK_LOGIC_OP_SET,
			VK_LOGIC_OP_INVERT,
			VK_LOGIC_OP_AND,
			VK_LOGIC_OP_NAND,
			VK_LOGIC_OP_AND_REVERSE,
			VK_LOGIC_OP_AND_INVERTED,
			VK_LOGIC_OP_OR,
			VK_LOGIC_OP_NOR,
			VK_LOGIC_OP_OR_REVERSE,
			VK_LOGIC_OP_OR_INVERTED,
			VK_LOGIC_OP_XOR,
			VK_LOGIC_OP_EQUIVALENT,
		};

		// Logic op
		static VkPipelineBindPoint g_PipelineBindPoint[u8(RHI::PipelineType::Count)] = {
			VK_PIPELINE_BIND_POINT_MAX_ENUM,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			VK_PIPELINE_BIND_POINT_COMPUTE,
		};

		// Input element type
		static VkFormat g_inputElementTypeToFormat[u8(RHI::InputElementType::Count)] = {
			VK_FORMAT_UNDEFINED,
			VK_FORMAT_R32_SFLOAT,
			VK_FORMAT_R32G32_SFLOAT,
			VK_FORMAT_R32G32B32_SFLOAT,
			VK_FORMAT_R32G32B32A32_SFLOAT,
			VK_FORMAT_R32_SINT,
			VK_FORMAT_R32G32_SINT,
			VK_FORMAT_R32G32B32_SINT,
			VK_FORMAT_R32G32B32A32_SINT,
			VK_FORMAT_R32_UINT,
			VK_FORMAT_R32G32_UINT,
			VK_FORMAT_R32G32B32_UINT,
			VK_FORMAT_R32G32B32A32_UINT,
			VK_FORMAT_R16_SINT,
			VK_FORMAT_R16G16_SINT,
			VK_FORMAT_R16G16B16_SINT,
			VK_FORMAT_R16G16B16A16_SINT,
			VK_FORMAT_R16_UINT,
			VK_FORMAT_R16G16_UINT,
			VK_FORMAT_R16G16B16_UINT,
			VK_FORMAT_R16G16B16A16_UINT,
		};

		// Shader stage
		static VkDescriptorType g_DescriptorTypes[u8(RHI::DescriptorSetBindingType::Count)] = {

			VK_DESCRIPTOR_TYPE_MAX_ENUM,
			VK_DESCRIPTOR_TYPE_SAMPLER,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
			VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
			VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
			VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
		};

		static VkImageType g_ImageTypes[u8(RHI::TextureType::Count)] = {
			VK_IMAGE_TYPE_MAX_ENUM,
			VK_IMAGE_TYPE_1D,
			VK_IMAGE_TYPE_1D,
			VK_IMAGE_TYPE_2D,
			VK_IMAGE_TYPE_2D,
			VK_IMAGE_TYPE_3D,
			VK_IMAGE_TYPE_2D,
			VK_IMAGE_TYPE_2D,
		};

		static VkImageViewType g_ImageViewTypes[u8(RHI::TextureType::Count)] = {
			VK_IMAGE_VIEW_TYPE_MAX_ENUM,
			VK_IMAGE_VIEW_TYPE_1D,
			VK_IMAGE_VIEW_TYPE_1D_ARRAY,
			VK_IMAGE_VIEW_TYPE_2D,
			VK_IMAGE_VIEW_TYPE_2D_ARRAY,
			VK_IMAGE_VIEW_TYPE_3D,
			VK_IMAGE_VIEW_TYPE_CUBE,
			VK_IMAGE_VIEW_TYPE_CUBE_ARRAY,
		};

		static VkImageLayout g_ImageLayouts[u8(RHI::TextureLayout::Count)] = {
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
			VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		};

		static RHI::TextureLayout g_TextureLayouts[VK_IMAGE_LAYOUT_PREINITIALIZED] = {
			RHI::TextureLayout::Unknown,
			RHI::TextureLayout::General,
			RHI::TextureLayout::ColorAttachment,
			RHI::TextureLayout::DepthStencil,
			RHI::TextureLayout::DepthStencilReadOnly,
			RHI::TextureLayout::ShaderReadOnly,
			RHI::TextureLayout::TransferSrc,
			RHI::TextureLayout::TransferDst,
		};

		// Not sure if correct
		static VkAccessFlags g_ImageTransitionAccessFlags[VK_IMAGE_LAYOUT_PREINITIALIZED] = {
			0,
			0,
			VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
			VK_ACCESS_SHADER_READ_BIT,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT,
		};

		static VkFilter g_Filters[u8(RHI::FilterMode::Count)] = {
			VK_FILTER_NEAREST,
			VK_FILTER_LINEAR,
			VK_FILTER_LINEAR,
			VK_FILTER_CUBIC_IMG,
		};

		static VkSamplerAddressMode g_samplerAddressModes[u8(RHI::AddressMode::Count)] = {
			VK_SAMPLER_ADDRESS_MODE_REPEAT,
			VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
		};

		static VkCompareOp g_CompareOps[u8(RHI::CompareOp::Count)] = {
			VK_COMPARE_OP_NEVER,
			VK_COMPARE_OP_EQUAL,
			VK_COMPARE_OP_NOT_EQUAL,
			VK_COMPARE_OP_LESS,
			VK_COMPARE_OP_LESS_OR_EQUAL,
			VK_COMPARE_OP_GREATER,
			VK_COMPARE_OP_GREATER_OR_EQUAL,
			VK_COMPARE_OP_ALWAYS,
		};

		static VkStencilOp g_stencilOps[u8(RHI::StencilOp::Count)] = {
			VK_STENCIL_OP_KEEP,
			VK_STENCIL_OP_ZERO,
			VK_STENCIL_OP_REPLACE,
			VK_STENCIL_OP_INCREMENT_AND_CLAMP,
			VK_STENCIL_OP_DECREMENT_AND_CLAMP,
			VK_STENCIL_OP_INVERT,
			VK_STENCIL_OP_INCREMENT_AND_WRAP,
			VK_STENCIL_OP_DECREMENT_AND_WRAP,
		};

	}
	// ReSharper restore CppNonInlineVariableDefinitionInHeaderFile
} }
