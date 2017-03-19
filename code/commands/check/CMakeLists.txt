
cmake_minimum_required(VERSION 3.3)

project(check VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

add_custom_target(
	${PROJECT_NAME}
	COMMAND ${CMAKE_COMMAND} -G "${MAKE_TARGET_GENERATOR}" -DMAKE_TARGET_GENERATOR="${MAKE_TARGET_GENERATOR}" -DMAKE_TARGET_KEY="${MAKE_TARGET_KEY}" -DMAKE_OPTIONS="${MAKE_OPTIONS}" -DMAKE_TARGET_DIR="${MAKE_TARGET_DIR}" ${MAKE_OPTIONS} ${ASD_ROOT} ${ASD_ROOT}/${MAKE_TARGET_DIR}
	WORKING_DIRECTORY ${ASD_ROOT}
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER commands)