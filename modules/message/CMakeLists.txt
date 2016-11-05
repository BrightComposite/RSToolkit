#--------------------------------------------------------
#	asd Message
#--------------------------------------------------------

project(asdMessage CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(asdCore "core")

	sources(modules)
		domain(message)

		group(include Headers)
		files(
			Channel.h
			Connect.h
			Message.h
			MessageClass.h
			Subject.h
		)

		group(src Sources)
		files(Subject.cpp)
	endsources()
endmodule()

#--------------------------------------------------------
