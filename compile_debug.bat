
set COMPILATION_TYPE=Debug

mkdir build

cmake -DCOMPILATION_TYPE=%COMPILATION_TYPE% . -G "Visual Studio 16 2019" -A x64 -DCMAKE_CXX_FLAGS="/MP3 /EHsc /W3" -B"build\%COMPILATION_TYPE%"

cmake --build "build\%COMPILATION_TYPE%" --config %COMPILATION_TYPE%

copy build\%COMPILATION_TYPE%\%COMPILATION_TYPE%\Ray-it.exe .\Ray-it_%COMPILATION_TYPE%.exe

