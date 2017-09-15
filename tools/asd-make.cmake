#--------------------------------------------------------
#	ASD make facilities
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

include(tools/workspace.cmake)

#--------------------------------------------------------

message("")

if(0)
	color_message(BLUE "  ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»")
	color_message(BLUE "  º         ÚÄÄÄÄÄÚÄÄÄÄÄ¿ÄÄÄÄÄ¿         º")
	color_message(BLUE "  º         ³ ÚÄÄÄ³  ÍÍÍµÄÄÄ¿ ³         º")
	color_message(BLUE "  º         ³ ³Ä¿ ÆÍÍÍ  ³ÄÄÄÙ ³         º")
	color_message(BLUE "  º         ÀÄÙ ÀÄÀÄÄÄÄÄÙÄÄÄÄÄÙ         º")
	color_message(BLUE "  ÈÍÍÍÍÍÍÍÍÍÍÍÍÍ MAKE TOOL ÍÍÍÍÍÍÍÍÍÍÍÍÍŒ")
elseif(0)
	color_message(BLUE "   ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ ")
	color_message(BLUE "            ÚÄÄÄÄÄÚÄÄÄÄÄ¿ÄÄÄÄÄ¿          ")
	color_message(BLUE "            ³ ÚÄÄÄ³  ÍÍÍµÄÄÄ¿ ³          ")
	color_message(BLUE "            ³ ³Ä¿ ÆÍÍÍ  ³ÄÄÄÙ ³          ")
	color_message(BLUE "            ÀÄÙ ÀÄÀÄÄÄÄÄÙÄÄÄÄÄÙ          ")
	color_message(BLUE "   ÍÍÍÍÍÍÍÍÍÍÍÍÍ MAKE TOOL ÍÍÍÍÍÍÍÍÍÍÍÍÍ ")
elseif(1)
	color_message(BLUE "                     ÚÄÄÄÄÄ¿  ÚÄÄÄÄÄ¿  ÚÄÄÄÄÄ¿       ")
	color_message(BLUE "                     ³ ÚÄÂÄŽ  ³  ÄÄÄŽ  ÃÄÂÄ¿ ³       ")
	color_message(BLUE "                     ³ ÃÄŽ ³  ÃÄÄÄ  ³  ³ ÀÄÙ ³       ")
	color_message(BLUE "                     ÀÄÙ ÀÄÙ  ÀÄÄÄÄÄÙ  ÀÄÄÄÄÄÙ       ")
	color_message(BLUE "               ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ ")
	color_message(BLUE "                             MAKE TOOL               ")
else()
	color_message(BLUE "              ÉÍÍÍÍÍÍÍÍÍ» ÉÍÍÍÍÍÍÍÍÍ» ÉÍÍÍÍÍÍÍÍÍ»")
	color_message(BLUE "              º ÚÄÄÄÄÄ¿ º º ÚÄÄÄÄÄ¿ º º ÚÄÄÄÄÄ¿ º")
	color_message(BLUE "              º ³ ÚÄÂÄŽ º º ³  ÄÄÄŽ º º ÃÄÂÄ¿ ³ º")
	color_message(BLUE "              º ³ ÃÄŽ ³ º º ÃÄÄÄ  ³ º º ³ ÀÄÙ ³ º")
	color_message(BLUE "              º ÀÄÙ ÀÄÙ º º ÀÄÄÄÄÄÙ º º ÀÄÄÄÄÄÙ º")
	color_message(BLUE "              ÈÍÍÍÍÍÍÍÍÍŒ ÈÍÍÍÍÍÍÍÍÍŒ ÈÍÍÍÍÍÍÍÍÍŒ")
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
			set(TARGET_DIR "build-ninja")
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
		execute_process(COMMAND ${CMAKE_COMMAND} -G "${TARGET_GENERATOR}" -DMAKE_TARGET_KEY="${BUILD_TARGET}" -DMAKE_OPTIONS="${OPTIONS}" -DMAKE_TARGET_DIR=${TARGET_DIR} ${OPTIONS} -B${ASD_ROOT}/${TARGET_DIR} -H${ASD_ROOT})
	endforeach()
endif()
