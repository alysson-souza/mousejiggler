@echo off
REM MouseJiggler C Rewrite - Windows Build Script
REM Requires MSVC compiler (cl.exe) to be in PATH

setlocal enabledelayedexpansion

REM Configuration
set SRC_DIR=..\src
set INCLUDE_DIR=..\include
set OBJ_DIR=..\obj
set BIN_DIR=..\bin

REM Create directories
if not exist %OBJ_DIR% mkdir %OBJ_DIR%
if not exist %BIN_DIR% mkdir %BIN_DIR%

REM Find and compile source files
set COMPILE_CMD=cl /W4 /O1 /GL /Gy /std:c11 /D_UNICODE /DUNICODE /I%INCLUDE_DIR%

echo Compiling MouseJiggler...

for /r %SRC_DIR% %%f in (*.c) do (
    echo Compiling %%f
    %COMPILE_CMD% /Fo%OBJ_DIR%\ /c "%%f"
    if errorlevel 1 (
        echo Compilation failed!
        exit /b 1
    )
)

REM Link object files
echo Linking...
set LINK_CMD=link /LTCG /OPT:REF /OPT:ICF /SUBSYSTEM:WINDOWS user32.lib gdi32.lib comctl32.lib shell32.lib advapi32.lib
%LINK_CMD% /OUT:%BIN_DIR%\mousejiggler.exe %OBJ_DIR%\*.obj

if errorlevel 1 (
    echo Linking failed!
    exit /b 1
)

echo Build successful!
echo Output: %BIN_DIR%\mousejiggler.exe

endlocal
