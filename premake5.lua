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
  cppdialect "C++17"

  location "%{wks.location}/build"
  targetdir "%{prj.location}/%{cfg.buildcfg}"
  objdir "%{prj.location}/%{cfg.buildcfg}/obj"

  files { "%{wks.location}/**.cpp", "%{wks.location}/**.hpp" }
  files { "%{wks.location}/**.c"  , "%{wks.location}/**.h"   }
  removefiles { "%{wks.location}/source/platform/specific/*" }
  includedirs { "%{wks.location}/source", "%{wks.location}/extern"}

  filter "system:windows"
    defines "PLATFORM_WIN32"
    defines "_CRT_SECURE_NO_WARNINGS"
    links {
      "User32.lib"  ,
      "Opengl32.lib",
      "Gdi32.lib"   ,
    }

  filter "system:linux"
    defines "PLATFORM_X11"
    links { "X11", "GLX", "dl" }

  filter {}
