
macro(listsubdir result curdir)
	file(GLOB children RELATIVE ${curdir} ${curdir}/*)
	set(dirs "")
	foreach(child ${children})
		if(IS_DIRECTORY ${curdir}/${child} AND EXISTS ${curdir}/${child}/CMakeLists.txt)
			list(APPEND dirs ${child})
		endif()
	endforeach()
	set(${result} ${dirs})
endmacro()

macro(use_cxx11)
	if (CMAKE_VERSION VERSION_LESS "3.1")
		if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")
		else ()
			set (CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")
		endif ()
	else ()
		set (CMAKE_CXX_STANDARD 11)
	endif ()
endmacro(use_cxx11)
