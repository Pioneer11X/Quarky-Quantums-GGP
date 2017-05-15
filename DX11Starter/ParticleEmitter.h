#pragma once

#include <DirectXMath.h>

class ParticleSystem;

class ParticleEmitter
{
private:
	friend class ParticleSystem;

	ParticleEmitter(ParticleSystem* ps, uint32_t poolIdx, uint32_t emitterIdx);

	ParticleSystem* ps;
	uint32_t poolIdx;
	uint32_t emitterIdx;


public:
	void SetParameters(DirectX::XMFLOAT3 & position, DirectX::XMFLOAT3 & velocity, float lifeTime, float emitRate);
};