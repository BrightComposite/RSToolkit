#--------------------------------------------------------
#	asd Message
#--------------------------------------------------------

project(message VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(SHARED)
	dependencies(core	0.*)

	sources(modules)
		domain(message)

		group(include Headers)
		files(
			Channel.h
			Channel.hpp
			Connect.h
			Message.h
			MessageClass.h
			MessageClass.hpp
			MessageConnector.h
			MessageBroadcaster.h
			Subject.h
		)

		group(src Sources)
		files(Subject.cpp)
	endsources()
endmodule()

#--------------------------------------------------------
