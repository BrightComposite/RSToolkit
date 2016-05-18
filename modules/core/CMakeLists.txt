#--------------------------------------------------------
#	Rapture Core
#--------------------------------------------------------

project(RaptureCore CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/project.cmake)

#--------------------------------------------------------

start_module()

set_project_sources(PROJECT_SOURCES ${PROJECT_SOURCE_DIR}
	SRC_GROUP include Headers
		START_SECTION core
			Class.h
            Exception.h
			Handle.h
            Hash.h
			Morph.h
			Object.h
			String.h
			Subject.h

			START_SECTION action
				Action.h
				Function.h
				Thread.h
			END_SECTION

			START_SECTION addition
				Cached.h
				Contents.h
				Named.h
				Singleton.h
				State.h
				Wrapper.h
			END_SECTION

			START_SECTION algorithm
				crc32.h
				lookup3.h
			END_SECTION

			START_SECTION container
				Array.h
                Container.h
				List.h
				Map.h
				Queue.h
				RawData.h
				Set.h
				Stack.h
				StaticMap.h
				TypedSet.h
				TypedMap.h
			END_SECTION

			START_SECTION intrinsic
				Intrinsic.h
				IntrinsicCvt.h
				IntrinsicData.h
			END_SECTION

			START_SECTION memory
				BlockPool.h
				Memory.h
				PagePool.h
				UnsafeBlockPool.h

				START_SECTION allocator
					AlignedAllocator.h
					DefaultAllocator.h
					PoolAllocator.h
				END_SECTION
			END_SECTION

			START_SECTION message
				Channel.h
				Message.h
				MessageClass.h
			END_SECTION
		END_SECTION

		START_SECTION meta
			Adapt.h
			Bitmask.h
			BoolTypes.h
			Flag.h
			Inheritance.h
			Macro.h
			Meta.h
			Preprocessor.h
			Sfinae.h
			Types.h
			UseIf.h
		END_SECTION

	SRC_GROUP src Sources
		START_SECTION core
			Exception.cpp
			String.cpp

			START_SECTION algorithm
				lookup3.cpp
			END_SECTION

			START_SECTION intrinsic
				Intrinsic.cpp
			END_SECTION
		END_SECTION
)

#--------------------------------------------------------

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
load_third_party(boost COMPONENT preprocessor)

end_module()

#--------------------------------------------------------
