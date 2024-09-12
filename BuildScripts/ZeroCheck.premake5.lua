local scriptDir = mainDir
local checkScript = path.join(scriptDir, "Setup.bat")

project "ZeroCheck"
	kind "ConsoleApp"
	language "C"
	location(projDir)
	targetdir (targetDirMain)
	objdir (objDirMain)
	

   buildcommands
   {
      "{ECHO} Solution regen...",
      "cmd /c \"" .. checkScript .. "\"",
   }

   buildoutputs { "Solution Regen Completed" }