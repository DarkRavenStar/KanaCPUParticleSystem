#include "EmitterShape.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

#include "Color.h"
#include "ParticleData.h"


ParticleSpawnData EmitterHelper::CircleEmitterSpawnData(const ImVec2& startPos, float radius)
{
	ParticleSpawnData spawnData;

	const float angle = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / c_MaxAngle);

	const float angleInRadians = c_ConvertToRadians ? angle * PI / 180.0f : angle;

	const float ranRadius = -radius + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (radius * 2.0f));
	const float triVelX = sin(angleInRadians);
	const float triVelY = cos(angleInRadians);

	spawnData.m_SpawnPos = startPos + ImVec2{ ranRadius * triVelX, ranRadius * triVelY };
	spawnData.m_SpawnAngle = angle;

	return spawnData;
}

ParticleSpawnData EmitterHelper::SquareEmitterSpawnData(const ImVec2& startPos, float radius)
{
	ParticleSpawnData spawnData;

	const float xRanRadius = -radius + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (radius * 2.0f));
	const float yRanRadius = -radius + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (radius * 2.0f));
	const ImVec2 pos = startPos + ImVec2{ xRanRadius, yRanRadius };

	const float correction = 90.0f * PI / 180.0f;
	const float angle = correction + atan2(pos.y, pos.x);

	spawnData.m_SpawnPos = pos;
	spawnData.m_SpawnAngle = angle;

	return spawnData;
}

ParticleSpawnData EmitterHelper::PointEmitterSpawnData(const ImVec2& startPos, float radius)
{
	ParticleSpawnData spawnData;

	spawnData.m_SpawnPos = startPos;
	spawnData.m_SpawnAngle = 0.0f;

	return spawnData;
}

ParticleSpawnData EmitterHelper::RingEmitterSpawnData(const ImVec2& startPos, float innerRadius, float outerRadius)
{
	ParticleSpawnData spawnData;

	const float angle = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / c_MaxAngle);
	
	const float angleInRadians = c_ConvertToRadians ? angle * PI / 180.0f : angle;

	const float ranRadius = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / ((outerRadius - innerRadius)));
	const float triVelX = sin(angleInRadians);
	const float triVelY = cos(angleInRadians);
	const ImVec2 angleVector = ImVec2{ (innerRadius + ranRadius) * triVelX, (innerRadius + ranRadius) * triVelY };
	const ImVec2 pos = startPos + angleVector;

	spawnData.m_SpawnPos = pos;
	spawnData.m_SpawnAngle = angle;

	return spawnData;
}

ImVec2 MotionHelper::SetRandomMotion()
{
	const float angle = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / c_MaxAngle);

	const float angleInRadians = c_ConvertToRadians ? angle * PI / 180.0f : angle;

	const float triVelX = sin(angleInRadians);
	const float triVelY = cos(angleInRadians);
	return ImVec2{ triVelX, triVelY };
}

//Maybe we can change this when we implement multi-threading
ImVec2 MotionHelper::SetSpiralMotion(int numOfArms, float angleInbetween, bool autoAngleBetweenArms)
{
	//Do we need the static?
	static float step = 0.0f;
	static float stepSpeed = 1.0f;

	const int randAngle = (rand() % numOfArms);
	

	//const float angle = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / c_MaxAngle);

	//const float angleInRadians = c_ConvertToRadians ? angle * PI / 180.0f : angle;

	//This angleBetween calculation got bugs or rather the intended outcome is not known
	//Leave this for now as it is
	const float angleBetween = angleInbetween / 360.0f;
	const float angleSteps = randAngle * (autoAngleBetweenArms ? (2 * PI / numOfArms) : (angleBetween * 2 * PI));
	
	const float angle = angleSteps + step;
	const float triVelX = sin(angle);
	const float triVelY = cos(angle);

	//Should we actually being using stepSpeed or deltaTime
	step += stepSpeed / 100.0f;
	
	return ImVec2{ triVelX, triVelY };
}
