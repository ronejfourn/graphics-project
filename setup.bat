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

call .\premake5.exe vs2022
