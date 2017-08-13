#--------------------------------------------------------
#	ASD make facilities
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

include(tools/workspace.cmake)

#--------------------------------------------------------

message("")

if(0)
	color_message(BLUE "  ษอออออออออออออออออออออออออออออออออออออป")
	color_message(BLUE "  บ         ฺฤฤฤฤฤฺฤฤฤฤฤฟฤฤฤฤฤฟ         บ")
	color_message(BLUE "  บ         ณ ฺฤฤฤณ  อออตฤฤฤฟ ณ         บ")
	color_message(BLUE "  บ         ณ ณฤฟ ฦอออ  ณฤฤฤู ณ         บ")
	color_message(BLUE "  บ         ภฤู ภฤภฤฤฤฤฤูฤฤฤฤฤู         บ")
	color_message(BLUE "  ศอออออออออออออ MAKE TOOL อออออออออออออผ")
elseif(0)
	color_message(BLUE "   อออออออออออออออออออออออออออออออออออออ ")
	color_message(BLUE "            ฺฤฤฤฤฤฺฤฤฤฤฤฟฤฤฤฤฤฟ          ")
	color_message(BLUE "            ณ ฺฤฤฤณ  อออตฤฤฤฟ ณ          ")
	color_message(BLUE "            ณ ณฤฟ ฦอออ  ณฤฤฤู ณ          ")
	color_message(BLUE "            ภฤู ภฤภฤฤฤฤฤูฤฤฤฤฤู          ")
	color_message(BLUE "   อออออออออออออ MAKE TOOL อออออออออออออ ")
elseif(1)
	color_message(BLUE "                     ฺฤฤฤฤฤฟ  ฺฤฤฤฤฤฟ  ฺฤฤฤฤฤฟ       ")
	color_message(BLUE "                     ณ ฺฤยฤด  ณ  ฤฤฤด  รฤยฤฟ ณ       ")
	color_message(BLUE "                     ณ รฤด ณ  รฤฤฤ  ณ  ณ ภฤู ณ       ")
	color_message(BLUE "                     ภฤู ภฤู  ภฤฤฤฤฤู  ภฤฤฤฤฤู       ")
	color_message(BLUE "               อออออออออออออออออออออออออออออออออออออ ")
	color_message(BLUE "                             MAKE TOOL               ")
else()
	color_message(BLUE "              ษอออออออออป ษอออออออออป ษอออออออออป")
	color_message(BLUE "              บ ฺฤฤฤฤฤฟ บ บ ฺฤฤฤฤฤฟ บ บ ฺฤฤฤฤฤฟ บ")
	color_message(BLUE "              บ ณ ฺฤยฤด บ บ ณ  ฤฤฤด บ บ รฤยฤฟ ณ บ")
	color_message(BLUE "              บ ณ รฤด ณ บ บ รฤฤฤ  ณ บ บ ณ ภฤู ณ บ")
	color_message(BLUE "              บ ภฤู ภฤู บ บ ภฤฤฤฤฤู บ บ ภฤฤฤฤฤู บ")
	color_message(BLUE "              ศอออออออออผ ศอออออออออผ ศอออออออออผ")
	color_message(BLUE "                           MAKE TOOL             ")
endif()

message("")

#--------------------------------------------------------

option(BUILD_TARGET "BUILD_TARGET" OFF)

if("${BUILD_TARGET}" STREQUAL "OFF" OR "${BUILD_TARGET}" STREQUAL "")
	message("  Available targets:")
	message("  * vs|vs32|vs86 - for Visual Studio (x86)")
	message("  * vs64         - for Visual Studio (x64)")
	message("  * cb           - for Code Blocks")
	message("  * qt           - for Qt Creator")
	message("  * clang        - for Clang")
else()
	message("- ASD Toolkit is at ${ASD_ROOT}")
	message("- Make specified targets...")
	message("")

	set(OPTIONS)

	foreach(cmd ${BUILD_TARGET})
		if("${cmd}" STREQUAL "vs" OR "${cmd}" STREQUAL "vs32" OR "${cmd}" STREQUAL "vs86")
			set(TARGET_NAME "Visual Studio (x86)")
			set(TARGET_DIR "build-vs-x86")
			set(TARGET_GENERATOR "Visual Studio 14 2015")
		elseif("${cmd}" STREQUAL "vs64")
			set(TARGET_NAME "Visual Studio (x64)")
			set(TARGET_DIR "build-vs-x64")
			set(TARGET_GENERATOR "Visual Studio 14 2015 Win64")
		elseif("${cmd}" STREQUAL "cb")
			set(OPTIONS "-DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ -DCMAKE_CC_COMPILER=x86_64-w64-mingw32-gcc")
			set(TARGET_NAME "Code Blocks")
			set(TARGET_DIR "build-cb")
			set(TARGET_GENERATOR "CodeBlocks - Unix Makefiles")
		elseif("${cmd}" STREQUAL "qt")
			set(OPTIONS "-DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ -DCMAKE_CC_COMPILER=x86_64-w64-mingw32-gcc")
			set(TARGET_NAME "Qt Creator")
			set(TARGET_DIR "build-qt")
			set(TARGET_GENERATOR "Ninja")
		elseif("${cmd}" STREQUAL "clang")
			set(OPTIONS "-DCMAKE_CXX_COMPILER=cl")
			set(TARGET_NAME "Clang")
			set(TARGET_DIR "build-clang")
			set(TARGET_GENERATOR "Ninja")
		else()
			message(WARNING "Unknown target: \"${cmd}\"")
			continue()
		endif()

		color_message(YELLOW "  == Making ${TARGET_NAME} version of ASD ==")

		execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${ASD_ROOT}/${TARGET_DIR}")
		execute_process(COMMAND ${CMAKE_COMMAND} -G "${TARGET_GENERATOR}" -DMAKE_TARGET_GENERATOR="${TARGET_GENERATOR}" -DMAKE_TARGET_KEY="${BUILD_TARGET}" -DMAKE_OPTIONS="${OPTIONS}" -DMAKE_TARGET_DIR=${TARGET_DIR} ${OPTIONS} -B${ASD_ROOT}/${TARGET_DIR} -H${ASD_ROOT})
	endforeach()
endif()