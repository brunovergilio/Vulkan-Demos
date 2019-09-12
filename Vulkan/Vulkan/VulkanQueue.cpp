#include "VulkanQueue.h"


VulkanQueue::VulkanQueue(const VulkanDevice & device, const uint32_t queueIndex)
	: m_Index(queueIndex)
{
	vkGetDeviceQueue(device, queueIndex, 0, &m_Queue);
}


VulkanQueue::~VulkanQueue()
{
}


VkResult VulkanQueue::Submit(const VkCommandBuffer commandBuffer, const VkSemaphore waitSemaphore, const VkSemaphore signalSemaphore, const VkPipelineStageFlags waitDstStageMask, const VkFence fence)
{
	assert(m_Queue != nullptr);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = waitSemaphore ? 1 : 0;
	submitInfo.pWaitSemaphores = waitSemaphore ? &waitSemaphore : nullptr;
	submitInfo.pWaitDstStageMask = waitDstStageMask ? &waitDstStageMask : nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	submitInfo.signalSemaphoreCount = signalSemaphore ? 1 : 0;
	submitInfo.pSignalSemaphores = signalSemaphore ? &signalSemaphore : nullptr;

	return vkQueueSubmit(m_Queue, 1, &submitInfo, fence);
}


VkResult VulkanQueue::Submit(const VkSubmitInfo & submitInfo, const VkFence fence)
{
	assert(m_Queue != nullptr);

	return vkQueueSubmit(m_Queue, 1, &submitInfo, fence);
}


VkResult VulkanQueue::Submit(const uint32_t submitInfoCount, const VkSubmitInfo * const pSubmitInfos, const VkFence fence)
{
	assert(m_Queue != nullptr);

	return vkQueueSubmit(m_Queue, submitInfoCount, pSubmitInfos, fence);
}


VkResult VulkanQueue::Present(const VkSwapchainKHR pSwapchain, const uint32_t imageIndex, const VkSemaphore waitSemaphore)
{
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &pSwapchain;
	presentInfo.pImageIndices = &imageIndex;
	// Check if a wait semaphore has been specified to wait for before presenting the image
	if (waitSemaphore)
	{
		presentInfo.pWaitSemaphores = &waitSemaphore;
		presentInfo.waitSemaphoreCount = 1;
	}

	return Present(presentInfo);
}

VkResult VulkanQueue::Present(const VkPresentInfoKHR & presentInfo)
{
	return vkQueuePresentKHR(m_Queue, &presentInfo);
}


VkResult VulkanQueue::WaitIdle()
{
	return vkQueueWaitIdle(m_Queue);
}