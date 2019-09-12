#pragma once


#include "BvMath.h"
#include "VulkanUtils.h"


enum BvVertexLayout : uint32_t
{
	BV_VERTEX_POSITION = 0x1, // 3 floats
	BV_VERTEX_COLOR = 0x2, // 4 floats
	BV_VERTEX_TEXCOORD = 0x4, // 2 floats
	BV_VERTEX_NORMAL = 0x8, // 3 floats
	BV_VERTEX_TANGENT = 0x10, // 3 floats
	BV_VERTEX_BITANGENT = 0x20, // 3 floats
	BV_VERTEX_BLEND_INDICES = 0x40, // 4 unsigned ints
	BV_VERTEX_BLEND_WEIGHTS = 0x80 // 3 floats (don't need 4, since it's a normalized amount -> w4 = 1.0f - w1 - w2 - w3)
};


struct BvVertexPack
{
	BvVertexPack(const float val) : m_Data(val) {}
	BvVertexPack(const uint32_t val) : m_Data(val) {}
	
	union pack
	{
		float asFloat;
		uint32_t asUInt;
		
		pack(const float val) : asFloat(val) {}
		pack(const uint32_t val) : asUInt(val) {}
	} m_Data;
};


constexpr uint32_t g_BvVertexSize = sizeof(BvVertexPack);


typedef std::vector<BvVertexPack> BvVertexList;
template<class uintType>
using BvIndexList = std::vector<uintType>;


uint32_t GetVertexLayoutCount(const uint32_t & layout);
uint32_t GetVertexLayoutSize(const uint32_t & layout);