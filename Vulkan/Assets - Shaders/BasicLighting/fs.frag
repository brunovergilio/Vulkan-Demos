#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;

layout (location = 0) out vec4 outColor;

struct Light
{
	vec3 position;
	float range;
	vec3 direction;
	float outerCos;
	vec3 attenuation;
	float innerCos;
	vec3 color;
	uint lightType;
};

layout (binding = 1) uniform LightData
{
	Light light;
	vec3 viewPos;
} lightData;

float LambertDiffuse(vec3 light, vec3 normal)
{
	return max(dot(light, normal), 0.0);
}

float PhongSpecular(vec3 light, vec3 normal, vec3 view)
{
	vec3 r = reflect(light, normal);
	return pow(max(dot(r, view), 0.0f), 32);
}

vec4 DirectionalLight(vec3 normal, vec3 view)
{
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
	float diffuse = LambertDiffuse(-lightData.light.direction, normal);
	if (diffuse > 0.0)
	{
		float specular = PhongSpecular(lightData.light.direction, normal, view);
		
		color = vec4(lightData.light.color, 1.0) * (diffuse + specular);
	}
	
	return color;
}


vec4 PointLight(vec3 normal, vec3 view)
{
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
	vec3 l = lightData.light.position - inPos;
	float d = length(l);
	vec3 direction = l / d;
	if (lightData.light.range >= d)
	{
		float diffuse = LambertDiffuse(direction, normal);
		if (diffuse > 0.0)
		{
			float specular = PhongSpecular(-direction, normal, view);
			
			color = vec4(lightData.light.color, 1.0) * (diffuse + specular) * 1.0 / (dot(lightData.light.attenuation, vec3(1.0, d, d * d)));
		}
	}
	
	return color;
}

vec4 SpotLight(vec3 normal, vec3 view)
{
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
	vec3 l = lightData.light.position - inPos;
	float d = length(l);
	if (lightData.light.range >= d)
	{
		float diffuse = LambertDiffuse(-lightData.light.direction, normal);
		if (diffuse > 0.0)
		{
			diffuse *= smoothstep(lightData.light.outerCos, lightData.light.innerCos, diffuse);
			float specular = PhongSpecular(lightData.light.direction, normal, view);
			
			color = vec4(lightData.light.color, 1.0) * (diffuse + specular) * 1.0 / (dot(lightData.light.attenuation, vec3(1.0, d, d * d)));
		}
	}
	
	return color;
}


void main()
{
	vec3 normal = normalize(inNormal);
	vec3 view = normalize(lightData.viewPos - inPos);
	
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
	switch(lightData.light.lightType)
	{
	case 1:
		color = vec4(lightData.light.color, 1.0);
		break;
	case 2:
		color = DirectionalLight(normal, view);
		break;
	case 3:
		color = PointLight(normal, view);
		break;
	case 4:
		color = SpotLight(normal, view);
		break;		
	}
	
	outColor = color;
}