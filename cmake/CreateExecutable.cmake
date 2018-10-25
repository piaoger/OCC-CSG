CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

function(CreateExecutable EXECUTABLESOURCE)
	
	get_filename_component(EXECUTABLEBINARY ${EXECUTABLESOURCE} NAME_WE)

	set(libslvs_SOURCES

	    ./opencascade/brep.cpp
	    ./opencascade/csg.cpp
	    ./opencascade/convert.cpp
	    ./opencascade/query.cpp
	)

	add_executable(${EXECUTABLEBINARY} ${EXECUTABLESOURCE} ${libslvs_SOURCES})
	target_link_libraries(${EXECUTABLEBINARY} ${ARGN})

endfunction(CreateExecutable)
