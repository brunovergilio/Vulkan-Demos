#include "Skybox.h"
#include "VulkanPipelineLayout.h"
#include "VulkanPipeline.h"
#include "VulkanShader.h"
#include "VertexLayout.h"
#include "VulkanImageView.h"
#include "VulkanFence.h"
#include "Primitives.h"
#include "VulkanCommandPool.h"
#include "VulkanQueue.h"


Skybox::Skybox()
{
}


Skybox::~Skybox()
{
}


void Skybox::Create(const VulkanDescriptorPool & descriptorPool, const VulkanPipelineCache & pipelineCache,
	const VulkanRenderPass & renderPass)
{
	VulkanDevice * pDevice = g_pVulkanBase->GetDevice();

	CreateBuffers();
	CreateUniformBuffers();

	DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
	descriptorSetLayoutCreateInfo.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	descriptorSetLayoutCreateInfo.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_pSetLayout = new VulkanDescriptorSetLayout(*pDevice);
	m_pSetLayout->Create(descriptorSetLayoutCreateInfo);

	m_pDescriptorSet = new VulkanDescriptorSet(*pDevice);
	descriptorPool.Allocate(m_pDescriptorSet, m_pSetLayout);

	PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
	pipelineLayoutCreateInfo.AddDescriptorSetLayout(m_pSetLayout->Get());
	m_pPipelineLayout = new VulkanPipelineLayout(*pDevice);
	m_pPipelineLayout->Create(pipelineLayoutCreateInfo);

	auto vsShader = g_pVulkanBase->GetShaderFilePath("vsSky.spv");
	auto fsShader = g_pVulkanBase->GetShaderFilePath("fsSky.spv");
	VulkanShader vs(*pDevice), fs(*pDevice);
	vs.Create(vsShader.c_str());
	fs.Create(fsShader.c_str());

	GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
	graphicsPipelineCreateInfo.SetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	graphicsPipelineCreateInfo.AddVertexBinding(0, sizeof(BvVertexPack) * 3, VK_VERTEX_INPUT_RATE_VERTEX);
	graphicsPipelineCreateInfo.AddVertexAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
	// No tessellation
	graphicsPipelineCreateInfo.SetRasterizer(VK_CULL_MODE_FRONT_BIT);
	// Default depth already set in constructor
	graphicsPipelineCreateInfo.SetStencilTest(VK_TRUE);
	graphicsPipelineCreateInfo.SetStencilOp();
	graphicsPipelineCreateInfo.SetStencilFunc(VK_COMPARE_OP_EQUAL);
	// Default Multisampling already set in constructor
	graphicsPipelineCreateInfo.AddBlendAttachment();
	graphicsPipelineCreateInfo.AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
	graphicsPipelineCreateInfo.AddDynamicState(VK_DYNAMIC_STATE_SCISSOR);
	graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vs.Get());
	graphicsPipelineCreateInfo.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fs.Get());
	graphicsPipelineCreateInfo.SetRenderInfo(m_pPipelineLayout->Get(), renderPass.Get(), 0);

	m_pPipeline = new VulkanPipeline(*pDevice);
	m_pPipeline->Create(graphicsPipelineCreateInfo, pipelineCache.Get());

	vs.Destroy();
	fs.Destroy();
}


void Skybox::Destroy()
{
	m_pVB->Destroy();
	m_pIB->Destroy();
	m_pUB->Destroy();

	m_pPipeline->Destroy();
	m_pPipelineLayout->Destroy();
	m_pSetLayout->Destroy();
}


void Skybox::Set(const VulkanTexture & texture, const VulkanSampler & sampler)
{
	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = m_pUB->Get();
	bufferInfo.range = VK_WHOLE_SIZE;

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = texture.GetView()->Get();
	imageInfo.sampler = sampler.Get();

	WriteDescriptorSet writeSets[] =
	{
		WriteDescriptorSet(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo),
		WriteDescriptorSet(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &imageInfo)
	};
	m_pDescriptorSet->Update(2, writeSets);
}


void Skybox::Update(const Float3 & camPos, const Float44 & viewProj)
{
	Float44 * pData = reinterpret_cast<Float44 *>(m_pUB->GetMappedData());

	BvFastMat wvp = MatrixTranslation(camPos.x, camPos.y, camPos.z) * Load44(viewProj.m);

	Store44(wvp, pData->m);
}


void Skybox::Render(const VulkanCommandBuffer & commandBuffer)
{
	commandBuffer.BindPipeline(m_pPipeline->Get());
	
	VkDescriptorSet set[] = { m_pDescriptorSet->Get() };
	commandBuffer.BindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, m_pPipelineLayout->Get(), 0, 1, set);
	
	VkBuffer vbs[] = { m_pVB->Get() };
	VkDeviceSize offsets[] = { 0 };
	commandBuffer.BindVertexBuffers(0, 1, vbs, offsets);
	commandBuffer.BindIndexBuffer(m_pIB->Get());
	commandBuffer.DrawIndexed(m_IndexCount);
}


void Skybox::CreateBuffers()
{
	BvVertexList vertices;
	BvIndexList<uint32_t> indices;

	CreateBox(BV_VERTEX_POSITION, vertices, indices, 2.0f, 2.0f, 2.0f);

	m_IndexCount = indices.size();

	VulkanDevice * pDevice = g_pVulkanBase->GetDevice();

	VulkanBuffer tmpVB(*pDevice);
	VkDeviceSize vbSize = sizeof(BvVertexPack) * vertices.size();
	tmpVB.Create(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		vbSize);
	tmpVB.Map();
	tmpVB.Update(vertices.data(), vbSize);
	tmpVB.Unmap();

	m_pVB = new VulkanBuffer(*pDevice);
	m_pVB->Create(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vbSize);


	VulkanBuffer tmpIB(*pDevice);
	VkDeviceSize ibSize = sizeof(uint32_t) * indices.size();
	tmpIB.Create(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		ibSize);
	tmpIB.Map();
	tmpIB.Update(indices.data(), ibSize);
	tmpIB.Unmap();

	m_pIB = new VulkanBuffer(*pDevice);
	m_pIB->Create(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ibSize);

	VulkanFence fence(*pDevice);
	fence.Create();

	VulkanCommandPool * pPool = g_pVulkanBase->GetDefaultCommandPool();
	VulkanCommandBuffer cb;
	pPool->Allocate(&cb);

	cb.BeginCommandBuffer();
	cb.CopyBuffer(tmpVB.Get(), m_pVB->Get(), vbSize);
	cb.CopyBuffer(tmpIB.Get(), m_pIB->Get(), ibSize);
	cb.EndCommandBuffer();

	pDevice->GetGraphicsQueue()->Submit(cb.Get(), VK_NULL_HANDLE, VK_NULL_HANDLE, 0, fence.Get());

	fence.Wait();

	tmpVB.Destroy();
	tmpIB.Destroy();
	fence.Destroy();

	pPool->Free(&cb);
}


void Skybox::CreateUniformBuffers()
{
	m_pUB = new VulkanBuffer(*g_pVulkanBase->GetDevice());
	m_pUB->Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		sizeof(WVP));
	m_pUB->Map();
}