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

  local GLFW = "%{wks.location}/extern/glfw/src/"

  files { "**.cpp", "**.hpp" }
  files { "%{wks.location}/extern/glad/glad.c" }
  files {
    GLFW .. "context.c",
    GLFW .. "window.c",
    GLFW .. "monitor.c",
    GLFW .. "init.c",
    GLFW .. "input.c",
    GLFW .. "vulkan.c",
    GLFW .. "osmesa_context.c",
    GLFW .. "egl_context.c",
  }

  includedirs { "%{wks.location}/include" }
  includedirs { "%{wks.location}/extern/glad" }
  includedirs { "%{wks.location}/extern/glfw/include" }

  filter "system:windows"
    defines "_GLFW_WIN32"
    defines "_CRT_SECURE_NO_WARNINGS"
    files {
      GLFW .. "win32_thread.c",
      GLFW .. "win32_window.c",
      GLFW .. "win32_monitor.c",
      GLFW .. "win32_init.c",
      GLFW .. "win32_time.c",
      GLFW .. "wgl_context.c",
      GLFW .. "win32_joystick.c",
    }

  filter "system:linux"
    defines "_GLFW_X11"
    links "X11"
    files {
      GLFW .. "posix_thread.c",
      GLFW .. "x11_window.c",
      GLFW .. "x11_monitor.c",
      GLFW .. "x11_init.c",
      GLFW .. "posix_time.c",
      GLFW .. "glx_context.c",
      GLFW .. "linux_joystick.c",
      GLFW .. "xkb_unicode.c",
    }

  filter {}
