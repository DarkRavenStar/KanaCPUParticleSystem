#pragma once
#include<cstdlib>
#include <random>
#include "MathConstant.h"

static void maybeUseThis() {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(1.0, 10.0);

    //for (int i=0; i<16; ++i)
        //std::cout << dist(mt) << "\n";
}

struct ImVec2;
struct ParticleSpawnData;

//Mainly used by ParticleCreationSystem

//Maybe can convert to new 360 angle format
namespace EmitterHelper
{
	static inline ParticleSpawnData CircleEmitterSpawnData(const ImVec2& startPos, float radius);
	
	static inline ParticleSpawnData SquareEmitterSpawnData(const ImVec2& startPos, float radius);
	
	static inline ParticleSpawnData PointEmitterSpawnData(const ImVec2& startPos, float radius);
	
	static inline ParticleSpawnData RingEmitterSpawnData(const ImVec2& startPos, float innerRadius, float outerRadius);	
}

namespace MotionHelper
{
	static inline ImVec2 SetRandomMotion();
	static inline ImVec2 SetSpiralMotion(int numOfArms, float angleInbetween, bool autoAngleBetweenArms);
}

namespace SpawnHelper
{
	/*
	void ParticleSystem::UpdateEmitterShape()
	{
		if (psData.emitterShapeType == EmitterShapeType::NONE)
		{
			psData.emitterShape = NULL;
		}
		else if (psData.emitterShapeType == EmitterShapeType::CIRCLE)
		{
			psData.emitterShape = &psData.m_circleEmitter;
		}
		else if (psData.emitterShapeType == EmitterShapeType::SQUARE)
		{
			psData.emitterShape = &psData.m_squareEmitter;
		}
		else if (psData.emitterShapeType == EmitterShapeType::POINT)
		{
			psData.emitterShape = &psData.m_pointEmiter;
		}
		else if (psData.emitterShapeType == EmitterShapeType::RING)
		{
			psData.emitterShape = &psData.m_ringEmitter;
		}
	}
	*/
}