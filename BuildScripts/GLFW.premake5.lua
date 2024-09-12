local dir = mainDir .. "/ThirdParty/GLFW"
glfwIncludeDir = dir .. "/include"
glfwLibDir = dir .. "/lib"
glfwLibName = "glfw3"

project "GLFW"
	kind "StaticLib"
	language "C++"
	location(projDir)
	
	files
	{
        glfwIncludeDir .. "/**.h",
    }
