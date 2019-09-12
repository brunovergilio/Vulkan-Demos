#pragma once


#include "VulkanUtils.h"
#include "VertexLayout.h"


// =================================================
// Declarations
template<typename uintType>
void CreateBox(const uint32_t layout, BvVertexList & vertexList, BvIndexList<uintType> & indexList,
	const float width = 1.0f, const float height = 1.0f, const float depth = 1.0f,
	const Float4 & color = Float4(1.0f, 1.0f, 1.0f, 1.0f));


template<typename uintType>
void CreateGeosphere(const uint32_t layout, BvVertexList & vertexList, BvIndexList<uintType> & indexList,
	const uint32_t numSubdivisions = 3, const float radius = 1.0f,
	const Float4 & color = Float4(1.0f, 1.0f, 1.0f, 1.0f));


template<typename uintType>
void CreateGrid(const uint32_t layout, BvVertexList & vertexList, BvIndexList<uintType> & indexList,
	const float width, const float depth, const uint32_t numRows = 1, const uint32_t numColumns = 1,
	const Float4 & color = Float4(1.0f, 1.0f, 1.0f, 1.0f));


// =================================================
// Definitions
template<typename uintType>
void CreateBox(const uint32_t layout,
	BvVertexList & vertexList, BvIndexList<uintType> & indexList,
	const float width, const float height, const float depth,
	const Float4 & color)
{
	// ========================================
	// Code below property of Frank D. Luna
	// ========================================

	struct LunaVertex
	{
		LunaVertex() {}
		LunaVertex(const Float3& p, const Float3& n, const Float3& t, const Float2& uv)
			: Position(p), Normal(n), TangentU(t), TexC(uv) {}
		LunaVertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			: Position(px, py, pz), Normal(nx, ny, nz),
			TangentU(tx, ty, tz), TexC(u, v) {}

		Float3 Position;
		Float3 Normal;
		Float3 TangentU;
		Float2 TexC;
	};

	//
	// Create the vertices.
	//

	LunaVertex v[24];

	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * depth;

	// Fill in the front face vertex data.
	v[0] = LunaVertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[1] = LunaVertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = LunaVertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[3] = LunaVertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	v[4] = LunaVertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[5] = LunaVertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[6] = LunaVertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[7] = LunaVertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	v[8] = LunaVertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[9] = LunaVertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[10] = LunaVertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[11] = LunaVertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	v[12] = LunaVertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[13] = LunaVertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[14] = LunaVertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[15] = LunaVertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	v[16] = LunaVertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[17] = LunaVertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[18] = LunaVertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[19] = LunaVertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	v[20] = LunaVertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[21] = LunaVertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[22] = LunaVertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[23] = LunaVertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	std::vector<LunaVertex> vertices;
	vertices.assign(&v[0], &v[24]);

	//
	// Create the indices.
	//

	uint32_t i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	for (auto j = 0U; j < (sizeof(i) / sizeof(i[0])); ++j)
	{
		indexList.push_back(static_cast<uintType>(i[j]));
	}

	// ========================================
	// Code above property of Frank D. Luna
	// ========================================

	for (auto vertexIndex = 0U; vertexIndex < static_cast<uint32_t>(vertices.size()); ++vertexIndex)
	{
		const LunaVertex & vertex = vertices[vertexIndex];

		if (layout & BvVertexLayout::BV_VERTEX_POSITION)
		{
			vertexList.push_back(BvVertexPack(vertex.Position.x));
			vertexList.push_back(BvVertexPack(vertex.Position.y));
			vertexList.push_back(BvVertexPack(vertex.Position.z));
		}
		if (layout & BvVertexLayout::BV_VERTEX_COLOR)
		{
			vertexList.push_back(BvVertexPack(color.x));
			vertexList.push_back(BvVertexPack(color.y));
			vertexList.push_back(BvVertexPack(color.z));
			vertexList.push_back(BvVertexPack(color.w));
		}
		if (layout & BvVertexLayout::BV_VERTEX_TEXCOORD)
		{
			vertexList.push_back(BvVertexPack(vertex.TexC.x));
			vertexList.push_back(BvVertexPack(vertex.TexC.y));
		}
		if (layout & BvVertexLayout::BV_VERTEX_NORMAL)
		{
			vertexList.push_back(BvVertexPack(vertex.Normal.x));
			vertexList.push_back(BvVertexPack(vertex.Normal.y));
			vertexList.push_back(BvVertexPack(vertex.Normal.z));
		}
		if (layout & BvVertexLayout::BV_VERTEX_TANGENT)
		{
			vertexList.push_back(BvVertexPack(vertex.TangentU.x));
			vertexList.push_back(BvVertexPack(vertex.TangentU.y));
			vertexList.push_back(BvVertexPack(vertex.TangentU.z));
		}
		if (layout & BvVertexLayout::BV_VERTEX_BITANGENT)
		{
			BvFastVec tangent = VectorSet(vertex.TangentU.x, vertex.TangentU.y, vertex.TangentU.z);
			BvFastVec normal = VectorSet(vertex.Normal.x, vertex.Normal.y, vertex.Normal.z);
			Float3 bitangent;
			Store3(VectorCross(tangent, normal), bitangent.v);

			vertexList.push_back(BvVertexPack(bitangent.x));
			vertexList.push_back(BvVertexPack(bitangent.y));
			vertexList.push_back(BvVertexPack(bitangent.z));
		}
	}
}


template<typename uintType>
void CreateGeosphere(const uint32_t layout,
	BvVertexList & vertexList, BvIndexList<uintType> & indexList,
	const uint32_t numSubdivisions, const float radius,
	const Float4 & color)
{
	// ========================================
	// Code below property of Frank D. Luna
	// ========================================

	struct LunaVertex
	{
		LunaVertex() {}
		LunaVertex(const Float3& p, const Float3& n, const Float3& t, const Float2& uv)
			: Position(p), Normal(n), TangentU(t), TexC(uv) {}
		LunaVertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			: Position(px, py, pz), Normal(nx, ny, nz),
			TangentU(tx, ty, tz), TexC(u, v) {}

		Float3 Position;
		Float3 Normal;
		Float3 TangentU;
		Float2 TexC;
	};

	auto subdivideFunc = [](std::vector<LunaVertex> & vertices, std::vector<uint32_t> & indices) -> void
	{
		// Save a copy of the input geometry.
		std::vector<LunaVertex> copyVertices = vertices;
		std::vector<uint32_t> copyIndices = indices;


		vertices.resize(0);
		indices.resize(0);

		//       v1
		//       *
		//      / \
		//     /   \
		//  m0*-----*m1
		//   / \   / \
		//  /   \ /   \
		// *-----*-----*
		// v0    m2     v2

		UINT numTris = copyIndices.size() / 3;
		for (UINT i = 0; i < numTris; ++i)
		{
			LunaVertex v0 = copyVertices[copyIndices[i * 3 + 0]];
			LunaVertex v1 = copyVertices[copyIndices[i * 3 + 1]];
			LunaVertex v2 = copyVertices[copyIndices[i * 3 + 2]];

			//
			// Generate the midpoints.
			//

			LunaVertex m0, m1, m2;

			// For subdivision, we just care about the position component.  We derive the other
			// vertex components in CreateGeosphere.

			m0.Position = Float3(
				0.5f*(v0.Position.x + v1.Position.x),
				0.5f*(v0.Position.y + v1.Position.y),
				0.5f*(v0.Position.z + v1.Position.z));

			m1.Position = Float3(
				0.5f*(v1.Position.x + v2.Position.x),
				0.5f*(v1.Position.y + v2.Position.y),
				0.5f*(v1.Position.z + v2.Position.z));

			m2.Position = Float3(
				0.5f*(v0.Position.x + v2.Position.x),
				0.5f*(v0.Position.y + v2.Position.y),
				0.5f*(v0.Position.z + v2.Position.z));

			//
			// Add new geometry.
			//

			vertices.push_back(v0); // 0
			vertices.push_back(v1); // 1
			vertices.push_back(v2); // 2
			vertices.push_back(m0); // 3
			vertices.push_back(m1); // 4
			vertices.push_back(m2); // 5

			indices.push_back(static_cast<uintType>(i * 6 + 0));
			indices.push_back(static_cast<uintType>(i * 6 + 3));
			indices.push_back(static_cast<uintType>(i * 6 + 5));
							  								 
			indices.push_back(static_cast<uintType>(i * 6 + 3));
			indices.push_back(static_cast<uintType>(i * 6 + 4));
			indices.push_back(static_cast<uintType>(i * 6 + 5));
							  								 
			indices.push_back(static_cast<uintType>(i * 6 + 5));
			indices.push_back(static_cast<uintType>(i * 6 + 4));
			indices.push_back(static_cast<uintType>(i * 6 + 2));
							  								 
			indices.push_back(static_cast<uintType>(i * 6 + 3));
			indices.push_back(static_cast<uintType>(i * 6 + 1));
			indices.push_back(static_cast<uintType>(i * 6 + 4));
		}
	};

	// Approximate a sphere by tessellating an icosahedron.

	const float X = 0.525731f;
	const float Z = 0.850651f;

	Float3 pos[12] =
	{
		Float3(-X, 0.0f, Z),  Float3(X, 0.0f, Z),
		Float3(-X, 0.0f, -Z), Float3(X, 0.0f, -Z),
		Float3(0.0f, Z, X),   Float3(0.0f, Z, -X),
		Float3(0.0f, -Z, X),  Float3(0.0f, -Z, -X),
		Float3(Z, X, 0.0f),   Float3(-Z, X, 0.0f),
		Float3(Z, -X, 0.0f),  Float3(-Z, -X, 0.0f)
	};

	DWORD k[60] =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	std::vector<LunaVertex> vertices;
	vertices.resize(12);
	indexList.resize(60);

	for (UINT i = 0; i < 12; ++i)
		vertices[i].Position = pos[i];

	for (UINT i = 0; i < 60; ++i)
		indexList[i] = static_cast<uintType>(k[i]);

	uint32_t subdivisionCount = numSubdivisions > 5 ? 5 : numSubdivisions;
	for (UINT i = 0; i < subdivisionCount; ++i)
		subdivideFunc(vertices, indexList);

	// Project vertices onto sphere and scale.
	for (UINT i = 0; i < vertices.size(); ++i)
	{
		// Project onto unit sphere.
		BvFastVec n = VectorNormalize(Load3(vertices[i].Position.v));

		// Project onto sphere.
		BvFastVec p = VectorMul(n, radius);

		Store3(p, vertices[i].Position.v);
		Store3(n, vertices[i].Normal.v);

		// Derive texture coordinates from spherical coordinates.
		float theta = atan2f(
			vertices[i].Position.z,
			vertices[i].Position.x);

		float phi = acosf(vertices[i].Position.y / radius);

		vertices[i].TexC.x = theta / Bv2Pi;
		vertices[i].TexC.y = phi / BvPi;

		// Partial derivative of P with respect to theta
		vertices[i].TangentU.x = -radius * sinf(phi)*sinf(theta);
		vertices[i].TangentU.y = 0.0f;
		vertices[i].TangentU.z = +radius * sinf(phi)*cosf(theta);

		BvFastVec T = Load3(vertices[i].TangentU.v);
		Store3(VectorNormalize(T), vertices[i].TangentU.v);
	}

	// ========================================
	// Code above property of Frank D. Luna
	// ========================================

	for (auto vertexIndex = 0U; vertexIndex < static_cast<uint32_t>(vertices.size()); ++vertexIndex)
	{
		const LunaVertex & vertex = vertices[vertexIndex];

		if (layout & BvVertexLayout::BV_VERTEX_POSITION)
		{
			vertexList.push_back(BvVertexPack(vertex.Position.x));
			vertexList.push_back(BvVertexPack(vertex.Position.y));
			vertexList.push_back(BvVertexPack(vertex.Position.z));
		}
		if (layout & BvVertexLayout::BV_VERTEX_COLOR)
		{
			vertexList.push_back(BvVertexPack(color.x));
			vertexList.push_back(BvVertexPack(color.y));
			vertexList.push_back(BvVertexPack(color.z));
			vertexList.push_back(BvVertexPack(color.w));
		}
		if (layout & BvVertexLayout::BV_VERTEX_TEXCOORD)
		{
			vertexList.push_back(BvVertexPack(vertex.TexC.x));
			vertexList.push_back(BvVertexPack(vertex.TexC.y));
		}
		if (layout & BvVertexLayout::BV_VERTEX_NORMAL)
		{
			vertexList.push_back(BvVertexPack(vertex.Normal.x));
			vertexList.push_back(BvVertexPack(vertex.Normal.y));
			vertexList.push_back(BvVertexPack(vertex.Normal.z));
		}
		if (layout & BvVertexLayout::BV_VERTEX_TANGENT)
		{
			vertexList.push_back(BvVertexPack(vertex.TangentU.x));
			vertexList.push_back(BvVertexPack(vertex.TangentU.y));
			vertexList.push_back(BvVertexPack(vertex.TangentU.z));
		}
		if (layout & BvVertexLayout::BV_VERTEX_BITANGENT)
		{
			BvFastVec tangent = VectorSet(vertex.TangentU.x, vertex.TangentU.y, vertex.TangentU.z);
			BvFastVec normal = VectorSet(vertex.Normal.x, vertex.Normal.y, vertex.Normal.z);
			Float3 bitangent;
			Store3(VectorCross(tangent, normal), bitangent.v);

			vertexList.push_back(BvVertexPack(bitangent.x));
			vertexList.push_back(BvVertexPack(bitangent.y));
			vertexList.push_back(BvVertexPack(bitangent.z));
		}
	}
}


template<typename uintType>
inline void CreateGrid(const uint32_t layout, BvVertexList & vertexList, BvIndexList<uintType>& indexList,
	const float width, const float depth, const uint32_t numRows, const uint32_t numColumns,
	const Float4 & color)
{
	float currX = -width * 0.5f;
	float currZ = +depth * 0.5f;

	float stepX = width / static_cast<float>(numColumns);
	float stepZ = -depth / static_cast<float>(numRows);

	uint32_t itemsPerCol = numColumns + 1;
	uint32_t itemsPerRow = numColumns + 1;
	vertexList.reserve(itemsPerCol * itemsPerRow * GetVertexLayoutSize(layout));

	uint32_t row, col;
	for (row = 0U; row <= numRows; ++row)
	{
		currX = -width * 0.5f;

		for (col = 0U; col <= numColumns; ++col)
		{
			if (layout & BvVertexLayout::BV_VERTEX_POSITION)
			{
				vertexList.push_back(BvVertexPack(currX));
				vertexList.push_back(BvVertexPack(0.0f));
				vertexList.push_back(BvVertexPack(currZ));
			}
			if (layout & BvVertexLayout::BV_VERTEX_COLOR)
			{
				vertexList.push_back(BvVertexPack(color.x));
				vertexList.push_back(BvVertexPack(color.y));
				vertexList.push_back(BvVertexPack(color.z));
				vertexList.push_back(BvVertexPack(color.w));
			}
			if (layout & BvVertexLayout::BV_VERTEX_TEXCOORD)
			{
				vertexList.push_back(BvVertexPack(static_cast<float>(col) / static_cast<float>(numColumns)));
				vertexList.push_back(BvVertexPack(static_cast<float>(row) / static_cast<float>(numRows)));
			}
			if (layout & BvVertexLayout::BV_VERTEX_NORMAL)
			{
				vertexList.push_back(BvVertexPack(0.0f));
				vertexList.push_back(BvVertexPack(1.0f));
				vertexList.push_back(BvVertexPack(0.0f));
			}
			if (layout & BvVertexLayout::BV_VERTEX_TANGENT)
			{
				vertexList.push_back(BvVertexPack(1.0f));
				vertexList.push_back(BvVertexPack(0.0f));
				vertexList.push_back(BvVertexPack(0.0f));
			}
			if (layout & BvVertexLayout::BV_VERTEX_BITANGENT)
			{
				vertexList.push_back(BvVertexPack(0.0f));
				vertexList.push_back(BvVertexPack(0.0f));
				vertexList.push_back(BvVertexPack(1.0f));
			}

			currX += stepX;
		}

		currZ = stepZ;
	}

	for (row = 0U; row < numRows; ++row)
	{
		for (col = 0U; col < numColumns; ++col)
		{
			indexList.push_back(static_cast<uintType>(row * itemsPerRow + col));
			indexList.push_back(static_cast<uintType>(row * itemsPerRow + col + 1));
			indexList.push_back(static_cast<uintType>((row + 1) * itemsPerRow + col + 1));
								
			indexList.push_back(static_cast<uintType>(row * itemsPerRow + col));
			indexList.push_back(static_cast<uintType>((row + 1) * itemsPerRow + col + 1));
			indexList.push_back(static_cast<uintType>((row + 1) * itemsPerRow + col));
		}
	}
}