#include "VulkanSampler.h"


VulkanSampler::VulkanSampler(const VulkanDevice & device)
	: m_Device(device)
{
}


VulkanSampler::~VulkanSampler()
{
	Destroy();
}


VkResult VulkanSampler::Create(const VkSamplerCreateInfo & samplerCreateInfo)
{
	return vkCreateSampler(m_Device, &samplerCreateInfo, nullptr, &m_Sampler);
}


void VulkanSampler::Destroy()
{
	VK_SAFE_DESTROY(m_Device, m_Sampler, nullptr, Sampler);
}