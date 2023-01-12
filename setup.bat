@echo off

if exist "premake5.exe" goto havePremake
mkdir premake
pushd premake
call curl -LO "https://github.com/premake/premake-core/releases/download/v5.0.0-beta2/premake-5.0.0-beta2-windows.zip"
call tar -xf premake-5.0.0-beta2-windows.zip
move premake5.exe ..\
popd
rmdir /Q /S premake
:havePremake

pushd extern

if exist "glfw/" goto haveGLFW
call curl -LO "https://github.com/glfw/glfw/archive/refs/tags/3.3.8.tar.gz"
call tar -xf 3.3.8.tar.gz
del 3.3.8.tar.gz
ren glfw-3.3.8 glfw
:haveGLFW

popd extern

call .\premake5.exe vs2022
