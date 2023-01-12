-- premake.lua
workspace "graphics-project"
  configurations {"debug", "release"}
  architecture "x86_64"

  filter "configurations:debug"
    symbols "On"
    defines "DEBUG"

  filter "configurations:release"
    optimize "On"
    defines "NDEBUG"

  filter {}

project "graphics-project"
  kind "ConsoleApp"
  language "C++"

  location "%{wks.location}/build"
  targetdir "%{prj.location}/%{cfg.buildcfg}"
  objdir "%{prj.location}/%{cfg.buildcfg}/obj"

  files { "**.cpp", "**.h", "extern/glad/glad.c" }
  warnings "Extra"

  includedirs { "%{wks.location}/extern/glad" }

  filter "not system:windows"
    links { "glfw" }

  filter "system:windows"
    includedirs { "%{wks.location}/extern/GLFW/include" }
    links { "glfw3.lib" }

  filter "action:vs2022"
    libdirs { "%{wks.location}/extern/GLFW/lib-vc2022" }

  filter {}
