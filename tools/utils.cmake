#--------------------------------------------------------
#	ASD cmake utils
#--------------------------------------------------------

cmake_minimum_required(VERSION 3.3)

function(color_message color message)
	#string(TOLOWER ${color} color)
	#execute_process(COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --${color} --bold ${message})
	message(${message})
endfunction()