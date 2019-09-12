#pragma once


#include "BvMath.h"


enum class LightType : uint32_t
{
	Undefined,
	Ambient,
	Directional,
	Point,
	Spot
};


struct Light
{
	Float3 m_Position{};
	float m_Range = 0.0f;
	Float3 m_Direction{};
	float m_OuterCos = 0.0f;
	Float3 m_Attenuation{};
	float m_InnerCos = 0.0f;
	Float3 m_Color{ 1.0f, 1.0f, 1.0f };
	LightType m_LightType = LightType::Undefined;
};