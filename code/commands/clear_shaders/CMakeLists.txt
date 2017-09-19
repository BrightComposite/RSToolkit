#--------------------------------------------------------
#	clear_shaders command
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

project(clear_shaders VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

message("")
message("+ Add command \"clear_shaders\"")

add_custom_target(
	${PROJECT_NAME}
	COMMAND clear-output.bat
	WORKING_DIRECTORY "${ASD_TOOLS}/shaders"
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER commands)
