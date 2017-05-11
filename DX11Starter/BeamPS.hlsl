//Texture Vars
Texture2D ShadowMap		: register(t3);
SamplerComparisonState ShadowSampler : register(s1);

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
	SpotLight spotLight;
	float3 cameraPosition;
	float alpha;
	float scatterAmount;
};

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float3 worldPos		: WORLDPOS;
	float2 uv			: TEXCOORD;
	float4 posForShadow : POSITION1;
};

float InScatter(float3 start, float3 dir, float3 lightPos, float d)
{
	// calculate quadratic coefficients a,b,c
	float3 q = start - lightPos;

	float b = dot(dir, q);
	float c = dot(q, q);

	// evaluate integral
	float s = 1.0f / sqrt(c - b*b);

	float l = s * (atan((d + b) * s) - atan(b*s));

	return l;
}

float4 main(VertexToPixel input, bool isFrontFace : SV_IsFrontFace) : SV_TARGET
{
	float4 specularLight = { 0.0f, 0.0f, 0.0f, 0.0f };
	float4 diffuseLight = { 0.0f, 0.0f, 0.0f, 0.0f };
	float4 ambientLight = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Normalize the normal vector
	input.normal = normalize(input.normal);

	// Implementation example from: http://www.lighthouse3d.com/tutorials/glsl-12-tutorial/spot-light-per-pixel/
	if (spotLight.isOn == 1)
	{
		// Calculate the raw vector from spot light to the current pixel
		float3 dirToSpotLight = normalize(spotLight.Position - input.worldPos);
		float3 normalSpotDirection = normalize(spotLight.Direction);

		// Compute the NdotL for light amount given to the current pixel.
		float spotLightAmount = saturate(dot(input.normal, dirToSpotLight));

		// Saturate will clamp the value between 0 and 1. So if there is a light amount we continue.
		if (spotLightAmount > 0.0)
		{
			// Calculate the distance to the light source
			float distance = length(dirToSpotLight);

			// Calculate the vector between the light direction and the direction from the spot
			// light to the pixel
			float spotEffect = max(dot(-dirToSpotLight, normalSpotDirection), 0.0f);

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

				//// Calculate specular light
				//float3 dirToCamera = normalize(cameraPosition - input.worldPos);
				//float3 reflectionVector = reflect(-dirToSpotLight, input.normal);
				//specularLight += attenuationEffect * pow(saturate(dot(reflectionVector, dirToCamera)), 128);
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

	float3 viewDir = normalize(input.worldPos - cameraPosition);

	float inscattering = InScatter(cameraPosition, viewDir, spotLight.Position, depthFromLight) * scatterAmount; // Function from Macklin's blog.
	inscattering *= isFrontFace ? -1.0f : 1.0f; // isFrontFace = SV_IsFrontFace

	float4 finalColor = ambientLight + (diffuseLight * shadowAmount) + inscattering;

	finalColor.a = alpha * (1 - depthFromLight);

	return finalColor;
}