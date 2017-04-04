struct VertexToPixel
{
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float3 uv			: TEXCOORD;
};

TextureCube Texture : register(t0);
SamplerState Sampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	return Texture.Sample(Sampler, input.uv);
}