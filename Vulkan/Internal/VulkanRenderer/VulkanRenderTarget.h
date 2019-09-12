#pragma once


#include "VulkanTexture.h"


class VulkanRenderPass;
class VulkanFramebuffer;


class VulkanRenderTarget
{
public:
	VulkanRenderTarget();
	~VulkanRenderTarget();

	//void AddTarget(const TextureCreateInfo & textureDesc);
	//
	//inline const std::vector<VulkanTexture> & GetTextures() const { return m_Textures; }
	//inline const std::vector<VulkanFramebuffer> & GetFramebuffers() const { return m_FrameBuffers; }
	//inline const VulkanRenderPass * const GetRenderPass() const { return m_pRenderPass; }

private:
	//std::vector<TextureCreateInfo> m_TextureInfos;
	//std::vector<VulkanTexture> m_Textures;
	//std::vector<VulkanFramebuffer> m_FrameBuffers;
	//VulkanRenderPass * m_pRenderPass = nullptr;
};