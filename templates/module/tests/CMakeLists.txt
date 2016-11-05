#--------------------------------------------------------
#	@module_path@ test facility
#--------------------------------------------------------

project(@module_path@ CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION)
	dependencies(
		asd-Application "application"
	)

	sources(tests)
		group(src Sources)
		files(
			main.cpp
		)
	endsources()
endmodule()

load_third_party(vld)

#--------------------------------------------------------