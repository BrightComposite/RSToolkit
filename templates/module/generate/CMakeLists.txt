#--------------------------------------------------------
#	@library_name@ module
#--------------------------------------------------------

project(@library_name@ CXX)

#--------------------------------------------------------

include(${ASD_TOOLS}/module.cmake)

#--------------------------------------------------------

module(@library_type@)
	sources(@library_destination@)
@library_sources@
	endsources()
endmodule()

#--------------------------------------------------------