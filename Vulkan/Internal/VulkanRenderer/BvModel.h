#pragma once


#include "VertexLayout.h"


struct BvMesh
{
	BvVertexList m_Vertices;
	BvIndexList<uint32_t> m_Indices;
	uint32_t m_Layout = 0;
	std::string m_AmbientMap;
	std::string m_EmissiveMap;
	std::string m_DiffuseMap;
	std::string m_SpecularMap;
	std::string m_NormalMap;
	std::string m_HeightMap;
	std::string m_GlossMap;

	BvMesh() = default;
	BvMesh(const uint32_t layout) : m_Layout(layout) {}

	void SetLayout(const uint32_t layout) { m_Layout = layout; }
};


struct BvModelInfo
{
	uint32_t vertexOffset;
	uint32_t vertexCount;
	uint32_t indexOffset;
	uint32_t indexCount;
	uint8_t DiffuseTextureIndex;
};


class BvModel
{
public:
	BvModel();
	~BvModel();

private:
};