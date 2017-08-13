#--------------------------------------------------------
#	asd Core
#--------------------------------------------------------

project(core VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(STATIC)
	dependencies(meta	0.*)

	sources(modules)
		domain(core)

		group(include Headers)
		files(
			meta_class.h
			Exception.h
			handle.h
			Hash.h
			object.h
			shareable.h
			String.h

			addition/
				cached.h
				contents.h
				named.h
				singleton.h
				state.h
				wrapper.h
				..
			algorithm/
				crc32.h
				lookup3.h
				..
			function/
				Function.h
				Thread.h
				..
			intrinsic/
				Intrinsic.h
				IntrinsicCvt.h
				IntrinsicData.h
				..
			memory/
				memory.h
				Pool.h
				aligned.h

				allocator/
					aligned_alloc.h
					default_alloc.h
					pool_alloc.h
			)

		group(src Sources)
		files(
			Exception.cpp
			String.cpp

			algorithm/
				lookup3.cpp
				..
			intrinsic/
				Intrinsic.cpp
			)

		domain(container)

		group(include Headers)
		files(
			ArrayList.h
			Container.h
			Data.h
			List.h
			Map.h
			Queue.h
			Set.h
			Stack.h
			StaticMap.h
			)

		domain(morph)

		group(include Headers)
		files(
			Component.h
			Component.hpp
			Morph.h
			Morph.hpp
			Property.h
			Property.hpp
			)
	endsources()
endmodule()

#[[if(${UNIX})
	find_package(Threads REQUIRED)
	
	if(THREADS_HAVE_PTHREAD_ARG)
		set_property(TARGET ${PROJECT_NAME} PROPERTY COMPILE_OPTIONS "-pthread")
		set_property(TARGET ${PROJECT_NAME} PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
	endif()
	
	if(CMAKE_THREAD_LIBS_INIT)
		target_link_libraries(${PROJECT_NAME} "${CMAKE_THREAD_LIBS_INIT}")
	endif()
endif()]]

#--------------------------------------------------------
