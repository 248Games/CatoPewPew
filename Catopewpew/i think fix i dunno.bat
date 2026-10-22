@echo off
setlocal enabledelayedexpansion

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 ( echo vcvars64 not found & pause & exit /b 1 )

mkdir x64\Release 2>nul

echo Compiling cheat DLL...
cl /nologo /O2 /MD /EHsc /DUNICODE /D_UNICODE ^
   src\cheat\cheat.cpp src\cheat\renderer.cpp ^
   src\cheat\d3d9hook.cpp src\cheat\dllmain.cpp ^
   /Fe:x64\Release\catopewpew.dll ^
   /link d3d9.lib user32.lib gdi32.lib
set CL_EXIT=!errorlevel!

echo.
if !CL_EXIT! equ 0 ( echo [OK] Build succeeded: x64\Release\catopewpew.dll ) else ( echo [FAIL] cl exit code: !CL_EXIT! )
pause
