cd ..

(rd /S /Q bin && mkdir bin) || mkdir bin
(rd /S /Q lib && mkdir lib) || mkdir lib

powershell -c "Invoke-WebRequest -Uri 'https://www.sfml-dev.org/files/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit.zip' -OutFile 'SFML.zip'"
powershell -c "Expand-Archive SFML.zip lib"
powershell -c "Rename-Item lib/SFML-2.5.1 SFML"

del SFML.zip
