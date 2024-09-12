local dir = mainDir .. "/ThirdParty/vulkan"

vulkanIncludeDir = dir .. "/Include"
vulkanLibDir = dir .. "/Lib"
vulkanLibName = "vulkan-1"

vulkanDLLDir = dir .. "/DLL/vulkan-1.dll"

project "Vulkan"
	kind "SharedLib"
	language "C"
	location(projDir)
	
	files
	{
        dir .. "/**.h",
    }