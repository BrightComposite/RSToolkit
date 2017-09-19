#--------------------------------------------------------
#	@module_path@ command
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

project(@module_path@ VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

message("")
message("+ Add command \"@module_path@\"")

add_custom_target(
	${PROJECT_NAME}
	COMMAND ${CMAKE_COMMAND} ...
	WORKING_DIRECTORY "${ASD_ROOT}"
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER commands)