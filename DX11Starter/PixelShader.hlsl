
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage

//Texture Vars
Texture2D diffuseTexture	: register(t0);
SamplerState basicSampler	: register(s0);

struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	//float4 color		: COLOR;
	float3 normal		: NORMAL;
	float3 worldPos		: WORLDPOS;
	float2 uv			: TEXCOORD;
};

struct DirectionalLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
};

struct PointLight
{
	float4 Color;
	float3 Position;
};

cbuffer ExternalData : register(b0)
{
	DirectionalLight light;
	DirectionalLight light2;
	PointLight pointLight;
	float3 cameraPosition;
};

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);

	// Normalize the normal vector
	input.normal = normalize(input.normal);

	// Negate light dir (fromDir) to get direction of light
	float3 lightDir = normalize(-light.Direction);
	float3 light2Dir = normalize(-light2.Direction);

	// Calculate N dot L
	float lightAmount = saturate(dot(input.normal, lightDir));
	float light2Amount = saturate(dot(input.normal, light2Dir));

	float3 dirToPointLight = normalize(pointLight.Position - input.worldPos);
	float pointLightAmount = saturate(dot(input.normal, dirToPointLight));

	float3 dirToCamera = normalize(cameraPosition - input.worldPos);
	float3 reflectionVector = reflect(-dirToPointLight, input.normal);
	float specularLight = pow(saturate(dot(reflectionVector, dirToCamera)), 128);

	return surfaceColor * 
		((light.DiffuseColor * lightAmount) +
		(light.AmbientColor) +
		(light2.DiffuseColor * light2Amount) +
		(light2.AmbientColor) +
		(pointLight.Color * pointLightAmount)) +
		(specularLight);
}