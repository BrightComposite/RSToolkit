#--------------------------------------------------------
#	vendor setup file
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

include(${ASD_TOOLS}/git.cmake)

#--------------------------------------------------------

message("${MESSAGES_INDENTATION}  > Depends on rxcpp")

set(ROOT ${CMAKE_CURRENT_LIST_DIR}/code)

if (NOT EXISTS "${ROOT}")
    git_clone("https://github.com/Reactive-Extensions/RxCpp.git" code ${CMAKE_CURRENT_LIST_DIR})
endif()

add_module_include_dirs("${ROOT}/Rx/v2/src")
