cmake -G "Visual Studio 18 2026" -A x64 -S app/src/main/jni -B build
cmake --build build --config Debug
.\build\Debug\clshow.exe