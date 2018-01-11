#--------------------------------------------------------
#	asd Message
#--------------------------------------------------------

project(message VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(STATIC)
	dependencies(core	0.*)

	sources(modules)
		domain(message)

		group(include Headers)
		files(
			Channel.h
			Channel.hpp
			Connect.h
			message.h
			MessageClass.h
			MessageClass.hpp
			MessageConnector.h
			MessageBroadcaster.h
			subject.h
		)

		group(src Sources)
		files(subject.cpp)
	endsources()
endmodule()

#--------------------------------------------------------
