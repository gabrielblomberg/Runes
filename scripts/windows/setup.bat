:: Clear directories.
(rd /S /Q install && mkdir install) || (mkdir install)
(rd /S /Q build && mkdir build) || (mkdir build)
(rd /S /Q vcpkg && mkdir vcpkg) || (mkdir vcpkg)

:: Install vcpkg locally for dependencies.
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
call bootstrap-vcpkg.bat
cd ..

:: Set the root location for vcpkg.
set VCPKG_ROOT=%cd%\vcpkg
set PATH=%VCPKG_ROOT%;%PATH%

vcpkg install sfml:x64-windows-static
