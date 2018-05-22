cmake -E make_directory build

cd build

cmake -G "Visual Studio 12 2013 Win64" -DCMAKE_BUILD_TYPE=Release ..

cmake --build . --config Release

copy Release\RawViewer2.exe H:\DLKit\usr\RawViewer2\RawViewer2.exe /y

cd ..

REM cmake -E remove_directory build

pause
