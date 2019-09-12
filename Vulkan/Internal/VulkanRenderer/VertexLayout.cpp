#include "VertexLayout.h"


uint32_t GetVertexLayoutCount(const uint32_t & layout)
{
	uint32_t layoutCount = 0;

	if (layout & BvVertexLayout::BV_VERTEX_POSITION) { layoutCount += 3; }
	if (layout & BvVertexLayout::BV_VERTEX_COLOR) { layoutCount += 4; }
	if (layout & BvVertexLayout::BV_VERTEX_TEXCOORD) { layoutCount += 2; }
	if (layout & BvVertexLayout::BV_VERTEX_NORMAL) { layoutCount += 3; }
	if (layout & BvVertexLayout::BV_VERTEX_TANGENT) { layoutCount += 3; }
	if (layout & BvVertexLayout::BV_VERTEX_BITANGENT) { layoutCount += 3; }
	if (layout & BvVertexLayout::BV_VERTEX_BLEND_INDICES) { layoutCount += 4; }
	if (layout & BvVertexLayout::BV_VERTEX_BLEND_WEIGHTS) { layoutCount += 3; }

	return layoutCount;
}


uint32_t GetVertexLayoutSize(const uint32_t & layout)
{
	return GetVertexLayoutCount(layout) * sizeof(BvVertexPack);
}