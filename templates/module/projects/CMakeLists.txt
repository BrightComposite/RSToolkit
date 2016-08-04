#--------------------------------------------------------
#	@module_path@ project
#--------------------------------------------------------

project(@module_path@ CXX)

#--------------------------------------------------------

include(${RAPTURE_TOOLS}/module.cmake)

#--------------------------------------------------------

module(APPLICATION)
	dependencies(
		RaptureApplication "application"
	)

	sources(projects)
		group(src Sources)
		files(
			main.cpp
		)
	endsources()
endmodule()

load_third_party(vld)

#--------------------------------------------------------