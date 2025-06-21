@echo off

:: Register vcpkg on path.
set WORKSPACE=%cd%
set VCPKG_ROOT=%WORKSPACE%\vcpkg
set PATH=%VCPKG_ROOT%;%PATH%

:: Create build files.
(rd /S /Q build && mkdir build) || (mkdir build)
cd build

cmake ^
    -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" ^
    -DVCPKG_TARGET_TRIPLET=x64-windows-static ^
    -Wno-dev ^
    "%WORKSPACE%/src" ^
    || goto :error

:: Success!
echo Success!
exit /b 0

:error
echo Failed!
exit /b %errorlevel%
