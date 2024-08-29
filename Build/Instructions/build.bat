cl.exe ^
/Fo"Build/x64/Debug/" ^
"./3d-engine-cpp/Dependencies/Implementations/*" ^
"./3d-engine-cpp/Source/Implementations/*.cpp" ^
glfw3_64.lib @Build\Instructions\msvc_libs.txt ^
/std:c++17 /Zi /I "3d-engine-cpp/Dependencies/Includes" ^
/I "3d-engine-cpp/Source/Headers" /Fd"Build\x64\Debug\vc143.pdb" ^
/MDd /EHsc /DEBUG /link "/LIBPATH:3d-engine-cpp/Dependencies/Libs" ^
/OUT:"Build\x64\Debug\3d-engine-cpp.exe"