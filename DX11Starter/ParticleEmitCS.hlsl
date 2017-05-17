
#include "ParticleIncludes.hlsli"
#include "SimplexNoise.hlsli"

cbuffer ExternalData : register(b0)
{
	float4 Position;
	float TotalTime;
	int EmitCount;
	int MaxParticles;
	int GridSize;
}

// Order should match UpdateCS (RW binding issues)
RWStructuredBuffer<Particle> ParticlePool	: register(u0);
ConsumeStructuredBuffer<uint> DeadList		: register(u1);


[numthreads(32, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
	// Outside range?
	if(id.x >= (uint)EmitCount) return;

	 // Grab a single index from the dead list
	uint emitIndex = DeadList.Consume();

	// This shader very specifically emits particles in a grid shape
	// based on the index and a particular grid size, which is 
	// passed in from C++
	float3 gridPos;
	uint gridIndex = emitIndex;
	gridPos.x = gridIndex % (GridSize + 1);
	gridIndex /= (GridSize + 1);
	gridPos.y = gridIndex % (GridSize + 1);
	gridIndex /= (GridSize + 1);
	gridPos.z = gridIndex;

	// Update it in the particle pool
	Particle emitParticle = ParticlePool.Load(emitIndex);

	// Color and position depend on the grid position and size
	emitParticle.Color = float4(gridPos / GridSize, 1);
	emitParticle.Age = 0.0f;
	emitParticle.Position = (float3)Position;
	emitParticle.Size = 0.05f;
	emitParticle.Velocity = float3(0, 0, 0);
	emitParticle.Alive = 1.0f;

	// Put it back
	ParticlePool[emitIndex] = emitParticle;
}