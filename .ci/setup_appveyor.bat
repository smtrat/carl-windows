git config --global user.email "gereon.kremer@cs.rwth-aachen.de"
git config --global user.name "Travis builder"

choco install -y visualstudio2017-workload-vctools

md build
cd build

cmake -G "Visual Studio 15 2017 Win64" -DBUILD_STATIC="ON" -DCMAKE_BUILD_TYPE=DEBUG -DLOGGING="ON" ..
cmake --build . --config Debug --target Eigen3-EP
cmake --build . --config Debug --target GMP-EP
cmake --build . --config Debug --target GTest-EP
cmake --build . --config Debug --target Boost-EP
cmake --build . --config Debug --target lib_carl
cmake --build . --config Debug --target lib_carl_static
cmake --build . --config Debug --target all-tests
.\bin\Debug\runCoreTests.exe
cmake --build . --config Debug --target RUN_TESTS
