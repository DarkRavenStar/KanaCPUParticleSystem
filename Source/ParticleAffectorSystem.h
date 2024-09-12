#pragma once
#include <vector>

template <typename... Types>
struct StorageView;

template <typename... Types>
struct SystemDataView;

struct AffectorSystemData;
struct RotationSystemData;
struct GravitySystemData;

struct ParticleAccelerationData;
struct ParticleVelocityData;
struct ParticlePositionData;

struct ParticleLifeData;
struct ParticleColorData;

struct ParticleSpeedData;
struct ParticleScaleData;
struct ParticleRotationData;

struct ParticleLifeLerpData;
struct ParticleScaleLerpData;
struct ParticleColorLerpData;

struct ParticleRenderSystemData;

class System
{
public:
	bool m_Enabled = true;
};

class ParticleSpawnSystem : public System
{
public:
	//Spawn System is unique that it need access to storage
	//to create, recreate, remove or mark particles for recycling
	//Ideally it should be an ECB format but now its kept simple
	//using StorageDataAlias = StorageData<ParticleLifeData>;

	using StorageViewAlias = StorageView<ParticleLifeData>;

	using SystemDataViewAlias = SystemDataView<const AffectorSystemData>;

	void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
};


class LifeAffectorSystem : public System
{
public:
	using StorageViewAlias = StorageView<ParticleLifeLerpData, ParticleLifeData>;

	using SystemDataViewAlias = SystemDataView<>;

	void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
};

class ColorAffectorSystem : public System
{
public:
	using StorageViewAlias = StorageView<ParticleColorLerpData, const ParticleColorData, const ParticleLifeLerpData>;

	using SystemDataViewAlias = SystemDataView<const AffectorSystemData>;

	void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
};

class TransformAffectorSystem : public System
{
public:
	using StorageViewAlias = StorageView<ParticleAccelerationData, ParticleVelocityData, ParticlePositionData, const ParticleSpeedData>;

	using SystemDataViewAlias = SystemDataView<>;

	void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
};

//This system affect each particle rotation, not overall grand motion pattern
class RotateAffectorSystem : public System
{
public:
	using StorageViewAlias = StorageView<ParticleRotationData, const ParticleVelocityData>;

	using SystemDataViewAlias = SystemDataView<const RotationSystemData>;

	void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
};

class GravityAffector : public System
{
public:	
	using StorageViewAlias = StorageView<ParticleAccelerationData>;
	
	using SystemDataViewAlias = SystemDataView<GravitySystemData>;

	void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
};

class SpeedAffector : public System
{
public:
	using StorageViewAlias = StorageView<ParticleSpeedData, const ParticleLifeLerpData>;

	using SystemDataViewAlias = SystemDataView<const AffectorSystemData>;

	void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
};

class ScaleAffector : public System
{
public:
	using StorageViewAlias = StorageView<ParticleScaleLerpData, const ParticleScaleData, const ParticleLifeLerpData> ;

	using SystemDataViewAlias = SystemDataView<const AffectorSystemData>;

	void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
};

namespace SystemHelper
{
	//void 
}

//Systems that process data to be rendered - might interface with ImGui
namespace RenderWorldSystem
{
	class ParticleRenderSystem : public System
	{

	public:
		using StorageViewAlias = StorageView
			<
			const ParticlePositionData,
			const ParticleRotationData,
			const ParticleScaleLerpData,
			const ParticleColorLerpData
			>;

		using SystemDataViewAlias = SystemDataView<const ParticleRenderSystemData>;

		void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
	};

	//One of the last system to be rendered along with GUI systems.
	//Interfaces with ImGui
	class PropertyPanelSystem : public System
	{
	public:
		using StorageViewAlias = StorageView <const ParticlePositionData, const ParticleRotationData, const ParticleScaleLerpData, const ParticleColorLerpData>;

		using SystemDataViewAlias = SystemDataView<const ParticleRenderSystemData>;

		void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
	};
}
