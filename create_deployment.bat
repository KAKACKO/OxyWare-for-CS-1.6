@echo off
setlocal

set "OUTPUT=deploy\OxyWare"
if not exist "%OUTPUT%" mkdir "%OUTPUT%"
if errorlevel 1 exit /b 1

for %%F in (OxyWare.exe cheater.dll oxui.dll util.dll) do (
    if not exist "build\%%F" exit /b 1
    copy /y "build\%%F" "%OUTPUT%\" >nul
    if errorlevel 1 exit /b 1
)

for %%F in (msvcp140.dll vcruntime140.dll) do (
    copy /y "dependencies\runtime\%%F" "%OUTPUT%\" >nul
    if errorlevel 1 exit /b 1
)

copy /y "dependencies\README.txt" "%OUTPUT%\" >nul
if errorlevel 1 exit /b 1
copy /y "LICENSE" "%OUTPUT%\" >nul
if errorlevel 1 exit /b 1
