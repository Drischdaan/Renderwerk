@echo off

set arg1=%1

pushd %~dp0

python --version > NUL
if errorlevel 1 goto errorNoPython

python .\Scripts\Launch.py %arg1%

goto:eof

:errorNoPython
echo.
echo Error: Python not installed

PAUSE
