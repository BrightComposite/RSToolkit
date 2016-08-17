#--------------------------------------------------------
#	Rapture Core
#--------------------------------------------------------

project(RaptureCore CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(RaptureMeta "meta")

	sources(modules)
		domain(core)
		
		group(include Headers)
		files(
			Class.h
			Component.h
			Exception.h
			Handle.h
			Hash.h
			Morpher.h
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
	endsources()
endmodule()

#--------------------------------------------------------
