#pragma once


#include "VulkanUtils.h"


class VulkanCommandBuffer
{
public:
	friend class VulkanCommandPool;

	VulkanCommandBuffer();
	~VulkanCommandBuffer();

	VkResult Reset(const VkFlags flags = 0) const;

	VkResult BeginCommandBuffer(const VkCommandBufferUsageFlags flags = 0, const VkCommandBufferInheritanceInfo * const pInheritanceInfo = nullptr) const;
	VkResult EndCommandBuffer() const;

	void BeginRenderPass(const VkRenderPass renderPass, const VkFramebuffer framebuffer, const VkRect2D & renderArea,
		const uint32_t clearValueCount, const VkClearValue * const pClearValues, const VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE) const;
	void BeginRenderPass(const VkRenderPass renderPass, const VkFramebuffer framebuffer, const uint32_t width, const uint32_t height,
		const uint32_t clearValueCount, const VkClearValue * const pClearValues, const VkSubpassContents subpassContents = VK_SUBPASS_CONTENTS_INLINE) const;
	void EndRenderPass() const;


	void SetViewport(const float width, const float height, const float minDepth = 0.0f, const float maxDepth = 1.0f) const;
	void SetViewport(const uint32_t viewportCount, const VkViewport * const pViewports) const;
	void SetScissor(const uint32_t width, const uint32_t height, const int32_t x = 0, const int32_t y = 0) const;
	void SetScissor(const uint32_t scissorCount, const VkRect2D * const pScissors) const;

	void BindPipeline(const VkPipeline pipeline, const VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS) const;

	void BindDescriptorSets(const VkPipelineBindPoint pipelineBindPoint, const VkPipelineLayout layout, const uint32_t firstSet, const uint32_t descriptorSetCount,
		const VkDescriptorSet* pDescriptorSets, const uint32_t dynamicOffsetCount = 0, const uint32_t* pDynamicOffsets = nullptr) const;

	void BindVertexBuffers(const uint32_t firstBinding, const uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets) const;
	void BindIndexBuffer(const VkBuffer buffer, const VkDeviceSize offset = 0, const VkIndexType indexType = VK_INDEX_TYPE_UINT32) const;

	void Draw(const uint32_t vertexCount, const uint32_t instanceCount = 1,
		const uint32_t firstVertex = 0, const uint32_t firstInstance = 0) const;
	void DrawIndexed(const uint32_t indexCount, const uint32_t instanceCount = 1, const uint32_t firstIndex = 0,
		const int32_t vertexOffset = 0, const uint32_t firstInstance = 0) const;

	void CopyBuffer(const VkBuffer srcBuffer, const VkBuffer dstBuffer, const VkDeviceSize size) const;
	void CopyBuffer(const VkBuffer srcBuffer, const VkBuffer dstBuffer, const uint32_t regionCount, const VkBufferCopy * const pRegions = nullptr) const;

	void CopyBufferToImage(const VkBuffer srcBuffer, const VkImage dstImage, const VkImageLayout dstImageLayout,
		const uint32_t regionCount, const VkBufferImageCopy* pRegions) const;

	void PipelineBarrier(const VkPipelineStageFlags srcStageMask,
		const VkPipelineStageFlags dstStageMask,
		const VkDependencyFlags dependencyFlags,
		const uint32_t memoryBarrierCount,
		const VkMemoryBarrier * pMemoryBarriers,
		const uint32_t bufferMemoryBarrierCount,
		const VkBufferMemoryBarrier * pBufferMemoryBarriers,
		const uint32_t imageMemoryBarrierCount,
		const VkImageMemoryBarrier * pImageMemoryBarriers) const;

	void NextSubpass(const VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE) const;

	inline const VkCommandBuffer Get() const { return m_CommandBuffer; }
	inline operator VkCommandBuffer() const { return m_CommandBuffer; }

private:
	VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
};