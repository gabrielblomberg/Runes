# Runes

A hexagonal competitive state based game implemented with C++ and
[SFML](https://www.sfml-dev.org/).

Resources:
- [Hexagonal Grid by RedBlobGames](https://www.redblobgames.com/grids/hexagons/)
- [SFML](https://www.sfml-dev.org/index.php)
- [SFML Reference](https://www.sfml-dev.org/documentation/2.5.1/group__graphics.php)

## Windows Setup

Compiling windows C++ applications requires the microsoft visual studio compiler
(mvsc) which is available as part of the [Build Tools for Visual Studio 2022](https://visualstudio.microsoft.com/downloads/).
This should be installed on your local machine. Select C++ Development for
Desktop and ensure the following are selected:

- Desktop development with C++
    - MSVC v143 - VS 2022 C++ 
    - Windows 11 SDK
    - C++ CMake tools for Windows
    - Testing tools core features - Build Tools
    - C++ AddressSanitzer

Once installed, **reboot**.

To verify that the windows SDK was properly installed, check that the file
`C:\Program Files (x86)\Windows Kits\10\Lib\<version>\um\x64\kernel32.Lib`
exists. If not, go to `Settings`, `Apps`, `Installed Apps` and search for
`Windows Software Development Kit`. Click the three dots, click `Modify`, then
click `Repair` on the installer.

The MSVC toolchain will not be added to path. The provided `Developer Command
Prompt for VS 2022`  application that comes with `BuildTools` can be used to
start a terminal with the correct environment variables. This can run by searching for `"Developer Command Prompt VS 2022"` in the windows search bar. Running `code .` in
this terminal will ensure every vscode terminal has access to the toolchain
tools. This is required for development on windows.
