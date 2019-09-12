#include "VulkanTexture.h"
#include "VulkanBase.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanCommandPool.h"
#include "VulkanFence.h"
#include "VulkanImage.h"
#include "VulkanImageView.h"
#include "VulkanBuffer.h"
#include "VulkanBarrier.h"
#include "VulkanFence.h"



VulkanTexture::VulkanTexture()
{
}


VulkanTexture::~VulkanTexture()
{
	Destroy();
}


void VulkanTexture::CreateFromFile(const char * const pFilename)
{
	gli::texture tex(gli::load(pFilename));
	
	assert(!tex.empty());

	std::vector<VkBufferImageCopy> bufferCopyRegions;

	uint32_t width;
	uint32_t height;
	FillBufferCopyRegions(bufferCopyRegions, tex, width, height);

	VkImageCreateFlags imageCreateFlags = 0;
	VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
	if (tex.target() == gli::target::TARGET_2D_ARRAY)
	{
		viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
	}
	else if (tex.target() == gli::target::TARGET_CUBE)
	{
		viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		imageCreateFlags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	}
	else if (tex.target() == gli::target::TARGET_CUBE_ARRAY)
	{
		viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
		imageCreateFlags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	}

	VulkanDevice * pDevice = g_pVulkanBase->GetDevice();

	VulkanBuffer buffer(*pDevice);
	buffer.Create(VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, tex.size());
	buffer.Map();
	buffer.Update(tex.data(), tex.size());
	buffer.Unmap();

	ImageCreateInfo imageCreateInfo(VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		{ width, height, 1 }, m_Levels, m_Layers, imageCreateFlags);
	m_pImage = new VulkanImage(*pDevice);
	m_pImage->Create(imageCreateInfo);

	VkImageSubresourceRange subresource{ VK_IMAGE_ASPECT_COLOR_BIT, 0, m_Levels, 0, m_Layers };
	ImageBarrier barrierForTransfer(0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		m_pImage->Get(), subresource);
	ImageBarrier barrierForShader(VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		m_pImage->Get(), subresource);

	VulkanFence fence(*pDevice);
	fence.Create();

	VulkanCommandPool * pCommandPool = g_pVulkanBase->GetDefaultCommandPool();
	VulkanCommandBuffer commandBuffer;
	pCommandPool->Allocate(&commandBuffer);

	commandBuffer.BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	// Switch layouts: undefined -> dst transfer
	commandBuffer.PipelineBarrier(VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		0, 0, nullptr, 0, nullptr, 1, &barrierForTransfer);
	// Copy data
	commandBuffer.CopyBufferToImage(buffer.Get(), m_pImage->Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		static_cast<uint32_t>(bufferCopyRegions.size()), bufferCopyRegions.data());
	// Switch layouts: dst transfer -> shader read
	commandBuffer.PipelineBarrier(VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		0, 0, nullptr, 0, nullptr, 1, &barrierForShader);
	commandBuffer.EndCommandBuffer();

	pDevice->GetGraphicsQueue()->Submit(commandBuffer.Get(), VK_NULL_HANDLE, VK_NULL_HANDLE, 0, fence.Get());
	fence.Wait();

	ImageViewCreateInfo imageViewCreateInfo(m_pImage->Get(), viewType, imageCreateInfo.format,
		VK_IMAGE_ASPECT_COLOR_BIT, m_Levels, m_Layers);
	m_pView = new VulkanImageView(*pDevice);
	m_pView->Create(imageViewCreateInfo);

	pCommandPool->Free(&commandBuffer);
	fence.Destroy();
	buffer.Destroy();
}


void VulkanTexture::Create(const TextureCreateInfo & textureCreateInfo)
{
	VulkanDevice * pDevice = g_pVulkanBase->GetDevice();

	VkImageCreateFlags imageCreateFlags = 0;
	VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
	m_Layers = textureCreateInfo.m_LayerCount;
	m_Levels = 1;
	if (textureCreateInfo.m_LayerCount > 1 && !textureCreateInfo.m_UseAsCubeMap)
	{
		viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
	}
	if (textureCreateInfo.m_UseAsCubeMap)
	{
		viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		imageCreateFlags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		m_Faces = 6;
		if (m_Layers > 1)
		{
			viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
		}
		m_Layers *= 6;
	}

	ImageCreateInfo imageCreateInfo(VK_IMAGE_TYPE_2D, textureCreateInfo.m_Format, textureCreateInfo.m_UsageFlags,
		{ textureCreateInfo.m_Width, textureCreateInfo.m_Height, 1 }, m_Levels, m_Layers, imageCreateFlags);
	m_pImage = new VulkanImage(*pDevice);
	m_pImage->Create(imageCreateInfo);

	ImageViewCreateInfo imageViewCreateInfo(m_pImage->Get(), viewType, textureCreateInfo.m_Format,
		textureCreateInfo.GetAspectMask(), m_Levels, m_Layers);
	m_pView = new VulkanImageView(*pDevice);
	m_pView->Create(imageViewCreateInfo);
}


void VulkanTexture::Destroy()
{
	m_pView->Destroy();
	m_pImage->Destroy();

	m_Levels = 0;
	m_Layers = 0;
	m_Faces = 0;
}


void VulkanTexture::FillBufferCopyRegions(std::vector<VkBufferImageCopy> & bufferCopyRegions, const gli::texture & tex,
	uint32_t & width, uint32_t & height)
{
	uint32_t offset = 0;
	if (tex.target() == gli::target::TARGET_2D)
	{
		gli::texture2d tex2d(tex);

		width = static_cast<uint32_t>(tex2d[0].extent().x);
		height = static_cast<uint32_t>(tex2d[0].extent().y);
		m_Levels = static_cast<uint32_t>(tex2d.levels());
		m_Layers = 1;
		m_Faces = 1;

		for (uint32_t i = 0; i < m_Levels; i++)
		{
			VkBufferImageCopy bufferCopyRegion = {};
			bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferCopyRegion.imageSubresource.mipLevel = i;
			bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
			bufferCopyRegion.imageSubresource.layerCount = 1;
			bufferCopyRegion.imageExtent.width = static_cast<uint32_t>(tex2d[i].extent().x);
			bufferCopyRegion.imageExtent.height = static_cast<uint32_t>(tex2d[i].extent().y);
			bufferCopyRegion.imageExtent.depth = 1;
			bufferCopyRegion.bufferOffset = offset;

			bufferCopyRegions.push_back(bufferCopyRegion);

			offset += static_cast<uint32_t>(tex2d[i].size());
		}
	}
	else if (tex.target() == gli::target::TARGET_2D_ARRAY)
	{
		gli::texture2d_array texArray(tex);

		width = static_cast<uint32_t>(texArray.extent().x);
		height = static_cast<uint32_t>(texArray.extent().y);
		m_Levels = static_cast<uint32_t>(texArray.levels());
		m_Layers = static_cast<uint32_t>(texArray.layers());;
		m_Faces = 1;

		for (uint32_t layer = 0; layer < m_Layers; ++layer)
		{
			for (uint32_t level = 0; level < m_Levels; ++level)
			{
				VkBufferImageCopy bufferCopyRegion = {};
				bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				bufferCopyRegion.imageSubresource.mipLevel = level;
				bufferCopyRegion.imageSubresource.baseArrayLayer = layer;
				bufferCopyRegion.imageSubresource.layerCount = 1;
				bufferCopyRegion.imageExtent.width = static_cast<uint32_t>(texArray[layer][level].extent().x);
				bufferCopyRegion.imageExtent.height = static_cast<uint32_t>(texArray[layer][level].extent().y);
				bufferCopyRegion.imageExtent.depth = 1;
				bufferCopyRegion.bufferOffset = offset;

				bufferCopyRegions.push_back(bufferCopyRegion);

				offset += static_cast<uint32_t>(texArray[layer][level].size());
			}
		}
	}
	else if (tex.target() == gli::target::TARGET_CUBE)
	{
		gli::texture_cube texCube(tex);

		width = static_cast<uint32_t>(texCube.extent().x);
		height = static_cast<uint32_t>(texCube.extent().y);
		m_Levels = static_cast<uint32_t>(texCube.levels());
		m_Layers = 1;
		m_Faces = static_cast<uint32_t>(texCube.faces());

		for (uint32_t face = 0; face < m_Faces; ++face)
		{
			for (uint32_t level = 0; level < m_Levels; ++level)
			{
				VkBufferImageCopy bufferCopyRegion = {};
				bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				bufferCopyRegion.imageSubresource.mipLevel = level;
				bufferCopyRegion.imageSubresource.baseArrayLayer = face;
				bufferCopyRegion.imageSubresource.layerCount = 1;
				bufferCopyRegion.imageExtent.width = static_cast<uint32_t>(texCube[face][level].extent().x);
				bufferCopyRegion.imageExtent.height = static_cast<uint32_t>(texCube[face][level].extent().y);
				bufferCopyRegion.imageExtent.depth = 1;
				bufferCopyRegion.bufferOffset = offset;

				bufferCopyRegions.push_back(bufferCopyRegion);

				offset += static_cast<uint32_t>(texCube[face][level].size());
			}
		}

		m_Layers *= m_Faces;
	}
	else if (tex.target() == gli::target::TARGET_CUBE_ARRAY)
	{
		gli::texture_cube_array texCubeArray(tex);

		width = static_cast<uint32_t>(texCubeArray.extent().x);
		height = static_cast<uint32_t>(texCubeArray.extent().y);
		m_Levels = static_cast<uint32_t>(texCubeArray.levels());
		m_Faces = static_cast<uint32_t>(texCubeArray.faces());
		m_Layers = static_cast<uint32_t>(texCubeArray.layers());

		for (uint32_t layer = 0; layer < m_Layers; ++layer)
		{
			for (uint32_t face = 0; face < 6; ++face)
			{
				for (uint32_t level = 0; level < m_Levels; ++level)
				{
					VkBufferImageCopy bufferCopyRegion = {};
					bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					bufferCopyRegion.imageSubresource.mipLevel = level;
					bufferCopyRegion.imageSubresource.baseArrayLayer = face;
					bufferCopyRegion.imageSubresource.layerCount = 1;
					bufferCopyRegion.imageExtent.width = static_cast<uint32_t>(texCubeArray[layer][face][level].extent().x);
					bufferCopyRegion.imageExtent.height = static_cast<uint32_t>(texCubeArray[layer][face][level].extent().y);
					bufferCopyRegion.imageExtent.depth = 1;
					bufferCopyRegion.bufferOffset = offset;

					bufferCopyRegions.push_back(bufferCopyRegion);

					offset += static_cast<uint32_t>(texCubeArray[layer][face][level].size());
				}
			}
		}

		m_Layers *= m_Faces;
	}
}