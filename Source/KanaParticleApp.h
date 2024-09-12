#pragma once


struct GLFWwindow;
struct ImVec2;
struct ImGuiViewport;


class KanaParticleApp
{
public:
	KanaParticleApp(GLFWwindow* window);

    void Update(float deltaTime);
    void Render();

private:

	void RenderPropertyTab(ImVec2 propertyTabPos, ImVec2 propertyTabSize);
	void RenderSceneTab(const ImGuiViewport& mainViewport, float propertyTabWidth);
	void RenderParticleData();

	GLFWwindow* m_Window;
};