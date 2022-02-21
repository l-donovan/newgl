set BUILD_DIR=_build
set DIST_DIR=dist
set VCPKG_DIR=%userprofile%\projects\vcpkg
set CONFIGURATION=RELEASE


mkdir %BUILD_DIR% %DIST_DIR%
cmake -B %BUILD_DIR% -S . -DCMAKE_TOOLCHAIN_FILE=%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1
"C:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin\MSBuild.exe" %BUILD_DIR%\NEWGL.sln /property:Configuration=Release
xcopy %BUILD_DIR%\src\%CONFIGURATION%\newgl.exe %DIST_DIR%\newgl.exe
xcopy %BUILD_DIR%\compile_commands.json .

pause