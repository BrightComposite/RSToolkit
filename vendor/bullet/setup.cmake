#--------------------------------------------------------
#	vendor setup file
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

#--------------------------------------------------------

set(HAS_LIBRARIES TRUE)

if(WIN32)
	set(UNITS
		Bullet3Collision.lib
		Bullet3Common.lib
		Bullet3Dynamics.lib
		Bullet3Geometry.lib
		Bullet3OpenCL_clew.lib
		Bullet2FileLoader/Bullet2FileLoader.lib
		BulletCollision.lib
		BulletDynamics.lib
		BulletInverseDynamics.lib
		BulletSoftBody.lib
		LinearMath.lib
		)
elseif(UNIX)
	set(UNITS
		Bullet3Collision/libBullet3Collision.a
		Bullet3Common/libBullet3Common.a
		Bullet3Dynamics/libBullet3Dynamics.a
		Bullet3Geometry/libBullet3Geometry.a
		Bullet3OpenCL/libBullet3OpenCL_clew.a
		Bullet3Serialize/Bullet2FileLoader/libBullet2FileLoader.a
		BulletCollision/libBulletCollision.a
		BulletDynamics/libBulletDynamics.a
		BulletInverseDynamics/libBulletInverseDynamics.a
		BulletSoftBody/libBulletSoftBody.a
		LinearMath/libLinearMath.a
		)
else()
	message(FATAL_ERROR "Unsupported platform! Needed to implement")
endif()

set(LIBRARIES)

foreach(UNIT ${UNITS})
	set(PATH ${BUILD_ROOT}/vendor/bullet/lib/Release/${UNIT})
	set(LIBRARIES ${LIBRARIES};${PATH})

	if(NOT EXISTS ${PATH})
		set(HAS_LIBRARIES FALSE)
		message("Can't find ${PATH}")
		break()
	endif()
endforeach()

if(NOT HAS_LIBRARIES OR NOT EXISTS "")
	file(MAKE_DIRECTORY ${OUTPUT_ROOT}/vendor/bullet)
	file(MAKE_DIRECTORY ${BUILD_ROOT}/vendor/bullet)
	execute_process(COMMAND ${CMAKE_COMMAND} ${CMAKE_CURRENT_LIST_DIR}/bullet3-2.86.1 -DCMAKE_INSTALL_PREFIX=${OUTPUT_ROOT}/vendor/bullet WORKING_DIRECTORY ${BUILD_ROOT}/vendor/bullet)
	execute_process(COMMAND ${CMAKE_COMMAND} --build ${BUILD_ROOT}/vendor/bullet --config Release)
endif()

# foreach(UNIT ${UNITS})
# 	set(PATH ${BUILD_ROOT}/vendor/bullet/lib/Release/${UNIT})

# 	if(NOT EXISTS ${PATH})
# 		message(FATAL_ERROR "Can't find ${PATH}")
# 	endif()
# endforeach()

add_module_include_dirs(${CMAKE_CURRENT_LIST_DIR}/bullet3-2.86.1/src)
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})
