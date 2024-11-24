#pragma once

#include "ValueWrapper.h"

//Require Imgui headers when included
//Require Color.h header when included

enum class DemoType
{
	NO_DEMO,
	FIRE_WAVE,
	SPIRAL,
	FIRE_SPARKS,
	FIRE,
	FIRE_FAN,
	FALLING_SQUARE,
	BOKEH,
	FLOWER_TRIANGLE,
	FIRE_FLY
};

enum class EmitterShapeType
{
	NONE,
	SQUARE,
	CIRCLE,
	POINT,
	RING,
};

enum class DrawShape
{
	TRIANGLE,
	CIRCLE,
	SQUARE,
	STRIPES,
	RANDOM
};

enum class AffectorType
{
	GRAVITY,
	COLOR,
	ROTATION,
	SPEED,
	SCALE
};

enum class MotionType
{
	NONE,
	SPIRAL,
	RANDOM,
};

//Per Particle System Data/Singleton-like
///////////////////////////////////////////////////////////////////////


//Used by the system to set the initial data
//but can be reused
struct ParticleData
{
	int particleNum = 20;
	float lifetime = 60.0f;

};

struct ParticleSystemSingletonData
{
	DemoType m_DemoType = DemoType::NO_DEMO;
};

struct AffectorSystemData
{
	bool m_LerpColor = true;
	bool m_LerpSpeed = true;
	bool m_LerpScale = true;

	float m_InnerRadius = 10.0f;
	float m_OuterRadius = 10.0f;
};

struct GravitySystemData
{
	bool	m_UpdateSinePerParticle = true;

	bool	m_UseSine = false;
	bool	m_UseSineX = false;
	bool	m_UseSineY = false;
	float	m_SineSpeedX = 0.0f;
	float	m_SineSpeedY = 0.0f;

	float	m_SineX = 0.0f;
	float	m_SineY = 0.0f;

	float	m_AccelX = 0.0f;
	float	m_AccelY = 0.0f;
};

struct RotationSystemData
{
	bool	m_CustomRotation = false;
	bool	m_RotateLeft = false;
	bool	m_UseSine = false;
	float	m_RotationSpeed = 1.0f;
};



//Used by the system
struct EmissionData
{
	uint32_t m_EmissionRate = 1;
	uint32_t m_EmisionLimit = 1;
	uint32_t m_EmisionCount = 0;
};

struct SpiralSystemData
{
	bool autoAngleBetweenArms = true;
	int numOfArms = 4;
	float angleBetweenArms = 45.0f;
	float stepSpeed = 1.0f;
};


struct ParticleRenderSystemData
{
	bool		m_DrawLatestParticleFirst = false;
	DrawShape	m_DrawShape = DrawShape::STRIPES;
};

struct ParticleSpawnSystemData
{
	EmitterShapeType m_EmitterShapeType = EmitterShapeType::NONE;
};


/*
class ParticleSystemData
{
public:
	MotionType motionType = MotionType::SPIRAL;
	SpiralData spiralData;
	EmitterShape* emitterShape = NULL;
	EmitterShapeType emitterShapeType;

};
*/

///////////////////////////////////////////////////////////////////////


//Per Particle Entity Data
///////////////////////////////////////////////////////////////////////
struct ParticleSpawnData
{
	ImVec2	m_SpawnPos;
	float	m_SpawnAngle = 0.0f;
};

struct ParticleLifeData
{
	float m_CurLife = 0.0f;
	float m_MaxLife = 0.0f;
};

struct ParticleLifeLerpData : ValueWrapper<float>
{
	using ValueWrapper<float>::ValueWrapper;
};

struct ParticleScaleData
{
	float m_StartScale = 0.0f;
	float m_EndScale = 0.0f;
};

struct ParticleScaleLerpData : ValueWrapper<float>
{
	using ValueWrapper<float>::ValueWrapper;
};

struct ParticleColorData
{
	Color m_StartColor;
	Color m_EndColor;
};

struct ParticleColorLerpData : ValueWrapper<Color>
{
	using ValueWrapper<Color>::ValueWrapper;
};


struct ParticlePositionData : ValueWrapper<ImVec2>
{
	using ValueWrapper<ImVec2>::ValueWrapper;
};

struct ParticleVelocityData : ValueWrapper<ImVec2>
{
	using ValueWrapper<ImVec2>::ValueWrapper;
};

struct ParticleAccelerationData : ValueWrapper<ImVec2>
{
	using ValueWrapper<ImVec2>::ValueWrapper;
};

struct ParticleRotationData : ValueWrapper<float>
{
	using ValueWrapper<float>::ValueWrapper;
};

/*

struct ParticleRotationData
{
	float m_Angle = 0.0f;
};
struct ParticlePositionData
{
	ImVec2 m_Position;
};

struct ParticleVelocityData
{
	ImVec2 m_Velocity;
};

struct ParticleAccelerationData
{
	ImVec2 m_Acceleration;
};
*/

struct ParticleSpeedData
{
	float m_MaxSpeed = 0.0f;
	float m_StartSpeed = 0.0f;
	float m_EndSpeed = 0.0f;
};

///////////////////////////////////////////////////////////////////////