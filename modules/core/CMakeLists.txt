#--------------------------------------------------------
#	asd Core
#--------------------------------------------------------

project(core VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(meta		0.*)

	sources(modules)
		domain(core)

		group(include Headers)
		files(
			Class.h
			Exception.h
			Handle.h
			Hash.h
			Object.h
			String.h

			addition/
				Cached.h
				Contents.h
				Named.h
				Singleton.h
				State.h
				Wrapper.h
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
				Memory.h
				Pool.h
				Aligned.h

				allocator/
					AlignedAllocator.h
					DefaultAllocator.h
					PoolAllocator.h
			)

		group(src Sources)
		files(
			Core.cpp
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

#--------------------------------------------------------
