if exist "build\" (
   rmdir /s /q "build"
)
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 -DMAYA_VERSION=2022 ..
cmake --build . --config Release


%(AdditionalOptions) 
/external:I "C:/Custom/Programs/Qt/5.15.2/msvc2019_64/include" 
/external:I "C:/Custom/Programs/Qt/5.15.2/msvc2019_64/include/QtCore" 
/external:I "C:/Custom/Programs/Qt/5.15.2/msvc2019_64/./mkspecs/win32-msvc" 
/external:I "C:/Custom/Programs/Qt/5.15.2/msvc2019_64/include/QtGui" 
/external:I "C:/Custom/Programs/Qt/5.15.2/msvc2019_64/include/QtANGLE" 
/external:I "C:/Custom/Programs/Qt/5.15.2/msvc2019_64/include/QtOpenGL" 
/external:I "C:/Custom/Programs/Qt/5.15.2/msvc2019_64/include/QtWidgets"