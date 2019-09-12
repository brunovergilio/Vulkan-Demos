#include "VulkanSemaphore.h"


VulkanSemaphore::VulkanSemaphore(const VulkanDevice & device)
	: m_Device(device)
{
}


VulkanSemaphore::~VulkanSemaphore()
{
	Destroy();
}


VkResult VulkanSemaphore::Create()
{
	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	return vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_Semaphore);
}


void VulkanSemaphore::Destroy()
{
	VK_SAFE_DESTROY(m_Device, m_Semaphore, nullptr, Semaphore);
}