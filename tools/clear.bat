@echo off
cd ..
echo -- Removing cmake build directories and projects...
set errors=0

if exist "build-x86" (
	( rd /S /Q "build-x86" ) && ( echo -- Successfully removed build directory for x86 architecture ) || ( set /a errors="%errors%+1" & echo !! Can't remove build directory for x86 architecture )
)

if exist "build-x64" (
	( rd /S /Q "build-x64" ) && ( echo -- Successfully removed build directory for x64 architecture ) || ( set /a errors="%errors%+1" & echo !! Can't remove build directory for x64 architecture )
)

if exist "bin" (
	( rd /S /Q "bin" ) && ( echo -- Successfully cleared executable output ) || ( set /a errors="%errors%+1" & echo !! Can't clear executable output )
)

if exist "lib" (
	( rd /S /Q "lib" ) && ( echo -- Successfully cleared library output) || ( set /a errors="%errors%+1" & echo !! Can't clear library output )
)

cd tools

if %errors% gtr 0 (
	echo -- %errors% errors occured while clearing. Maybe some files are opened by another applications ^(your IDE, for example^). Try again after closing all applications that can resist the Holy Power of Clearing!
) else (
	echo -- Looks clear! No errors occured while clearing.
)

pause