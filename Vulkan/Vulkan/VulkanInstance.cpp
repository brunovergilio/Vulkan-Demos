#include "VulkanInstance.h"



VulkanInstance::VulkanInstance()
{
}


VulkanInstance::~VulkanInstance()
{
	Destroy();
}


VkResult VulkanInstance::Create(const char * const pAppName, const VkBool32 debugSupport)
{
	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = pAppName;
	applicationInfo.pEngineName = pAppName;
	applicationInfo.apiVersion = VK_API_VERSION_1_1;


	std::vector<const char *> extensions;
	extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

	std::vector<const char *> validationLayers;
	if (debugSupport)
	{
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
	}

	VkInstanceCreateInfo instanceCreateInfo{};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &applicationInfo;

	instanceCreateInfo.ppEnabledExtensionNames = extensions.size() > 0 ? extensions.data() : nullptr;
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());

	instanceCreateInfo.ppEnabledLayerNames = validationLayers.size() > 0 ? validationLayers.data() : nullptr;
	instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());

	return vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance);
}


void VulkanInstance::Destroy()
{
	VK_SAFE_DESTROY_BASE(m_Instance, nullptr, Instance);
}


VkResult VulkanInstance::EnumerateGPUs(VulkanGPUInfoList & gpuInfoList)
{
	// ===========================================================
	// Get list of GPUs with support for Vulkan
	uint32_t physicalDeviceCount = 0;
	VkResult result = vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, nullptr);
	if (result != VK_SUCCESS)
	{
		return result;
	}

	gpuInfoList.resize(physicalDeviceCount);
	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	result = vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, physicalDevices.data());
	if (result != VK_SUCCESS)
	{
		return result;
	}

	for (size_t i = 0; i < physicalDevices.size(); ++i)
	{
		// =================================
		// Get the device
		gpuInfoList[i].m_PhysicalDevice = physicalDevices[i];

		// =================================
		// Get the properties / features / memory properties
		vkGetPhysicalDeviceProperties(gpuInfoList[i].m_PhysicalDevice, &gpuInfoList[i].m_DeviceProperties);
		vkGetPhysicalDeviceFeatures(gpuInfoList[i].m_PhysicalDevice, &gpuInfoList[i].m_DeviceFeatures);
		vkGetPhysicalDeviceMemoryProperties(gpuInfoList[i].m_PhysicalDevice, &gpuInfoList[i].m_DeviceMemoryProperties);

		// =================================
		// Get the queues
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(gpuInfoList[i].m_PhysicalDevice, &queueFamilyCount, nullptr);

		gpuInfoList[i].m_QueueFamilyProperties.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(gpuInfoList[i].m_PhysicalDevice, &queueFamilyCount, gpuInfoList[i].m_QueueFamilyProperties.data());

		// =================================
		// Get supported extensions
		uint32_t extensionCount = 0;
		result = vkEnumerateDeviceExtensionProperties(gpuInfoList[i].m_PhysicalDevice, nullptr, &extensionCount, nullptr);
		if (result != VK_SUCCESS)
		{
			return result;
		}

		std::vector<VkExtensionProperties> extensionProperties(extensionCount);
		result = vkEnumerateDeviceExtensionProperties(gpuInfoList[i].m_PhysicalDevice, nullptr, &extensionCount, extensionProperties.data());
		if (result != VK_SUCCESS)
		{
			return result;
		}

		for (auto extension : extensionProperties)
		{
			gpuInfoList[i].m_SupportedExtensions.push_back(extension.extensionName);
		}
	}

	return VK_SUCCESS;
}