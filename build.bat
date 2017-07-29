@echo off

SET CC="gcc"

SET CFLAGS=-Wno-long-long -Wno-variadic-macros -fexceptions -std=c11 -pedantic

SET SYSINCLUDE=-Ic:/users/charles.waldner/projects/persevere/include/sdl2 -Ic:\mingw\bin\../lib/gcc/mingw32/5.3.0/include -Ic:\mingw\bin\../lib/gcc/mingw32/5.3.0/../../../../include -Ic:\mingw\bin\../lib/gcc/mingw32/5.3.0/include-fixed


SET SDLPATH=C:\MingW\SDL2-2.0.5\i686-w64-mingw32

if not exist .\bin mkdir .\bin
pushd .\bin

del *.pdb > NUL 2> NUL

echo "WAITING FOR COMPILE ..." > persevere-core.lock
%CC% %CFLAGS% %SYSINCLUDE% -I%SDLPATH%\include\SDL2 -shared ..\src\game_core.c -g -o persevere-core.dll -L%SDLPATH%\lib -lSDL2
del persevere-core.lock

%CC% %CFLAGS% %SYSINCLUDE% -I%SDLPATH%\include\SDL2 ..\src\platform.c -g -o persevere.exe -L%SDLPATH%\lib -lm -lSDL2 -lSDL2main -Wl,-subsystem:console
xcopy /D /q "%SDLPATH%\bin\*.dll" .
xcopy /D /q "C:\MingW\bin\*.dll" .
popd

echo Done!
