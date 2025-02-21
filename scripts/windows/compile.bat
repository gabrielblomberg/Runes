@echo off

:: Initialise the MSVC environment.
call "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Auxiliary/Build/vcvars64" || goto :error

:: %1 is the release or debug mode.
if "%1" NEQ "Debug" if "%1" NEQ "Release" (
    echo Pass either Debug or Release, not "%1" 
    exit /b 1
)

cd build
cmake --build . --config %1 || goto :error

cd ..
cmake --install build --config %1 --prefix install || goto :error

:: Success!
echo Success!
exit /b 0

:error
echo Failed!
exit /b %errorlevel%
