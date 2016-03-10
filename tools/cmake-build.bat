@echo off
@cd %~dp0/..

@echo =================================================
@echo == Making x86 version of Rapture State Toolkit ==
@echo =================================================
@if not exist "build-x86" md "build-x86"
@cd "build-x86"
cmake -G "Visual Studio 14 2015" "../modules"
@cd ..

@echo =================================================
@echo == Making x64 version of Rapture State Toolkit ==
@echo =================================================
@if not exist "build-x64" md "build-x64"
@cd "build-x64"
cmake -G "Visual Studio 14 2015 Win64" "../modules"
@cd ..

@cd tools