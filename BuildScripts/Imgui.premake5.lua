dofile("Helper.lua")

include "GLFW.premake5.lua"
include "Volk.premake5.lua"
include "Vulkan.premake5.lua"

local dir = mainDir .. "/ThirdParty/Imgui"
imguiIncludeDir = dir
imguiLibName = "ImGui"

project "ImGui"
	kind "StaticLib"
	language "C++"
	location(projDir)
	targetdir (targetDirMain)
	objdir (objDirMain)
	cppdialect "c++17"
	
	addFilesWithFilters
	(
	   {
			dir .. "/*.cpp", 
			dir .. "/*.h",
			dir .. "/backends/imgui_impl_glfw.h",
			dir .. "/backends/imgui_impl_glfw.cpp",
			dir .. "/backends/imgui_impl_vulkan.h",
			dir .. "/backends/imgui_impl_vulkan.cpp",
	   },
	   {
	   }
	)

	includedirs
	{
	   glfwIncludeDir,
	   imguiIncludeDir,
	   volkIncludeDir,
	   vulkanIncludeDir,
	}
	
	libdirs
	{
	   glfwLibDir,
	   vulkanLibDir,
	}
	
	links
	{
	   glfwLibName,
	   volkLibName,
	   vulkanLibName,
	}

	defines
	{
		"IMGUI_IMPL_VULKAN_USE_VOLK",
		"VK_USE_PLATFORM_WIN32_KHR"
	}