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
