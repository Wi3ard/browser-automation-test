@echo off

:: Check build directory.
set BUILD_DIR=..\build\vc14-x64-release
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
pushd %BUILD_DIR%

:: Run CMake.
echo [*] CMake is running...
cmake ^
	-G"Visual Studio 14 2015 Win64" ^
	"-DCMAKE_CONFIGURATION_TYPES:STRING=Release" ^
	"-DQT_ROOT:PATH=%QT_ROOT%\msvc2015_64" ^
	"-DCPACK_GENERATOR:STRING=NSIS" ^
	../..
if errorlevel 1 goto :error

:: Exit
echo [*] Project generation succeeded!
popd
goto :eof

:: Error handling
:error
echo [!] ERROR: Failed to generate project - see above and correct.
popd
exit /b 1
