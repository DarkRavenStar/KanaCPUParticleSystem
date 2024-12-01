#include "ParticleAffectorSystem.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

#include "MathConstant.h"
#include "Color.h"
#include "ParticleData.h"
#include "Storage.h"
#include "ImGuiHelper.h"

#pragma optimize("", off)

struct SystemDataStructTest1
{
	float m_Speed = 0.0f;
};

struct SystemDataStructTest2
{
	int m_NumberOfPies = 0;
};

using StorageViewAliasCorrectTest = StorageView<const int, double>;
void TestFunctionCorrect(const StorageViewAliasCorrectTest& view)
{
	for (const auto& [lifeLerp, life] : view.GetAllSpan())
	{
		life += lifeLerp;
	}
}

using StorageViewAliasIncorrectTest = StorageView<const int, double, std::string>;
void TestFunctionIncorrect(const StorageViewAliasIncorrectTest& view)
{
	for (const auto& [lifeLerp, life, name] : view.GetAllSpan())
	{
		life += lifeLerp;
	}
}

using SystemDataViewAliasCorrectTest = SystemDataView<SystemDataStructTest1>;
void TestFunctionCorrectSystemData(const SystemDataViewAliasCorrectTest& view)
{
	SystemDataStructTest1& sdst1 = view.GetSystemData<SystemDataStructTest1>();
	sdst1.m_Speed += 10;
}

void UnitTest::TestUpdate()
{
	static bool doOnce = false;
	static StorageData<int, float, double> storage;

	if (!doOnce)
	{
		doOnce = true;

		const auto& [i, f, d] = storage.AddNewElement();

		i = 10;
		f = 15.0f;
		d = 20.0;
	}

	auto storageDataRef = storage.GetStorageReferences();

	//decltype(auto) tt = storage.ToStorageView(StorageViewAliasTest::CastTypeListAlias{});
	TestFunctionCorrect(storage);
	
	//using this will cause compile time error due std::string not being in the Storage
	//TestFunctionIncorrect(storage);

	static SystemData<SystemDataStructTest1, SystemDataStructTest2> systemData;

	auto systemDataRef = systemData.GetStorageReferences();

	//std::get<SystemDataStructTest1&>(systemDataRef).m_Speed += 10;
	const SystemDataStructTest1& hh = std::get<SystemDataStructTest1&>(systemDataRef);
	hh.m_Speed;

	std::get<SystemDataStructTest2&>(systemDataRef).m_NumberOfPies += 3;

	TestFunctionCorrectSystemData(systemData);

	printf("");
}


//Teena - Need to implement object pooling and chuck based storage if possible

//When implementing the rendering system, draw latest particle first, can be just using reverse forloop

//Idea for new affector system, lifetime sine use case

void GameWorld::ParticleSpawnSystem::Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view)
{

}


void GameWorld::LifeAffectorSystem::Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view)
{
	for (const auto& [lifeLerp, life] : view.GetAllSpan())
	{
		life.m_CurLife -= deltaTime;

		//Check if this is valid logic or we can just remove 1 -
		lifeLerp = 1.0f - (life.m_CurLife / life.m_MaxLife);
	}
}

void GameWorld::ColorAffectorSystem::Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view)
{
	const auto& asd = systemDataView.GetSystemData<const AffectorSystemData>();
	
	for (const auto& [colorLerp, color, lifeLerp] : view.GetAllSpan())
	{
		colorLerp = asd.m_LerpColor ? ColorHelper::LerpRGBA(color.m_StartColor, color.m_EndColor, lifeLerp) : color.m_StartColor;
	}	
}

void GameWorld::TransformAffectorSystem::Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view)
{
	for (const auto& [accel, vel, pos, speed] : view.GetAllSpan())
	{
		vel += accel * deltaTime;
		
		vel = ImVec2Helper::SetMagnitude(vel, speed.m_MaxSpeed);
		
		pos += vel * deltaTime;

		//Always reset
		accel = ImVec2{ 0.0f, 0.0f };
	}
}

void GameWorld::GravityAffector::Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view)
{
	auto& gsd = systemDataView.GetSystemData<GravitySystemData>();

	//Check if we really need the SineX/Y to be system data
	for (const auto& [accel] : view.GetAllSpan())
	{
		if (gsd.m_UseSine)
		{
			gsd.m_SineX += (gsd.m_UpdateSinePerParticle && gsd.m_UseSineX) ? gsd.m_SineSpeedX * deltaTime : -gsd.m_SineX;
			gsd.m_SineY += (gsd.m_UpdateSinePerParticle && gsd.m_UseSineY) ? gsd.m_SineSpeedY * deltaTime : -gsd.m_SineY;

			float x = gsd.m_UseSineX ? sin(gsd.m_SineX) : gsd.m_AccelX;
			float y = gsd.m_UseSineY ? sin(gsd.m_SineY) : gsd.m_AccelY;
			
			accel = ImVec2{ x, y };
		}
		else
		{
			accel = ImVec2{ 0.0f, 0.0f };
		}
	}
}

void GameWorld::RotateAffectorSystem::Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view)
{
	const auto& rsd = systemDataView.GetSystemData<const RotationSystemData>();

	for (const auto& [rot, vel] : view.GetAllSpan())
	{
		float angleInRadians = rot * PI / 180.0f;

		if (rsd.m_CustomRotation)
		{
			float speedInRadians = (rsd.m_RotationSpeed * deltaTime) * PI / 180.0f;

			angleInRadians += rsd.m_RotateLeft ? -speedInRadians : speedInRadians;
		}
		else
		{
			ImVec2 angleVector = vel;
			float correction = 90.0f * PI / 180.0f;
			angleInRadians = correction + atan2(angleVector.y, angleVector.x);

		}

		angleInRadians = rsd.m_UseSine ? sin(angleInRadians) : angleInRadians;

		//Normalize it before assigning it back
		angleInRadians = fmod(fmod(angleInRadians, 2.0f * PI) + 2.0f * PI, 2.0f * PI);

		rot = angleInRadians * 180.0f / PI;
	}
}

void GameWorld::SpeedAffector::Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view)
{
	const auto& asd = systemDataView.GetSystemData<const AffectorSystemData>();

	for (const auto& [speed, lifeLerp] : view.GetAllSpan())
	{
		speed.m_MaxSpeed = asd.m_LerpSpeed ? ImLerp<float>(speed.m_StartSpeed, speed.m_EndSpeed, lifeLerp) : speed.m_StartSpeed;
	}
}

void GameWorld::ScaleAffector::Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view)
{
	const auto& asd = systemDataView.GetSystemData<const AffectorSystemData>();

	for (const auto& [scaleLerp, scale, lifeLerp] : view.GetAllSpan())
	{
		scaleLerp = asd.m_LerpScale ? ImLerp<float>(scale.m_StartScale, scale.m_EndScale, lifeLerp) : scale.m_StartScale;
	}
}

void RenderWorld::PropertyPanelSystem::Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view)
{

}


void RenderWorld::ParticleRenderSystem::Update(float deltaTime, SystemDataViewAlias& systemDataView, StorageViewAlias& view)
{
	const auto& prsd = systemDataView.GetSystemData<const ParticleRenderSystemData>();
	
	//auto storageSpan = view.GetMultiSpan(StorageViewAlias::TypeListAlias{});
	//auto storageSpanView = ReverseRange(storageSpan);

	//if (prsd.m_DrawLatestParticleFirst) {}
	//else{}

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	if (!drawList)
	{
		return;
	}

	for (const auto& [pos, rot, scale, color] : view.GetAllSpan())
	{
		DrawShape shape = prsd.m_DrawShape == DrawShape::RANDOM ? static_cast<DrawShape>(rand() % 5) : prsd.m_DrawShape;

		float centerScale = scale * 0.5f;

		float cos_a = cosf(rot);
		float sin_a = sinf(rot);

		if (shape == DrawShape::CIRCLE)
		{
			drawList->AddCircleFilled(pos, scale, ImGui::ColorConvertFloat4ToU32(color));
		}
		else if (shape == DrawShape::TRIANGLE)
		{
			ImVec2 point1 = pos + ImRotate(ImVec2(0.0f, 0.0f), cos_a, sin_a); //top corner
			ImVec2 point2 = pos + ImRotate(ImVec2(+centerScale, -centerScale), cos_a, sin_a); //right corner
			ImVec2 point3 = pos + ImRotate(ImVec2(-centerScale, -centerScale), cos_a, sin_a); //left corner

			drawList->AddTriangleFilled(point1, point2, point3, ImGui::ColorConvertFloat4ToU32(color));
		}
		else if (shape == DrawShape::SQUARE)
		{
			ImVec2 point1 = pos + ImRotate(ImVec2(-centerScale, -centerScale), cos_a, sin_a); //left top corner
			ImVec2 point2 = pos + ImRotate(ImVec2(+centerScale, -centerScale), cos_a, sin_a); //right top corner
			ImVec2 point3 = pos + ImRotate(ImVec2(+centerScale, +centerScale), cos_a, sin_a); //right bottom corner
			ImVec2 point4 = pos + ImRotate(ImVec2(-centerScale, +centerScale), cos_a, sin_a); //left bottom corner
			
			drawList->AddQuad(point1, point2, point3, point4, ImGui::ColorConvertFloat4ToU32(color));
		}
		else if (shape == DrawShape::STRIPES)
		{
			float centerScaleY = centerScale;
			float centerScaleX = centerScale * 0.2f;

			ImVec2 point1 = pos + ImRotate(ImVec2(-centerScaleX, -centerScaleY), cos_a, sin_a); //left top corner
			ImVec2 point2 = pos + ImRotate(ImVec2(+centerScaleX, -centerScaleY), cos_a, sin_a); //right top corner
			ImVec2 point3 = pos + ImRotate(ImVec2(+centerScaleX, +centerScaleY), cos_a, sin_a); //right bottom corner
			ImVec2 point4 = pos + ImRotate(ImVec2(-centerScaleX, +centerScaleY), cos_a, sin_a); //left bottom corner

			drawList->AddQuad(point1, point2, point3, point4, ImGui::ColorConvertFloat4ToU32(color));
		}
	}
}