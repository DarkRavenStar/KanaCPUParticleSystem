#pragma once

#include <cmath>
#include <math.h>

//Scoped ImGui Style Handler
class ScopedImGuiColorAndStyle
{
	int m_PrevColorCount = 0;
	int m_PrevStyleCount = 0;

public:

	ScopedImGuiColorAndStyle()
	{
		ImGuiContext& g = *GImGui;
		m_PrevColorCount = g.ColorStack.size();
		m_PrevStyleCount = g.StyleVarStack.size();
	}

	~ScopedImGuiColorAndStyle()
	{
		ImGuiContext& g = *GImGui;
		int popColorCount = g.ColorStack.size() - m_PrevColorCount;
		int popStyleCount = g.StyleVarStack.size() - m_PrevStyleCount;

		assert(popColorCount >= 0 && popStyleCount >= 0);

		//DebugPrintInfo();

		ImGui::PopStyleColor(popColorCount);
		ImGui::PopStyleVar(popStyleCount);
	}

	void DebugPrintInfo()
	{
		ImGuiContext& g = *GImGui;
		int popColorCount = g.ColorStack.size() - m_PrevColorCount;
		int popStyleCount = g.StyleVarStack.size() - m_PrevStyleCount;

		printf("g.ColorStack: %d \n", g.ColorStack.size());
		printf("g.StyleVarStack: %d \n", g.StyleVarStack.size());

		printf("\n");

		printf("m_PrevColorCount: %d \n", m_PrevColorCount);
		printf("m_PrevStyleCount: %d \n", m_PrevStyleCount);

		printf("\n");

		printf("popColorCount: %d \n", popColorCount);
		printf("popStyleCount: %d \n", popStyleCount);

		printf("\n");
		printf("\n");
		printf("\n");
	}
};

namespace ImVec2Helper
{
    static inline ImVec2 SetMagnitude(const ImVec2& lhs, float val)
    {
        return lhs * (val / ImInvLength(lhs, val));
    }

	static inline ImVec2 Rotate(const ImVec2& lhs, float angle, bool isAngleInDegrees)
	{
		float angleInRadians = isAngleInDegrees ? angle * PI / 180.0f : angle;
		ImVec2 result;
		result.x = lhs.x * std::cos(angle) - lhs.y * std::sin(angle);
		result.y = lhs.x * std::sin(angle) + lhs.y * std::cos(angle);
		return result;
	}

    /*
    // Normalize the entire vector instead of getting a normalized copy
    ImVec2 Normalize(const ImVec2& lhs)
    {
        float invL = 1.0f / ImInvLength(lhs, 1.0f);;
        return lhs * ImVec2{invL, invL};
    }
    */	
}

