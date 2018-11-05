choco install -y visualstudio2017-workload-vctools
choco install -y boost-msvc-12

echo "Looking for preinstalled Boost"
rem $env:PATH += ";$BOOST_ROOT"
rem $env:PATH += ";$BOOST_LIBRARYDIR"

md build
cd build

cmake -G "Visual Studio 14 2015 Win64" -DBUILD_STATIC="ON" -DBOOST_ROOT="$BOOST_ROOT" -DBOOST_LIBRARYDIR="$BOOST_LIBRARYDIR" -DCMAKE_BUILD_TYPE=DEBUG -DBoost_COMPILER="-vc141" -DLOGGING="ON" ..
cmake --build . --config Debug --target lib_carl
cmake --build . --config Debug --target lib_carl_static
cmake --build . --config Debug --target all-tests
.\bin\Debug\runCoreTests.exe
cmake --build . --config Debug --target RUN_TESTS
