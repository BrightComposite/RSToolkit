@echo off
cd ../../modules
echo -- Removing shader output and timestamps...
set errors=0

for /R %%f in (*.timestamp) do (
	( del /Q %%f ) && ( echo -- Successfully removed timestamp file %%f ) || ( set /a errors="%errors%+1" & echo !! Can't remove timestamp file %%f )
)

for /R %%f in (*.inc) do (
	( del /Q %%f ) && ( echo -- Successfully removed shader output %%f ) || ( set /a errors="%errors%+1" & echo !! Can't remove shader output %%f )
)

cd ../tools/shaders

if %errors% gtr 0 (
	echo -- %errors% errors occured while clearing. Maybe some files are opened by another applications ^(your IDE, for example^). Try again after closing all applications that can resist the Holy Power of Clearing!
) else (
	echo -- Looks clear! No errors occured while clearing.
)

pause