@echo off
echo Compiling...

cd ../src && ^
dir /s /b | ^
findstr /R .*\.cpp$ > ../source.txt && cd ..

@REM gcc @file is terrible and ignores backslashes. Replace all '\' with '\\'.
@REM This also means we can't pipe files from findstr into gcc.
powershell -c "(gc source.txt) -replace '\\', '\\' | Out-File -encoding ASCII 'source.txt'"
powershell -c "(gc source.txt) -replace ' ', '\ ' | Out-File -encoding ASCII 'source.txt'"

if "%~1"=="debug" (
    g++ -std=c++2a @source.txt -g -o bin/runes.exe -static -static-libgcc ^
    -Isrc -Ilib/SFML/include -Llib/SFML/lib -DSFML_STATIC ^
    -lsfml-graphics-s-d -lsfml-window-s-d -lsfml-network-s-d -lsfml-system-s-d ^
    -lopengl32 -lfreetype -lwinmm -lgdi32 -lws2_32 -lwinmm^
    -Wall -Werror -Wpedantic
) else if "%~1"=="release" (
    g++ -std=c++2a @source.txt -o bin/runes.exe -static -static-libgcc ^
    -Isrc -Ilib/SFML/include -Llib/SFML/lib -DSFML_STATIC ^
    -lsfml-graphics-s -lsfml-window-s -lsfml-network-s -lsfml-system-s ^
    -lopengl32 -lfreetype -lwinmm -lgdi32 -lws2_32 -lwinmm -mwindows ^
    -Wall -Werror -Wpedantic
) else (
    echo No build specified.
    exit 1
)

if %errorlevel%==0 (echo Success!) else (echo Failed!)

exit %errorlevel%
