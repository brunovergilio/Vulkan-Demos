#pragma once


#include "VulkanDevice.h"


struct SamplerCreateInfo : public VkSamplerCreateInfo
{
	SamplerCreateInfo(const VkFilter magFilter = VK_FILTER_LINEAR, const VkFilter minFilter = VK_FILTER_LINEAR,
		const VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		const VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT)
		: VkSamplerCreateInfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO, nullptr, 0, magFilter, minFilter, mipmapMode,
			addressMode, addressMode, addressMode, 0.0f, VK_TRUE, 1.0f, VK_FALSE,
			VK_COMPARE_OP_NEVER, 0.0f, maxLod, VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE, VK_FALSE }
	{}

	void SetAnisotropy(const VkBool32 anisotropyEnable = VK_TRUE, const float maxAnisotropy = 1.0f)
	{
		this->anisotropyEnable = anisotropyEnable;
		this->maxAnisotropy = maxAnisotropy;
	}

	void SetCompareOp(const VkBool32 compareEnable = VK_FALSE, const VkCompareOp compareOp = VK_COMPARE_OP_NEVER)
	{
		this->compareEnable = compareEnable;
		this->compareOp = compareOp;
	}
};



class VulkanSampler
{
public:
	VulkanSampler() = delete;
	VulkanSampler(const VulkanDevice & device);
	~VulkanSampler();

	VkResult Create(const VkSamplerCreateInfo & samplerCreateInfo);
	void Destroy();

	inline const VkSampler Get() const { return m_Sampler; }
	inline operator VkSampler() const { return m_Sampler; }

private:
	const VulkanDevice & m_Device;
	VkSampler m_Sampler = VK_NULL_HANDLE;
};