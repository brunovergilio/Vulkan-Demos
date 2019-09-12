#include "VulkanShader.h"
#include <fstream>


VulkanShader::VulkanShader(const VulkanDevice & device)
	: m_Device(device)
{
}


VulkanShader::~VulkanShader()
{
	Destroy();
}


VkResult VulkanShader::Create(const char * const pFilename)
{
	std::ifstream file(pFilename, std::ios::ate | std::ios::binary | std::ios::in);
	assert(file.good() == true);

	size_t codeSize = static_cast<size_t>(file.tellg());
	file.seekg(0, std::ios::beg);

	uint8_t * pCode = new uint8_t[codeSize];
	file.read(reinterpret_cast<char *>(pCode), codeSize);

	file.close();

	VkShaderModuleCreateInfo shaderInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, nullptr, 0,
		codeSize, reinterpret_cast<const uint32_t *>(pCode) };

	VkResult result = vkCreateShaderModule(m_Device, &shaderInfo, nullptr, &m_Shader);

	delete[] pCode;

	return result;
}


void VulkanShader::Destroy()
{
	VK_SAFE_DESTROY(m_Device, m_Shader, nullptr, ShaderModule);
}