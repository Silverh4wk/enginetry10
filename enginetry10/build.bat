@echo off
set "PROJECT_ROOT=C:\Users\silver\source\repos\enginetry10"
set "BUILD_DIR=%PROJECT_ROOT%\build"

IF NOT EXIST %BUILD_DIR% mkdir ..\..\Build
pushd "%BUILD_DIR%"
cl -DGAME_INTERNAL=1  -FC -Zi "%PROJECT_ROOT%\enginetry10\win32Call.cpp" ^
             "%PROJECT_ROOT%\enginetry10\GameCode.cpp" ^
             "%PROJECT_ROOT%\enginetry10\XAudioSoundClass.cpp" ^
             /link user32.lib gdi32.lib ole32.lib xaudio2.lib
popd
