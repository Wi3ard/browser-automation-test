@echo off

call cmakegen-vc15-x64-release.cmd
if errorlevel 1 goto :error

set BUILD_DIR=..\build\vc15-x64-release
pushd %BUILD_DIR%

:: Run build.
echo [*] Build is running...
cmake --build . --config Release --target PACKAGE
if errorlevel 1 goto :error

:: Exit
echo [*] Project build succeeded!
popd
goto :eof

:: Error handling
:error
echo [!] ERROR: Failed to build project - see above and correct.
popd
exit /b 1
