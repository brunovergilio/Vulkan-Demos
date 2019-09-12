#pragma once


#include "VulkanBase.h"


class ForwardRenderer;


class ClearScreen : public VulkanBase
{
public:
	ClearScreen();
	~ClearScreen();

private:
	virtual void Create(const VulkanRenderInfo & info) override;
	virtual void Destroy() override;

	void Resize() override;

	void CreateRenderers();
	void RecordCommandBuffers();

	void Update() override {};
	void Render() override;

private:
	ForwardRenderer * m_pForwardRenderer = nullptr;
};