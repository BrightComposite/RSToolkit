#--------------------------------------------------------
#	@library_name@ module
#--------------------------------------------------------

project(@library_name@ VERSION 0.1)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(@library_type@)
	sources(@library_destination@)
@library_sources@
	endsources()
endmodule()

#--------------------------------------------------------