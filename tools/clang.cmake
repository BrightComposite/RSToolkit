set(CMAKE_C_FLAGS_INIT                	"-Wall -std=c99" 	CACHE STRING "Cmake C flags")
set(CMAKE_C_FLAGS_DEBUG_INIT          	"-g" 				CACHE STRING "Cmake C flags debug")
set(CMAKE_C_FLAGS_MINSIZEREL_INIT     	"-Os -DNDEBUG" 		CACHE STRING "Cmake C flags minsizerel")
set(CMAKE_C_FLAGS_RELEASE_INIT        	"-O4 -DNDEBUG" 		CACHE STRING "Cmake C flags release")
set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT 	"-O2 -g" 			CACHE STRING "Cmake C flags relwithdebinfo")

#set(CMAKE_CXX_FLAGS_INIT                "-Wall -std=c++14" 	CACHE STRING "Cmake Cpp flags")
set(CMAKE_CXX_FLAGS_DEBUG_INIT          "-g"				CACHE STRING "Cmake Cpp flags debug")
set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "-Os -DNDEBUG" 		CACHE STRING "Cmake Cpp flags minsizerel")
set(CMAKE_CXX_FLAGS_RELEASE_INIT        "-O4 -DNDEBUG" 		CACHE STRING "Cmake Cpp flags release")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g" 			CACHE STRING "Cmake Cpp flags relwithdebinfo")

set(_CMAKE_TOOLCHAIN_PREFIX 			"llvm-" 			CACHE STRING "Cmake toolchain prefix")
