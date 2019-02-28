#pragma once
#include "../General/TypesAndMacros.h"
#include <glm/vec3.hpp>

namespace RHI {

	enum class RHIValidationLevel : u8
	{
		None,			/**< No validation checks */
		Error,			/**< Only check errors */
		Warning,		/**< Check errors and warnings */
		Performance,	/**< Check errors, warnings and performance warnings */
		Info,			/**< Check errors, warnings and performance warnings + show info */
		DebugInfo		/**< Check errors, warnings and performance warnings + detailed info */
	};

	enum class QueueType : u8
	{
		Unknown = 0x00,		/**< Unkown queue */
		Graphics = 0x01,	/**< Graphics queue (also handles vulkan sparse and present queue family) */
		Compute = 0x02,		 /**< Compute queue */
		Transfer = 0x04,	/**< Transfer/copy queue */
	};
	ENABLE_ENUM_FLAG_OPERATORS(QueueType);

	enum class QueuePriority : u8
	{
		Low,	/**< Low queue priority */
		High	/**< High queue priority */
	};

	struct QueueInfo
	{
		QueueType type;
		QueuePriority priority;
		u8 count;
	};

	enum class VSyncMode : u8
	{
		Off,	/**< No v-sync */
		VSync,	/**< V-sync */
		Tripple	/**< Tripple buffered v-sync */
	};

	enum class BufferType : u8
	{
		Default = 0x00,	/**< Basic buffer */
		Vertex = 0x01,	/**< Vertex buffer */
		Index = 0x02,	/**< Index buffer */
		Uniform = 0x04,	/**< Uniform buffer */
		UniformTexel = 0x08,	/**< Uniform texel buffer */
		Storage = 0x10,	/**< Storage buffer */
		StorageTexel = 0x20,	/**< Storage texel buffer */
		Indirect = 0x40,	/**< Buffer for indirect draw/dispatch */
		Staging = 0x80,	/**< Staging buffer */
	};
	ENABLE_ENUM_FLAG_OPERATORS(BufferType);

	enum class BufferFlags : u8
	{
		None = 0x00,	/**< No flags */
		Static = 0x01, /**< Static buffer, data won't be changed often (incompatible with dynamic) */
		Dynamic = 0x02, /**< Dynamic buffer, data will be changed often (incompatible with static) */
		NoWrite = 0x04,	/**< Disable buffer write */
		NoRead = 0x08,	/**< Disable buffer read */
	};
	ENABLE_ENUM_FLAG_OPERATORS(BufferFlags);

	enum class BufferWriteFlags
	{
		None,		/**< No flags */
		Discard		/**< Discard the parts of the buffer that is not being written to */
	};

	enum class IndexType : u8
	{
		Unknown,				/**< Unknown */
		UShort = sizeof(u16),	/**< Unsigned short */
		UInt = sizeof(u32)		/**< Unsigned int */
	};

	enum class TextureFlags : u8
	{
		None = 0x00,		/**< No flags */
		Dynamic = 0x01,		/**< Dynamic texture, changed often (while this may be benifitial for copying data, the user needs to check if this doesn't impact their performace to much, since this flag does not guarantee an optimal layout on the GPU) */
		Static = 0x02,		/**< Static texture, almost never changed */

		NoSampling = 0x04,		/**< Image cannot be used as a sampled image (Fragment) */
		Storage = 0x08,		/**< Image can be used as a storage image (Compute) */
		InputAttachment = 0x10,		/**< Image can be used as a input attachment (Compute) */

		RenderTargetable = 0x20,	/**< [INTERNAL] If the texture can be used as a the underlying texture for a rendertarget */
		Color = 0x40,				/**< [INTERNAL] If the texture can be used as a the underlying texture for a color attachment */
		DepthStencil = 0x80,		/**< [INTERNAL] If the texture can be used as a the underlying texture for a depth stencil attachment */
	};
	ENABLE_ENUM_FLAG_OPERATORS(TextureFlags);

	enum class RenderTargetType : u8
	{
		None,				/**< No type */
		Color,				/**< Color */
		Presentable,		/**< Presentable (color) */
		DepthStencil,		/**< Depth stencil */
		MultisampleResolve,	/**< Multisample resolve */

		Input,				/**< [RENDERPASS] Use as input attachment */
		Preserve,			/**< [RENDERPASS] Explicitly preserve content */
		Count,				/**< Number of types */
	};

	enum class TextureType : u8
	{
		None,			/**< No type (invalid) */
		Tex1D,			/**< 1D texture */
		Tex1DArray,		/**< 1D texture array */
		Tex2D,			/**< 2D texture (allowed for render target) */
		Tex2DArray,		/**< 2D texture array */
		Tex3D,			/**< 3D texture (allowed for render target) */
		Cubemap,		/**< Cubemap (allowed for render target) */
		CubemapArray,	/**< Cubemap array */
		Count
	};

	enum class SampleCount : u8
	{
		Sample1,
		Sample2,
		Sample4,
		Sample8,
		Sample16,
		Count
	};

	enum class ShaderType : u8
	{
		None = 0x00,
		Vertex = 0x01,
		Geometry = 0x02,
		Hull = 0x04,
		Domain = 0x08,
		Fragment = 0x10,
		Compute = 0x20,
	};
	ENABLE_ENUM_FLAG_OPERATORS(ShaderType);

	enum class ShaderLanguage : u8
	{
		None,		/**< No language */
		GLSL,		/**< GLSL */
		Spirv,		/**< Spir-v (compiled GLSL) */
		HLSL,		/**< HLSL */
		BinHLSL,	/**< Binary HLSL (compiled HLSL) */
	};

	enum class FillMode : u8
	{
		Solid,
		Wireframe,
		Points,
		Count,
	};

	enum class PrimitiveTopology : u8
	{
		None,
		Point,
		Line,
		LineStrip,
		LineAdj,
		LineStripAdj,
		Triangle,
		TraingleStrip,
		TriangleAdj,
		TraingleStripAdj,
		PatchList,
		Count
	};

	enum class CullMode : u8
	{
		None,
		Front,
		Back,
		FrontBack,
		Count
	};

	enum class FrontFace : u8
	{
		CounterClockWise,
		ClockWise,
		Count
	};

	enum class BlendFactor : u8
	{
		Zero,
		One,
		SrcColor,
		OneMinusSrcColor,
		DstColor,
		OneMinusDstColor,
		SrcAlpha,
		OneMinusSrcAlpha,
		DstAlpha,
		OneMinusDstAlpha,
		ConstColor,
		OneMinusConstColor,
		ConstAlpha,
		OneMinusConstAlpha,
		SrcAlphaSaturate,
		Src1Color,
		OneMinusSrc1Color,
		Count,
	};

	enum class BlendOp : u8
	{
		Add,
		Subtract,
		ReverseSubtract,
		Min,
		Max,
		Count
	};

	enum class ColorComponentMask : u8
	{
		None = 0x00,
		R = 0x01,
		G = 0x02,
		B = 0x04,
		A = 0x08,
		Max = 0x0F
	};
	ENABLE_ENUM_FLAG_OPERATORS(ColorComponentMask);

	enum class LogicOp : u8
	{
		None,			/**< d */
		Copy,			/**< s */
		CopyInverted,	/**< !s */
		Clear,			/**< Set all to 0 */
		Set,			/**< Set all to 1 */
		Invert,			/**< !s */
		And,			/**< s & d */
		Nand,			/**< !(s & d) */
		AndReverse,		/**< s & !d */
		AndInverted,	/**< !s & d */
		Or,				/**< s | d */
		Nor,			/**< !(s | d) */
		OrReverse,		/**< s | !d */
		OrInverted,		/**< !s | d */
		Xor,			/**< s ^ d */
		Equivalent,		/**< !(s ^ d) */
		Count,
	};

	enum class DynamicState
	{
		None = 0x0000,
		Viewport = 0x0001,
		Scissor = 0x0002,
		LineWidth = 0x0004,
		DepthBias = 0x0010,
		BlendConstants = 0x0020,
		DepthBounds = 0x0040,
		StencilCompareMask = 0x0080,
		StencilWriteMask = 0x0100,
		StencilReference = 0x0200
	};
	ENABLE_ENUM_FLAG_OPERATORS(DynamicState);

	enum class PipelineType : u8
	{
		None,
		Graphics,
		Compute,
		Count,
	};

	enum class LoadOp : u8
	{
		Load,		/**< Preserve previous content */
		Clear,		/**< Clear previous content */
		DontCare,	/**< Don't care about previous content (undefined) */
		Count,		/**< Number of loadOps */
	};

	enum class StoreOp : u8
	{
		Store,		/**< Store the content */
		DontCare,	/**< Don't care about the content (undefined) */
		Count,		/**< Number of storeOps */
	};

	enum class FenceStatus : u8
	{
		Unsignaled,
		Submitted,
		Signaled
	};

	enum class CommandListState : u8
	{
		Reset,
		Recording,
		Recorded,
		Submited,
		Finished
	};

	enum class PipelineStage
	{
		None = 0x0000'0000,
		TopOfPipe = 0x0000'0001,
		DrawIndirect = 0x0000'0002,
		VertexInput = 0x0000'0004,
		VertexShader = 0x0000'0008,
		HullShader = 0x0000'0010,
		DomainShader = 0x0000'0020,
		GeometryShader = 0x0000'0040,
		FragmentShader = 0x0000'0080,
		EarlyFragmentTest = 0x0000'0100,
		LastFragmentTest = 0x0000'0200,
		ColorAttachmentOutput = 0x0000'0400,
		ComputeShader = 0x0000'0800,
		Transfer = 0x0000'1000,
		BottomOfPipe = 0x0000'2000,
		Host = 0x0000'4000,
		AllGraphics = 0x0000'8000,
		AllCommands = 0x0001'0000,
	};
	ENABLE_ENUM_FLAG_OPERATORS(PipelineStage);

	struct BufferCopyRegion
	{
		u64 srcOffset;
		u64 dstOffset;
		u64 size;
	};

	enum class DescriptorSetBindingType : u8
	{
		None,
		SampledImage,
		CombinedImageSampler,
		Sampler,
		StorageImage,
		UniformTexelBuffer,
		StorageTexelBuffer,
		Uniform,
		Storage,
		DynamicUniformBuffer,
		DynamicStorageBuffer,
		InputAttachment,
		Count,
	};

	enum class FilterMode : u8
	{
		Nearest,
		Linear,
		Trilinear,
		Cubemap,
		Count
	};

	enum class AddressMode : u8
	{
		Repeat,
		MirrorRepeat,
		ClampToEdge,
		ClampToBorder,
		Count
	};

	enum class CompareOp : u8
	{
		Never,
		Equal,
		NotEqual,
		Less,
		LessEqual,
		Greater,
		GreaterEqual,
		Always,
		Count
	};

	enum class TextureWriteFlags
	{
		None,		/**< No flags */
		Discard		/**< Discard the parts of the texture that is not being written to (black) */
	};


	struct TextureRegion
	{
		u8 mipLevel;
		u32 baseArrayLayer;
		u32 layerCount;

		glm::uvec3 offset;
		glm::uvec3 extent;
	};

	struct TextureCopyRegion
	{
		glm::uvec3 srcOffset;
		u8 srcMipLevel;
		u32 srcBaseArrayLayer;

		glm::uvec3 dstOffset;
		u8 dstMipLevel;
		u32 dstBaseArrayLayer;

		glm::uvec3 extent;
		u32 layerCount;
	};

	struct TextureBufferCopyRegion
	{
		u64 bufferOffset;
		u32 bufferRowLength;
		u32 bufferImageHeight;

		u8 mipLevel;
		u32 baseArrayLayer;
		u32 layerCount;

		glm::uvec3 texOffset;
		glm::uvec3 texExtent;
	};

	enum class TextureLayout
	{
		Unknown,
		General,
		ColorAttachment,
		DepthStencil,
		DepthStencilReadOnly,
		ShaderReadOnly,
		TransferSrc,
		TransferDst,
		DepthReadOnlyWithStencil,
		DepthWithStencilReadOnly,
		Present,
		Count
	};

	struct TextureLayoutTransition
	{
		TextureLayout layout;
		u32 baseArrayLayer;
		u32 layerCount;
		u8 baseMipLevel;
		u8 mipLevelCount;
	};

	enum class StencilOp : u8
	{
		Keep,		/**< Keep the current value */
		Zero,		/**< Set the current value to 0 */
		Replace,	/**< Replace the current value */
		IncClamp,	/**< Increment the current value and clamp it */
		DecClamp,	/**< Decrement the current value and clamp it */
		Invert,		/**< Invert the current value */
		IncWrap,	/**< Increment the current value and wrap it */
		DecWrap,	/**< Decrement the current value and wrap it */
		Count
	};

	struct StencilOpState
	{
		StencilOp failOp;		/**< Action on stencil test fail */
		StencilOp passOp;		/**< Action on stencil test pass */
		StencilOp depthFailOp;	/**< Action on depth test fail */
		CompareOp compareOp;	/**< Compare op for test */
		u32 compareMask;		/**< Mask for comparing */
		u32 writeMask;			/**< Mask for writing */
		u32 reference;			/**< Reference value to compare to */
	};

	
}