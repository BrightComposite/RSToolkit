@echo off

echo ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
echo บ -------------- Rapture State Toolkit make tool -------------- บ
echo ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

if "%~1" EQU "all" (
	set build_vs_86="1"
	set build_vs_64="1"
	set build_cb="1"
	set build_clang="1"
) else if [%1] == [] (
	echo Make a build target. List of available targets:
	echo   vs^|vs32^|vs86 - for Visual Studio ^(x86^)
	echo   vs64         - for Visual Studio ^(x64^)
	echo   cb           - for Code Blocks
	echo   qt           - for Qt Creator
	echo   clang        - for Clang

	goto :end
) else (
	set build_vs_86="0"
	set build_vs_64="0"
	set build_qt="0"
	set build_cb="0"
	set build_clang="0"

	for %%I in (%*) do (
		       if "%%I" EQU "vs"  	(
			set build_vs_86="1"
		) else if "%%I" EQU "vs32" 	(
			set build_vs_86="1"
		) else if "%%I" EQU "vs86" 	(
			set build_vs_86="1"
		) else if "%%I" EQU "vs64" 	(
			set build_vs_64="1"
		) else if "%%I" EQU "qt" 	(
			set build_qt="1"
		) else if "%%I" EQU "cb" 	(
			set build_cb="1"
		) else if "%%I" EQU "clang" (
			set build_clang="1"
		) else (
			echo Unknown target: "%%I"
		)
	)
)

set here=%~dp0
cd ..

if %build_vs_86% EQU "1" (
	echo =================================================================
	echo == Making Visual Studio ^(x86^) version of Rapture State Toolkit ==
	echo =================================================================
	cmake -E make_directory build-vs-x86
	cd "build-vs-x86"
	cmake -G "Visual Studio 14 2015" ../modules
	cd ..
)

if %build_vs_64% EQU "1"  (
	echo =================================================================
	echo == Making Visual Studio ^(x64^) version of Rapture State Toolkit ==
	echo =================================================================
	cmake -E make_directory build-vs-x64
	cd "build-vs-x64"
	cmake -G "Visual Studio 14 2015 Win64" ../modules
	cd ..
)

if %build_cb% EQU "1"  (
	echo =================================================================
	echo ====== Making Code Blocks version of Rapture State Toolkit ======
	echo =================================================================
	cmake -E make_directory build-cb
	cd "build-cb"
	set CC=x86_64-w64-mingw32-gcc
	set CXX=x86_64-w64-mingw32-g++
	cmake -G "CodeBlocks - MinGW Makefiles" ../modules
	cd ..
)

if %build_qt% EQU "1"  (
	echo =================================================================
	echo ====== Making Qt Creator version of Rapture State Toolkit =======
	echo =================================================================
	cmake -E make_directory build-qt
	cd "build-qt"
	set CC=x86_64-w64-mingw32-gcc
	set CXX=x86_64-w64-mingw32-g++
	cmake -G "Ninja" ../modules
	cd ..
)

if %build_clang% EQU "1"  (
	echo =================================================================
	echo ========= Making Clang version of Rapture State Toolkit =========
	echo =================================================================
	cmake -E make_directory build-clang
	cd "build-clang"
	set CXX=cl
	cmake -G "Ninja" ../modules
	cd ..
)

cd %here%

:end
pause