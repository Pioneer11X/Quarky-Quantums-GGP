
#include "ParticleIncludes.hlsli"

cbuffer externalData : register(b0)
{
	matrix view;
	matrix projection;
	float aspectRatio;
};

StructuredBuffer<Particle>		ParticlePool	: register(t0);
StructuredBuffer<ParticleDraw>	DrawList		: register(t1);

struct VStoPS
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 uv		: TEXCOORD;
};

VStoPS main(uint id : SV_VertexID, uint iid : SV_InstanceID)
{
	// Get id info
	uint particleID = id / 4;
	uint cornerID = id % 4;

	// Look up the draw info, then this particle
	ParticleDraw draw = DrawList.Load(particleID);
	Particle particle = ParticlePool.Load(draw.Index);

	// Offsets for smaller triangles
	float2 offsets[4];
	offsets[0] = float2(-1.0f, -1.0f);  // BL
	offsets[1] = float2(-1.0f, +1.0f);  // TL
	offsets[2] = float2(+1.0f, -1.0f);  // BR
	offsets[3] = float2(+1.0f, +1.0f);  // TR

										// Output struct
	VStoPS output;

	// Calculate world view proj matrix
	float4 pos = float4(particle.Position, 1);

	pos = mul(pos, view);

	output.position = mul(pos, projection);

	// Depth stuff (change this to 1.0 for no size change when getting close)
	float depthChange = output.position.z / output.position.w;

	// Adjust based on depth
	offsets[cornerID].y *= aspectRatio;
	output.position.xy += offsets[cornerID] * depthChange * particle.Size;
	output.color = particle.Color;
	output.uv = saturate(offsets[cornerID]);

	return output;
}