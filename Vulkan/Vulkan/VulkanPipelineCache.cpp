#include "VulkanPipelineCache.h"


VulkanPipelineCache::VulkanPipelineCache(const VulkanDevice & device)
	: m_Device(device)
{
}


VulkanPipelineCache::~VulkanPipelineCache()
{
	Destroy();
}


VkResult VulkanPipelineCache::Create(size_t initialDataSize, const void * const pInitialData)
{
	VkPipelineCacheCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	createInfo.initialDataSize = initialDataSize;
	createInfo.pInitialData = pInitialData;

	return vkCreatePipelineCache(m_Device, &createInfo, nullptr, &m_Cache);
}


void VulkanPipelineCache::Destroy()
{
	VK_SAFE_DESTROY(m_Device, m_Cache, nullptr, PipelineCache);
}