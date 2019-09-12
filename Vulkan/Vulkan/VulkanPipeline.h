#pragma once


#include "VulkanDevice.h"


template<class Type>
struct SpecializationInfo : public VkSpecializationInfo
{
	std::vector<VkSpecializationMapEntry> m_MapEntries;

	SpecializationInfo()
		: VkSpecializationInfo{ 0, nullptr, sizeof(Type), nullptr }
	{}

	SpecializationInfo(const Type & data)
		: VkSpecializationInfo{ 0, nullptr, sizeof(Type), &data }
	{}

	void AddMapEntry(uint32_t constantID, const uint32_t offset, const size_t size)
	{
		m_MapEntries.push_back(VkSpecializationMapEntry{ constantID, offset, size });

		mapEntryCount = static_cast<uint32_t>(m_MapEntries.size());
		pMapEntries = m_MapEntries.data();
	}

	void ClearMapEntries()
	{
		m_MapEntries.clear();

		mapEntryCount = static_cast<uint32_t>(m_MapEntries.size());
		pMapEntries = nullptr;
	}

	void SetData(const Type & data)
	{
		pData = &data;
	}
};


struct GraphicsPipelineCreateInfo : VkGraphicsPipelineCreateInfo
{
	std::vector<VkPipelineShaderStageCreateInfo> m_Stages;

	VkPipelineVertexInputStateCreateInfo m_VertexInputState;
	std::vector<VkVertexInputBindingDescription> m_VertexBindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> m_VertexAttributeDescriptions;

	VkPipelineInputAssemblyStateCreateInfo m_InputAssemblyState;

	VkPipelineTessellationStateCreateInfo m_TessellationState;

	VkPipelineViewportStateCreateInfo m_ViewportState;
	std::vector<VkViewport> m_Viewports;
	std::vector<VkRect2D> m_Scissors;

	VkPipelineRasterizationStateCreateInfo m_RasterizationState;

	VkPipelineMultisampleStateCreateInfo m_MultisampleState;

	VkPipelineDepthStencilStateCreateInfo m_DepthStencilState;

	VkPipelineColorBlendStateCreateInfo m_ColorBlendState;
	std::vector<VkPipelineColorBlendAttachmentState> m_Attachments;

	VkPipelineDynamicStateCreateInfo m_DynamicState;
	std::vector<VkDynamicState> m_DynamicStates;

	GraphicsPipelineCreateInfo()
		: VkGraphicsPipelineCreateInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, nullptr, 0, 0, nullptr,
			&m_VertexInputState, &m_InputAssemblyState, nullptr, &m_ViewportState, &m_RasterizationState, &m_MultisampleState,
			&m_DepthStencilState, &m_ColorBlendState, &m_DynamicState, VK_NULL_HANDLE, VK_NULL_HANDLE, 0, 0, 0 }
		, m_VertexInputState(VkPipelineVertexInputStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			nullptr, 0, 0, nullptr, 0, nullptr })
		, m_InputAssemblyState(VkPipelineInputAssemblyStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, 
			nullptr, 0, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE })
		, m_TessellationState(VkPipelineTessellationStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO, nullptr,
			0, 0 })
		, m_ViewportState(VkPipelineViewportStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, nullptr, 0,
			1, nullptr, 1, nullptr })
		, m_RasterizationState(VkPipelineRasterizationStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			nullptr, 0, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE, VK_FALSE,
			0.0f, 0.0f, 0.0f, 1.0f })
		, m_MultisampleState(VkPipelineMultisampleStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, nullptr, 0,
			VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 0.0f, nullptr, VK_FALSE, VK_FALSE})
		, m_DepthStencilState(VkPipelineDepthStencilStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, nullptr,
			0, VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL, VK_FALSE, VK_FALSE, {}, {}, 0.0f, 0.0f })
		, m_ColorBlendState(VkPipelineColorBlendStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, nullptr, 0,
			VK_FALSE, VK_LOGIC_OP_CLEAR, 0, nullptr, { 0.0f, 0.0f, 0.0f, 0.0f} })
		, m_DynamicState(VkPipelineDynamicStateCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, nullptr, 0, 0, nullptr })
	{
	}

	inline void AddShaderStage(const VkShaderStageFlagBits stage, const VkShaderModule module,
			const char * const pName = "main", const VkSpecializationInfo * const pSpecializationInfo = nullptr)
	{
		m_Stages.push_back({ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, stage, module, pName, pSpecializationInfo });

		stageCount = static_cast<uint32_t>(m_Stages.size());
		pStages = m_Stages.data();
	}

	inline void ClearShaderStages()
	{
		m_Stages.clear();

		stageCount = 0;
		pStages = nullptr;
	}

	inline void AddVertexBinding(const uint32_t binding, const uint32_t stride, const VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX)
	{
		m_VertexBindingDescriptions.push_back({ binding,stride, inputRate });

		m_VertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(m_VertexBindingDescriptions.size());
		m_VertexInputState.pVertexBindingDescriptions = m_VertexBindingDescriptions.data();
	}

	inline void ClearVertexBindings()
	{
		m_VertexBindingDescriptions.clear();

		m_VertexInputState.vertexBindingDescriptionCount = 0;
		m_VertexInputState.pVertexBindingDescriptions = nullptr;
	}

	inline void AddVertexAttribute(const uint32_t location, const uint32_t binding, const VkFormat format, const uint32_t offset)
	{
		m_VertexAttributeDescriptions.push_back({ location, binding, format, offset });

		m_VertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_VertexAttributeDescriptions.size());
		m_VertexInputState.pVertexAttributeDescriptions = m_VertexAttributeDescriptions.data();
	}

	inline void ClearVertexAttributes()
	{
		m_VertexAttributeDescriptions.clear();

		m_VertexInputState.vertexAttributeDescriptionCount = 0;
		m_VertexInputState.pVertexAttributeDescriptions = nullptr;
	}

	inline void SetInputAssembly(const VkPrimitiveTopology topology,
		const VkBool32 primitiveRestartEnable = VK_FALSE)
	{
		m_InputAssemblyState.topology = topology;
		m_InputAssemblyState.primitiveRestartEnable = primitiveRestartEnable;
	}

	inline void SetTesselation(const VkBool32 use, const uint32_t patchControlPoints = 0)
	{
		m_TessellationState.patchControlPoints = patchControlPoints;
		pTessellationState = use ? &m_TessellationState : nullptr;
	}

	inline void AddViewport(const float width, const float height, const float minDepth, const float maxDepth,
		const float x = 0.0f, const float y = 0.0f)
	{
		m_Viewports.push_back({ x, y, width, height, minDepth, maxDepth });

		m_ViewportState.viewportCount = static_cast<uint32_t>(m_Viewports.size());
		m_ViewportState.pViewports = m_Viewports.data();
	}

	inline void ClearViewports()
	{
		m_Viewports.clear();

		m_ViewportState.viewportCount = 0;
		m_ViewportState.pViewports = nullptr;
	}

	inline void SetDynamicViewportCount(const uint32_t viewportCount)
	{
		m_Viewports.clear();
		m_ViewportState.viewportCount = viewportCount;
		m_ViewportState.pViewports = nullptr;
	}

	inline void AddScissor(const uint32_t width, const uint32_t height, const int32_t x = 0, const int32_t y = 0)
	{
		m_Scissors.push_back({ x, y, width, height });

		m_ViewportState.scissorCount = static_cast<uint32_t>(m_Scissors.size());
		m_ViewportState.pScissors = m_Scissors.data();
	}

	inline void ClearScissors()
	{
		m_Scissors.clear();

		m_ViewportState.scissorCount = 0;
		m_ViewportState.pScissors = nullptr;
	}

	inline void SetDynamicScissorCount(const uint32_t scissorCount)
	{
		m_Scissors.clear();
		m_ViewportState.scissorCount = scissorCount;
		m_ViewportState.pScissors = nullptr;
	}

	inline void SetRasterizer(const VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT, const VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE,
		const VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
		const float lineWidth = 1.0f, const VkBool32 rasterizerDiscardEnable = VK_FALSE)
	{
		m_RasterizationState.rasterizerDiscardEnable = rasterizerDiscardEnable;
		m_RasterizationState.polygonMode = polygonMode;
		m_RasterizationState.cullMode = cullMode;
		m_RasterizationState.frontFace = frontFace;
		m_RasterizationState.lineWidth = lineWidth;
	}

	inline void SetDepthClamp(const VkBool32 depthClampEnable = VK_FALSE, const VkBool32 depthBiasEnable = VK_FALSE,
		const float depthBiasConstantFactor = 0.0f, const float depthBiasClamp = 0.0f, const float depthBiasSlopeFactor = 0.0f)
	{
		m_RasterizationState.depthClampEnable = depthClampEnable;
		m_RasterizationState.depthBiasEnable = depthBiasEnable;
		m_RasterizationState.depthBiasConstantFactor = depthBiasConstantFactor;
		m_RasterizationState.depthBiasClamp = depthBiasClamp;
		m_RasterizationState.depthBiasSlopeFactor = depthBiasSlopeFactor;
	}

	inline void SetMultisampling(const VkSampleCountFlagBits rasterizationSamples, const VkBool32 sampleShadingEnable,
		const float minSampleShading, const VkBool32 alphaToCoverageEnable,
		const VkBool32 alphaToOneEnable, const VkSampleMask * const pSampleMask)
	{
		m_MultisampleState.rasterizationSamples = rasterizationSamples;
		m_MultisampleState.sampleShadingEnable = sampleShadingEnable;
		m_MultisampleState.minSampleShading = minSampleShading;
		m_MultisampleState.alphaToCoverageEnable = alphaToCoverageEnable;
		m_MultisampleState.alphaToOneEnable = alphaToOneEnable;
		m_MultisampleState.pSampleMask = pSampleMask;
	}

	inline void SetDepthTest(const VkBool32 depthTestEnable = VK_TRUE, const VkBool32 depthWriteEnable = VK_TRUE,
		const VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL)
	{
		m_DepthStencilState.depthTestEnable = depthTestEnable;
		m_DepthStencilState.depthWriteEnable = depthWriteEnable;
		m_DepthStencilState.depthCompareOp = depthCompareOp;
	}

	inline void SetDepthBoundTest(const VkBool32 depthBoundsTestEnable = VK_FALSE,
		const float minDepthBounds = 0.0f, const float maxDepthBounds = 0.0f)
	{
		m_DepthStencilState.depthBoundsTestEnable = depthBoundsTestEnable;
		m_DepthStencilState.minDepthBounds = minDepthBounds;
		m_DepthStencilState.maxDepthBounds = maxDepthBounds;
	}

	inline void SetStencilTest(const VkBool32 stencilTestEnable)
	{
		m_DepthStencilState.stencilTestEnable = stencilTestEnable;
	}

	inline void SetStencilOp(const VkStencilOp failOp = VK_STENCIL_OP_KEEP, const VkStencilOp passOp = VK_STENCIL_OP_KEEP,
		const VkStencilOp depthFailOp = VK_STENCIL_OP_KEEP)
	{
		SetStencilOpFront(failOp, passOp, depthFailOp);
		SetStencilOpBack(failOp, passOp, depthFailOp);
	}

	inline void SetStencilFunc(const VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS, const uint32_t reference = 0,
		const uint32_t compareMask = 0xFF, const uint32_t writeMask = 0xFF)
	{
		SetStencilFuncFront(compareOp, reference, compareMask, writeMask);
		SetStencilFuncBack(compareOp, reference, compareMask, writeMask);
	}

	inline void SetStencilOpFront(const VkStencilOp failOp = VK_STENCIL_OP_KEEP, const VkStencilOp passOp = VK_STENCIL_OP_KEEP,
		const VkStencilOp depthFailOp = VK_STENCIL_OP_KEEP)
	{
		m_DepthStencilState.front.failOp = failOp;
		m_DepthStencilState.front.passOp = passOp;
		m_DepthStencilState.front.depthFailOp = depthFailOp;
	}

	inline void SetStencilFuncFront(const VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS, const uint32_t reference = 0,
		const uint32_t compareMask = 0xFF, const uint32_t writeMask = 0xFF)
	{
		m_DepthStencilState.front.compareOp = compareOp;
		m_DepthStencilState.front.compareMask = compareMask;
		m_DepthStencilState.front.writeMask = writeMask;
		m_DepthStencilState.front.reference = reference;
	}

	inline void SetStencilOpBack(const VkStencilOp failOp = VK_STENCIL_OP_KEEP, const VkStencilOp passOp = VK_STENCIL_OP_KEEP,
		const VkStencilOp depthFailOp = VK_STENCIL_OP_KEEP)
	{
		m_DepthStencilState.back.failOp = failOp;
		m_DepthStencilState.back.passOp = passOp;
		m_DepthStencilState.back.depthFailOp = depthFailOp;
	}

	inline void SetStencilFuncBack(const VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS, const uint32_t reference = 0,
		const uint32_t compareMask = 0xFF, const uint32_t writeMask = 0xFF)
	{
		m_DepthStencilState.back.compareOp = compareOp;
		m_DepthStencilState.back.compareMask = compareMask;
		m_DepthStencilState.back.writeMask = writeMask;
		m_DepthStencilState.back.reference = reference;
	}

	inline void AddBlendAttachment(const VkColorComponentFlags colorWriteMask = 0xf, const VkBool32 blendEnable = VK_FALSE,
		const VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ZERO, const VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		const VkBlendOp colorBlendOp = VK_BLEND_OP_ADD,	const VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		const VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, const VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD)
	{
		m_Attachments.push_back({ blendEnable, srcColorBlendFactor, dstColorBlendFactor, colorBlendOp,
			srcAlphaBlendFactor, dstAlphaBlendFactor, alphaBlendOp, colorWriteMask });

		m_ColorBlendState.attachmentCount = static_cast<uint32_t>(m_Attachments.size());
		m_ColorBlendState.pAttachments = m_Attachments.data();
	}

	inline void ClearBlendAttachments()
	{
		m_Attachments.clear();

		m_ColorBlendState.attachmentCount = 0;
		m_ColorBlendState.pAttachments = nullptr;
	}

	inline void AddDynamicState(const VkDynamicState state)
	{
		m_DynamicStates.push_back(state);

		m_DynamicState.dynamicStateCount = static_cast<uint32_t>(m_DynamicStates.size());
		m_DynamicState.pDynamicStates = m_DynamicStates.data();
	}

	inline void ClearDynamicStates()
	{
		m_DynamicStates.clear();

		m_DynamicState.dynamicStateCount = 0;
		m_DynamicState.pDynamicStates = nullptr;
	}

	inline void SetRenderInfo(const VkPipelineLayout layout, const VkRenderPass renderPass, const uint32_t subpass = 0,
		const VkPipelineCreateFlags flags = 0, const VkPipeline basePipelineHandle = VK_NULL_HANDLE)
	{
		this->flags = flags;
		this->layout = layout;
		this->renderPass = renderPass;
		this->subpass = subpass;
		this->basePipelineHandle = basePipelineHandle;
		this->basePipelineIndex = (VK_PIPELINE_CREATE_DERIVATIVE_BIT & flags) > 0 ? -1 : 0;
	}
};


class VulkanPipeline
{
public:
	explicit VulkanPipeline(const VulkanDevice & device);
	~VulkanPipeline();

	VkResult Create(const VkGraphicsPipelineCreateInfo & createInfo, const VkPipelineCache pipelineCache);
	void Destroy();

	inline const VkPipeline Get() const { return m_Pipeline; }
	inline operator VkPipeline() const { return m_Pipeline; }

private:
	VkPipeline m_Pipeline = VK_NULL_HANDLE;
	VkDevice m_Device = VK_NULL_HANDLE;
};