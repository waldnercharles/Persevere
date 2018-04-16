@echo off

set "LLVMPath=C:/Program Files/LLVM"
set "LLVMVersion=6.0.0"

set "VSPath=C:/Program Files (x86)/Microsoft Visual Studio/2017/Community"
set "VSVersion=14.12.25827"

set "WinSDKPath=C:/Program Files (x86)/Windows Kits/10"
set "WinSDKVersion=10.0.16299.0"

set "VSBasePath=%VSPath%/VC/Tools/MSVC/%VSVersion%"
set "InitialPATH=%PATH%"
set "PATH=%PATH%;%LLVMPath%/bin;%VSBasePath%/bin/HostX64/x64"

set SYSINCLUDE=^
-imsvc"%VSBasePath%/include" ^
-imsvc"%WinSDKPath%/Include/%WinSDKVersion%/shared" ^
-imsvc"%WinSDKPath%/Include/%WinSDKVersion%/ucrt" ^
-imsvc"%WinSDKPath%/Include/%WinSDKVersion%/um"

set INCLUDE=-imsvcthird_party\include -Iinclude

set CFLAGS=%SYSINCLUDE% %INCLUDE% -O0 -MTd -fp:fast -fp:except- -GR- -EHa- -Zo -Oi -WX -W4 -FC -Z7 -Wno-deprecated-declarations
set LFLAGS=^
-machine:x64 ^
-nodefaultlib ^
-subsystem:console ^
-opt:ref ^
-incremental:no ^
-libpath:"%VSBasePath%/lib/x64" ^
-libpath:"%WinSDKPath%/Lib/%WinSDKVersion%/ucrt/x64" ^
-libpath:"%WinSDKPath%/Lib/%WinSDKVersion%/um/x64" ^
-libpath:"third_party/lib/x64" ^
libucrt.lib libvcruntime.lib libcmt.lib libcpmt.lib ^
legacy_stdio_definitions.lib oldnames.lib ^
legacy_stdio_wide_specifiers.lib ^
kernel32.lib User32.lib

set EXPORTS=^
-export:engine_init ^
-export:engine_bind ^
-export:engine_unbind ^
-export:engine_resize ^
-export:game_start ^
-export:game_loop

del bin\*.pdb > NUL 2>NUL
echo Locking... > bin\persevere-core.lock

@echo on

@clang-cl %CFLAGS% src\game.c -Febin\persevere-core.dll -LD -link %LFLAGS% -pdb:bin\persevere_core_%random%.pdb %EXPORTS% opengl32.lib glew32.lib
@clang-cl %CFLAGS% src\platform_sdl.c -Febin\persevere.exe -link %LFLAGS% SDL2.lib SDL2main.lib opengl32.lib glew32.lib

@echo off

del bin\persevere-core.lock

xcopy third_party\lib\x64\SDL2.dll bin /D /j /Y
xcopy assets bin\assets /D /E /i /Y

set "PATH=%InitialPATH%"
set InitialPATH=
