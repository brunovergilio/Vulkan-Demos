#version 450

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D depthInput;
layout(set = 0, binding = 1) uniform sampler2D diffuseInput;
layout(set = 0, binding = 2) uniform sampler2D normalInput;

layout (binding = 3) uniform GBufferUnpackData
{
	mat4 invView;
	vec4 projValues;
} unpackData;

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

layout (binding = 4) uniform LightData
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
	return pow(max(dot(r, view), 0.0f), 16);
}

vec3 DirectionalLight(vec3 normal, vec3 view)
{
	vec3 color = vec3(0.0, 0.0, 0.0);
	float diffuse = LambertDiffuse(-lightData.light.direction, normal);
	if (diffuse > 0.0)
	{
		float specular = PhongSpecular(lightData.light.direction, normal, view);
		
		color = lightData.light.color.rgb * (diffuse + specular);
	}
	
	return color;
}


float ToLinearDepth(float depth)
{
	return unpackData.projValues.w / (depth - unpackData.projValues.z);
}

vec3 ToWorldPos(float linearDepth)
{
	vec4 posVS = vec4(unpackData.projValues.xy * linearDepth, linearDepth, 1.0);
	
	return (unpackData.invView * posVS).xyz;
}

void main()
{
	float linearDepth = ToLinearDepth(texture(depthInput, inUV).x);
	vec3 posWS = ToWorldPos(linearDepth);
	
	vec4 color = texture(diffuseInput, inUV);
	vec3 normal = texture(normalInput, inUV).xyz * 2.0 - 1.0;
	
	const vec3 ambient = vec3(0.4, 0.4, 0.4);
	
	outColor = color * vec4(ambient + DirectionalLight(normal, normalize(lightData.viewPos - posWS)), 1.0);
}