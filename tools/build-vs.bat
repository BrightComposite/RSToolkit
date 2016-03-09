@echo off

@set arg="%~1"
		
@if %arg% == "" (
	@set arg="%VS140COMNTOOLS%"
) else (
	@if %arg% == "--build" (
		@goto :build
	)
)

%comspec% /k "%arg%"VsMSBuildCmd" & build-vs --build"
@exit

:build
call cmake-build
@cd "../build-x86"
msbuild ALL_BUILD.vcxproj /p:Configuration=Release
msbuild ALL_BUILD.vcxproj /p:Configuration=Debug
@cd "../build-x64"
msbuild ALL_BUILD.vcxproj /p:Configuration=Release
msbuild ALL_BUILD.vcxproj /p:Configuration=Debug
@cd "../tools"

@pause
@exit