@echo off

SET CC="g++"

SET CFLAGS=-Wno-long-long -Wno-variadic-macros -fexceptions -std=c++11 -pedantic

REM -target i686-w32-windows-gnu -Xclang -flto-visibility-public-std

SET SYSINCLUDE=-Ic:/users/charles.waldner/projects/persevere/include/sdl2 -Ic:\mingw\bin\../lib/gcc/mingw32/5.3.0/include/c++ -Ic:\mingw\bin\../lib/gcc/mingw32/5.3.0/include/c++/mingw32 -Ic:\mingw\bin\../lib/gcc/mingw32/5.3.0/include/c++/backward -Ic:\mingw\bin\../lib/gcc/mingw32/5.3.0/include -Ic:\mingw\bin\../lib/gcc/mingw32/5.3.0/../../../../include -Ic:\mingw\bin\../lib/gcc/mingw32/5.3.0/include-fixed

SET SDLPATH=C:\MingW\SDL2-2.0.5\i686-w64-mingw32

if not exist .\bin mkdir .\bin
pushd .\bin

del *.pdb > NUL 2> NUL

echo "WAITING FOR COMPILE ..." > persevere-core.lock
%CC% %CFLAGS% -I%SDLPATH%\include\SDL2 -shared ../src/game_core.cpp -o persevere-core.dll -L%SDLPATH%\lib -lSDL2 -lSDL2main 
del persevere-core.lock

%CC% %CFLAGS% -I%SDLPATH%\include\SDL2 ../src/platform.cpp -g -o persevere.exe -L%SDLPATH%\lib -lm -lSDL2 -lSDL2main -Wl,-subsystem:console
xcopy /D /q "%SDLPATH%\bin\*.dll" .
xcopy /D /q "C:\MingW\bin\*.dll" .
popd

echo Done!
