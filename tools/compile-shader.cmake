

cmake_minimum_required(VERSION 3.0)

	get_filename_component(OutputDir ${Output} DIRECTORY)
	get_filename_component(OutputName ${Output} NAME_WE)
	
	set(TimestampFile ${OutputDir}/${OutputName}.timestamp)
	file(TIMESTAMP ${Input} Timestamp)
	
	if(NOT EXISTS ${Output})
		if(NOT EXISTS ${OutputDir})
			file(MAKE_DIRECTORY ${OutputDir})
		endif()
		
		file(WRITE ${Output})
	endif()
	
	if(EXISTS ${TimestampFile})
		file(READ ${TimestampFile} SavedTimestamp)
		
		if(${SavedTimestamp} STREQUAL ${Timestamp})
			return()
		endif()
	endif()
		
	execute_process(COMMAND fxc /nologo "/T${ShaderType}_4_0" /Zpr /Fh ${Output} /Vn shader_code_${ShaderId} ${Input})
	file(WRITE ${TimestampFile} ${Timestamp})