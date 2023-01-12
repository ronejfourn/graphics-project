pushd extern

if [ ! -d "glfw" ]; then
    curl -LO "https://github.com/glfw/glfw/archive/refs/tags/3.3.8.tar.gz"
    tar -xf 3.3.8.tar.gz
    rm 3.3.8.tar.gz
    mv glfw-3.3.8 glfw
fi

popd


if [ ! -f "premake5" ]; then
    mkdir premake
    pushd premake
    curl -LO "https://github.com/premake/premake-core/releases/download/v5.0.0-beta2/premake-5.0.0-beta2-linux.tar.gz"
    tar -xf premake-5.0.0-beta2-linux.tar.gz
    mv premake5 ../
    popd
    rm -rf premake
fi

./premake5 gmake2
