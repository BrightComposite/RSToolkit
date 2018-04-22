#--------------------------------------------------------
#	vendor setup file
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

message("${MESSAGES_INDENTATION}  > Depends on vld")

set(ROOT ${CMAKE_CURRENT_LIST_DIR})

set(LIBRARY "${ROOT}/lib/${MODULE_ARCH}/vld.lib")
file(GLOB DLLS "${ROOT}/bin/${MODULE_ARCH}/*.dll")
file(GLOB MANIFEST "${ROOT}/bin/${MODULE_ARCH}/*.manifest")

add_module_include_dirs(${ROOT}/include)

file(COPY ${LIBRARY} DESTINATION ${LIBRARY_OUTPUT})

file(COPY ${DLLS} DESTINATION "${OUTPUT_ROOT}/bin/release")
file(COPY ${MANIFEST} DESTINATION "${OUTPUT_ROOT}/bin/release")
file(COPY ${DLLS} DESTINATION "${OUTPUT_ROOT}/bin/debug")
file(COPY ${MANIFEST} DESTINATION "${OUTPUT_ROOT}/bin/debug")