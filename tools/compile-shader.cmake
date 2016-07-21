

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
		
	if("${ShaderLang}" STREQUAL "HLSL")
		execute_process(COMMAND fxc /nologo "/T${ShaderType}_4_0" /Zpr /Fh ${Output} /Vn ${OutputVariable} ${Input})
	elseif("${ShaderLang}" STREQUAL "GLSL")
		file(READ ${Input} Contents)
		set(Contents "static const char ${OutputVariable}[] = R\"SHADER(\r\n${Contents}\r\n)SHADER\"\;")
		file(WRITE ${Output} ${Contents})
	endif()
	
	file(WRITE ${TimestampFile} ${Timestamp})