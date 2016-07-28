#--------------------------------------------------------
#	Rapture Physics
#--------------------------------------------------------

project(RapturePhysics CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(RaptureScene "scene")

	sources(modules)
		domain(physics)
		
		group(include Headers)
		files(
			Physics.h
			PhysicalWorld.h
			PlaneObject.h
		)

		group(src Sources)
		files(Physics.cpp)
	endsources()
endmodule()

load_third_party(bullet)

#--------------------------------------------------------