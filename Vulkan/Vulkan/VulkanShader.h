#pragma once


#include "VulkanDevice.h"


class VulkanShader
{
public:
	explicit VulkanShader(const VulkanDevice & device);
	~VulkanShader();

	VkResult Create(const char * const pFilename);
	void Destroy();

	inline const VkShaderModule Get() const { return m_Shader; }
	inline operator VkShaderModule() const { return m_Shader; }

private:
	const VulkanDevice & m_Device;
	VkShaderModule m_Shader = VK_NULL_HANDLE;
};