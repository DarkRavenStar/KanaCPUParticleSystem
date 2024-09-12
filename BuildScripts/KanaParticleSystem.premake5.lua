-- Include the helper functions
dofile("Helper.lua")

-- Get the current working directory
currentDir = os.getcwd()

-- Move up one directory by removing the last component of the path
mainDir = currentDir:match("(.*/)")
if mainDir then
    mainDir = mainDir:sub(1, -2)  -- Remove the trailing slash
else
    mainDir = currentDir
end

-- Print to verify
premake.info("Main Dir: " .. mainDir)
premake.info("Current Dir: " .. currentDir)

-- Define custom directories
-- Simplify this since testing on windows only
--outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
outputdir = ""

solutionDir = mainDir .. "/ProjectSetup"
targetDirMain = solutionDir .. "/Bin" .. outputdir .. "/%{prj.name}"
objDirMain = solutionDir .. "/Bin-Int" .. outputdir .. "/%{prj.name}"
projDir = solutionDir .. "/Projects/%{prj.name}"

workspace "Kana Particle System"
   architecture "x64"
   configurations
   {
      "Debug",
      "Release"
   }
   
   location(solutionDir)  -- Set the solution directory

   startproject "Kana Particle System"

   cppdialect "c++20"

   include "ZeroCheck.premake5.lua"
   include "Imgui.premake5.lua"
   include "GLFW.premake5.lua"
   include "Volk.premake5.lua"
   include "Vulkan.premake5.lua"

   -- Do this globally unless need per project overide
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "RELEASE" }
      optimize "On"

   filter "system:windows"
      systemversion "latest"
      staticruntime "On"
      
		

project "Kana Particle System"
   kind "ConsoleApp"
   language "C++"
   location(projDir)
   targetdir (targetDirMain)
   objdir (objDirMain)

   -- Include the files in the project
   addFilesWithFilters
   (
      {
         mainDir .. "/Source" .. "/**.cpp",
         mainDir .. "/Source" .. "/**.h",
      },
      {
         mainDir .. "/Source" .. "/TestMain.cpp",
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
	   imguiLibName,
	   volkLibName,
	   vulkanLibName,
	}

	defines
	{
		"IMGUI_IMPL_VULKAN_USE_VOLK",
		"VOLK_IMPLEMENTATION",
      "VK_USE_PLATFORM_WIN32_KHR",
		"VK_NO_PROTOTYPES",
		"GLFW_INCLUDE_NONE",
		"GLFW_INCLUDE_VULKAN"
	}

   dependson { "ZeroCheck" }
 