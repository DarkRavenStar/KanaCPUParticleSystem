#pragma once
#include <vector>

//Forward Declaration
///////////////////////////////
template <typename... Types>
struct StorageView;

template <typename... Types>
struct SystemDataView;

struct AffectorSystemData;
struct RotationSystemData;
struct GravitySystemData;
struct ParticleRenderSystemData;

struct ParticleAccelerationComponent;
struct ParticleVelocityComponent;
struct ParticlePositionComponent;

struct ParticleLifeComponent;
struct ParticleColorComponent;

struct ParticleSpeedComponent;
struct ParticleScaleComponent;
struct ParticleRotationComponent;

struct ParticleLifeLerpComponent;
struct ParticleScaleLerpComponent;
struct ParticleColorLerpComponent;
///////////////////////////////

class System
{
public:
	bool m_Enabled = true;
};

namespace UnitTest
{
	void TestUpdate();
}

namespace SystemHelper
{
	//void 
}

namespace GameWorld
{
	class ParticleSpawnSystem : public System
	{
	public:
		//Spawn System is unique that it need access to storage
		//to create, recreate, remove or mark particles for recycling
		//Ideally it should be an ECB format but now its kept simple
		//using StorageDataAlias = StorageData<ParticleLifeData>;

		using StorageViewAlias = StorageView<ParticleLifeComponent>;

		using SystemDataViewAlias = SystemDataView<const AffectorSystemData>;

		void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
	};

	class LifeAffectorSystem : public System
	{
	public:
		using StorageViewAlias = StorageView<ParticleLifeLerpComponent, ParticleLifeComponent>;

		using SystemDataViewAlias = SystemDataView<>;

		void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
	};

	class ColorAffectorSystem : public System
	{
	public:
		using StorageViewAlias = StorageView
			<
			ParticleColorLerpComponent,
			const ParticleColorComponent,
			const ParticleLifeLerpComponent
			>;

		using SystemDataViewAlias = SystemDataView<const AffectorSystemData>;

		void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
	};

	class TransformAffectorSystem : public System
	{
	public:
		using StorageViewAlias = StorageView
			<
			ParticleAccelerationComponent,
			ParticleVelocityComponent,
			ParticlePositionComponent,
			const ParticleSpeedComponent
			>;

		using SystemDataViewAlias = SystemDataView<>;

		void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
	};

	//This system affect each particle rotation, not overall grand motion pattern
	class RotateAffectorSystem : public System
	{
	public:
		using StorageViewAlias = StorageView<ParticleRotationComponent, const ParticleVelocityComponent>;

		using SystemDataViewAlias = SystemDataView<const RotationSystemData>;

		void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
	};

	class GravityAffector : public System
	{
	public:
		using StorageViewAlias = StorageView<ParticleAccelerationComponent>;

		using SystemDataViewAlias = SystemDataView<GravitySystemData>;

		void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
	};

	class SpeedAffector : public System
	{
	public:
		using StorageViewAlias = StorageView<ParticleSpeedComponent, const ParticleLifeLerpComponent>;

		using SystemDataViewAlias = SystemDataView<const AffectorSystemData>;

		void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
	};

	class ScaleAffector : public System
	{
	public:
		using StorageViewAlias = StorageView<ParticleScaleLerpComponent, const ParticleScaleComponent, const ParticleLifeLerpComponent>;

		using SystemDataViewAlias = SystemDataView<const AffectorSystemData>;

		void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
	};

}

//Systems that process data to be rendered - might interface with ImGui
namespace RenderWorld
{
	class ParticleRenderSystem : public System
	{

	public:
		using StorageViewAlias = StorageView
			<
			const ParticlePositionComponent,
			const ParticleRotationComponent,
			const ParticleScaleLerpComponent,
			const ParticleColorLerpComponent
			>;

		using SystemDataViewAlias = SystemDataView<const ParticleRenderSystemData>;

		void Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
	};

	//One of the last system to be rendered along with GUI systems.
	//Interfaces with ImGui
	class PropertyPanelSystem : public System
	{
	public:
		using StorageViewAlias = StorageView
		<
			const ParticlePositionComponent,
			const ParticleRotationComponent,
			const ParticleScaleLerpComponent,
			const ParticleColorLerpComponent
		>;


		//ParticleSystemSingletonData, AffectorSystemData, GravitySystemData, RotationSystemData, EmissionData, SpiralSystemData, ParticleRenderSystemData, ParticleSpawnSystemData



		using SystemDataViewAlias = SystemDataView<const ParticleRenderSystemData>;

		void Update();// float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view);
	};
}