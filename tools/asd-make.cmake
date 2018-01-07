#--------------------------------------------------------
#	ASD make facilities
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

include(tools/workspace.cmake)

#--------------------------------------------------------

message("")

color_message(BLUE "= ASD MAKE TOOL ")

message("")

#--------------------------------------------------------

option(BUILD_TARGET "BUILD_TARGET" OFF)

if("${BUILD_TARGET}" STREQUAL "OFF" OR "${BUILD_TARGET}" STREQUAL "")
	message("  Available targets:")
	message("  * vs|vs32|vs86 				- Visual Studio 2017 (x86)")
	message("  * vs64         				- Visual Studio 2017 (x64)")
	message("  * vs2015|vs2015-32|vs2015-86 - Visual Studio 2015 (x86)")
	message("  * vs2015-64    				- Visual Studio 2015 (x64)")
	message("  * cb|code-blocks     		- Code Blocks")
	message("  * qt           				- Qt Creator")
	message("  * clang       				- Clang")
else()
	message("- ASD is at ${ASD_ROOT}")
	message("- Make specified targets...")
	message("")

	set(OPTIONS)

	foreach(cmd ${BUILD_TARGET})
		if("${cmd}" STREQUAL "vs" OR "${cmd}" STREQUAL "vs32" OR "${cmd}" STREQUAL "vs86")
			set(TARGET_NAME "Visual Studio 2017 (x86)")
			set(TARGET_SUFFIX "msvc-2017-x86")
			set(TARGET_GENERATOR "Visual Studio 15 2017")
		elseif("${cmd}" STREQUAL "vs64")
			set(TARGET_NAME "Visual Studio 2017 (x64)")
			set(TARGET_SUFFIX "msvc-2017-x64")
			set(TARGET_GENERATOR "Visual Studio 15 2017 Win64")
		elseif("${cmd}" STREQUAL "vs2015" OR "${cmd}" STREQUAL "vs2015-32" OR "${cmd}" STREQUAL "vs2015-86")
			set(TARGET_NAME "Visual Studio 2015 (x86)")
			set(TARGET_SUFFIX "msvc-2015-x86")
			set(TARGET_GENERATOR "Visual Studio 14 2015")
		elseif("${cmd}" STREQUAL "vs2015-64")
			set(TARGET_NAME "Visual Studio 2015 (x64)")
			set(TARGET_SUFFIX "msvc-2015-x64")
			set(TARGET_GENERATOR "Visual Studio 14 2015 Win64")
		elseif("${cmd}" STREQUAL "cb" OR "${cmd}" STREQUAL "code-blocks")
			set(OPTIONS "-DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ -DCMAKE_CC_COMPILER=x86_64-w64-mingw32-gcc")
			set(TARGET_NAME "Code Blocks")
			set(TARGET_SUFFIX "code-blocks")
			set(TARGET_GENERATOR "CodeBlocks - Unix Makefiles")
		elseif("${cmd}" STREQUAL "qt")
			set(OPTIONS "-DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ -DCMAKE_CC_COMPILER=x86_64-w64-mingw32-gcc")
			set(TARGET_NAME "Qt Creator")
			set(TARGET_SUFFIX "ninja")
			set(TARGET_GENERATOR "Ninja")
		elseif("${cmd}" STREQUAL "clang")
			set(OPTIONS "-DCMAKE_CXX_COMPILER=cl")
			set(TARGET_NAME "Clang")
			set(TARGET_SUFFIX "clang")
			set(TARGET_GENERATOR "Ninja")
		else()
			message(WARNING "Unknown target: \"${cmd}\"")
			continue()
		endif()

		color_message(YELLOW "== Making ${TARGET_NAME} version of ASD ==")

		execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${ASD_ROOT}/build/${TARGET_SUFFIX}")
		execute_process(COMMAND ${CMAKE_COMMAND} -G "${TARGET_GENERATOR}" -DMAKE_TARGET_KEY="${BUILD_TARGET}" -DMAKE_OPTIONS="${OPTIONS}" -DTARGET_SUFFIX=${TARGET_SUFFIX} ${OPTIONS} -B${ASD_ROOT}/build/${TARGET_SUFFIX} -H${ASD_ROOT})
	endforeach()
endif()
