#pragma once


#include "VulkanCommandBuffer.h"
#pragma warning(push, 0)
#include "gli/gli.hpp"
#pragma warning(pop)


struct TextureCreateInfo
{
	uint32_t m_Width = 0;
	uint32_t m_Height = 0;
	uint32_t m_LayerCount = 1;
	VkImageUsageFlags m_UsageFlags = 0;
	VkSampleCountFlags m_SampleCount = VK_SAMPLE_COUNT_1_BIT;
	VkFormat m_Format = VK_FORMAT_UNDEFINED;
	bool m_UseAsCubeMap = false;
	bool m_UseForOffscreen = false;

	TextureCreateInfo() = default;

	TextureCreateInfo(const uint32_t width, const uint32_t height, const VkFormat format, const VkImageUsageFlags usageFlags,
		const uint32_t layerCount = 1, const VkSampleCountFlags sampleCount = VK_SAMPLE_COUNT_1_BIT, const bool useAsCubeMap = false)
		: m_Width(width), m_Height(height), m_Format(format), m_UsageFlags(usageFlags),
		m_LayerCount(layerCount), m_SampleCount(sampleCount), m_UseAsCubeMap(useAsCubeMap) {}

	inline TextureCreateInfo & SetDimensions(const uint32_t width, const uint32_t height) { m_Width = width; m_Height = height; return *this; }
	inline TextureCreateInfo & SetFormat(const VkFormat format) { m_Format = format; return *this; }
	inline TextureCreateInfo & SetLayerCount(const uint32_t layerCount = 1) { m_LayerCount = layerCount; return *this; }
	inline TextureCreateInfo & SetSampleCount(const VkSampleCountFlags sampleCount = VK_SAMPLE_COUNT_1_BIT) { m_SampleCount = sampleCount; return *this; }
	inline TextureCreateInfo & SetUsage(const VkImageUsageFlags usageFlags) { m_UsageFlags = usageFlags; return *this; }
	inline TextureCreateInfo & UseAsCubeMap(const bool use = false) { m_UseAsCubeMap = use; return *this; }
	inline TextureCreateInfo & UseForOffscreen(const bool use = false) { m_UseForOffscreen = use; return *this; }

	inline TextureCreateInfo & UseForColor() { m_UsageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; return *this; }
	inline TextureCreateInfo & UseForDepthStencil() { m_UsageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; return *this; }
	inline TextureCreateInfo & UseForShaderResource() { m_UsageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT; return *this; }
	inline TextureCreateInfo & UseForSubpassInput() { m_UsageFlags |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT; return *this; }
	inline TextureCreateInfo & UseForTransient() { m_UsageFlags |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT; return *this; }
	inline TextureCreateInfo & UseForSrcTransfer() { m_UsageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT; return *this; }
	inline TextureCreateInfo & UseForDstTransfer() { m_UsageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT; return *this; }

	inline const VkImageAspectFlags GetAspectMask() const
	{
		VkImageAspectFlags aspectMask = 0;
		if (m_UsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		{
			aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}
		else
		{
			const VkFormat formats[] =
			{
				VK_FORMAT_D16_UNORM,
				VK_FORMAT_X8_D24_UNORM_PACK32,
				VK_FORMAT_D32_SFLOAT,
				VK_FORMAT_D16_UNORM_S8_UINT,
				VK_FORMAT_D24_UNORM_S8_UINT,
				VK_FORMAT_D32_SFLOAT_S8_UINT,
				VK_FORMAT_S8_UINT
			};

			for (auto i = 0U; i < 7; ++i)
			{
				if (m_Format == formats[i])
				{
					if (i < 6) { aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT; }
					if (i > 2) { aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT; }
					break;
				}
			}
		}

		return aspectMask;
	}
};


class VulkanImage;
class VulkanImageView;


class VulkanTexture
{
public:
	VulkanTexture();
	~VulkanTexture();

	void CreateFromFile(const char * const pFilename);
	void Create(const TextureCreateInfo & textureCreateInfo);
	void Destroy();

	inline VulkanImage * const GetImage() const { return m_pImage; }
	inline VulkanImageView * const GetView() const { return m_pView; }
	inline uint32_t const GetLevelCount() const { return m_Levels; }
	inline uint32_t const GetLayerCount() const { return m_Layers; }
	inline uint32_t const GetFaceCount() const { return m_Faces; }

private:
	void FillBufferCopyRegions(std::vector<VkBufferImageCopy> & bufferCopyRegions, const gli::texture & tex,
		uint32_t & width, uint32_t & height);

private:
	VulkanImage * m_pImage = nullptr;
	VulkanImageView * m_pView = nullptr;
	uint32_t m_Levels = 0;
	uint32_t m_Layers = 0;
	uint32_t m_Faces = 0;
};