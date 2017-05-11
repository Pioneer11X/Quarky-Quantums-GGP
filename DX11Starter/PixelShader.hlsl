
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage

//Texture Vars
Texture2D Texture	: register(t0);
SamplerState Sampler	: register(s0);
Texture2D ShadowMap		: register(t3);
SamplerComparisonState ShadowSampler : register(s1);

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
	float4 posForShadow : POSITION1;
};

struct DirectionalLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
	int isOn;
};

struct PointLight
{
	float4 Color;
	float3 Position;
	int isOn;
};

// The order for these members matters
// If two float3 values are by eachother then the data structure is unaligned.
struct SpotLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
	float AngleRads;
	float3 Position;
	int isOn;
	float SpotIntensity;
	float ConstAtten;
	float LinearAtten;
	float ExpoAtten;
};

cbuffer ExternalData : register(b0)
{
	DirectionalLight light;
	DirectionalLight light2;
	PointLight pointLight;
	SpotLight spotLight;
	float3 cameraPosition;
	float alpha;
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
	float4 surfaceColor = Texture.Sample(Sampler, input.uv);
	float4 specularLight = { 0.0f, 0.0f, 0.0f, 0.0f };
	float4 diffuseLight = { 0.0f, 0.0f, 0.0f, 0.0f };
	float4 ambientLight = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Normalize the normal vector
	input.normal = normalize(input.normal);


	if (light.isOn == 1)
	{
		// Negate light dir (fromDir) to get direction of light
		float3 lightDir = normalize(-light.Direction);

		// Calculate N dot L
		float lightAmount = saturate(dot(input.normal, lightDir));

		diffuseLight += (light.DiffuseColor * lightAmount);
		ambientLight += (light.AmbientColor);
	}

	if (light2.isOn == 1)
	{
		// Negate light dir (fromDir) to get direction of light
		float3 light2Dir = normalize(-light2.Direction);

		// Calculate N dot L
		float light2Amount = saturate(dot(input.normal, light2Dir));

		diffuseLight += (light2.DiffuseColor * light2Amount);
		ambientLight += (light2.AmbientColor);
	}

	if (pointLight.isOn == 1)
	{
		float3 dirToPointLight = normalize(pointLight.Position - input.worldPos);
		float pointLightAmount = saturate(dot(input.normal, dirToPointLight));

		float3 dirToCamera = normalize(cameraPosition - input.worldPos);
		float3 reflectionVector = reflect(-dirToPointLight, input.normal);
		specularLight += pow(saturate(dot(reflectionVector, dirToCamera)), 128);

		diffuseLight += (pointLight.Color * pointLightAmount);
	}

	// Implementation example from: http://www.lighthouse3d.com/tutorials/glsl-12-tutorial/spot-light-per-pixel/
	if (spotLight.isOn == 1)
	{
		// Calculate the raw vector from spot light to the current pixel
		float3 dirToSpotLight = spotLight.Position - input.worldPos;
		float3 normalDirToSpotLight = normalize(dirToSpotLight);
		float3 normalSpotDirection = normalize(spotLight.Direction);

		// Compute the NdotL for light amount given to the current pixel.
		float spotLightAmount = saturate(dot(input.normal, normalDirToSpotLight));

		// Saturate will clamp the value between 0 and 1. So if there is a light amount we continue.
		if (spotLightAmount > 0.0)
		{
			// Calculate the distance to the light source
			float distance = length(dirToSpotLight);

			// Calculate the vector between the light direction and the direction from the spot
			// light to the pixel
			float spotEffect = max(dot(-normalDirToSpotLight, normalSpotDirection), 0.0f);

			// Calculate the angle between two vectors
			float angle = acos(spotEffect);

			// Is this length greather than the what it should be based on the angle of our spot light?
			// In other words is the point outside the spot light?
			if (angle <= spotLight.AngleRads)
			{
				// Raise the intensity of the spot effect by a factor.
				spotEffect = pow(spotEffect, spotLight.SpotIntensity);
				// Attenuation equation for light dropoff.
				float attenuationEffect = spotEffect / (spotLight.ConstAtten + spotLight.LinearAtten * distance + spotLight.ExpoAtten * distance * distance);

				// Calculate diffuse light
				diffuseLight += (attenuationEffect * spotLight.DiffuseColor * spotLightAmount);
				ambientLight += (spotLight.AmbientColor);

				// Calculate specular light
				float3 dirToCamera = normalize(cameraPosition - input.worldPos);
				float3 reflectionVector = reflect(-dirToSpotLight, input.normal);
				specularLight += attenuationEffect * pow(saturate(dot(reflectionVector, dirToCamera)), 128);
			}
		}

	}

	// Shadow mapping

	// Calculate this pixel's UV on the shadow map
	float2 shadowUV = input.posForShadow.xy / input.posForShadow.w * 0.5f + 0.5f;

	// Flip the Y's because the NDC and UV's are upside down
	shadowUV.y = 1.0f - shadowUV.y;

	// Get the actual depth from the light's position
	float depthFromLight = input.posForShadow.z / input.posForShadow.w;

	//// Can be used to limit the lights travel distance
	//if (depthFromLight > 0.99)
	//	diffuseLight = 0;

	float shadowAmount = ShadowMap.SampleCmpLevelZero(
		ShadowSampler,
		shadowUV,
		depthFromLight);

	float4 finalColor = surfaceColor * (ambientLight + (diffuseLight * shadowAmount)) + (specularLight);

	finalColor.a = alpha;

	return finalColor;
}