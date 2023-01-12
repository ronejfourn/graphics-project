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

if not exist "extern/" mkdir extern

pushd extern

if exist "GLFW/" goto haveGLFW
echo Downloading GLFW
call curl -LO "https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.bin.WIN64.zip"
call tar -xf glfw-3.3.8.bin.WIN64.zip
del /q glfw-3.3.8.bin.WIN64.zip
ren glfw-3.3.8.bin.WIN64 GLFW
echo Finished Downloading GLFW
:haveGLFW

popd extern

call premake5 vs2022
