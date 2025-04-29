@echo off

setlocal enabledelayedexpansion

gcc bpad.c -o bpad
if errorlevel 1 exit /b %errorlevel%

bpad %*
if errorlevel 1 exit /b %errorlevel%