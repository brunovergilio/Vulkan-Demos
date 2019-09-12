#include "VulkanImageView.h"


VulkanImageView::VulkanImageView(const VulkanDevice & device)
	: m_Device(device)
{
}


VulkanImageView::~VulkanImageView()
{
	Destroy();
}


VkResult VulkanImageView::Create(const VkImageViewCreateInfo & imageViewCreateInfo)
{
	return vkCreateImageView(m_Device, &imageViewCreateInfo, nullptr, &m_View);
}


void VulkanImageView::Destroy()
{
	VK_SAFE_DESTROY(m_Device, m_View, nullptr, ImageView);
}