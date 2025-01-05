function(declare_dependent_file file_path out_var)
	if(NOT EXISTS "${file_path}")
		message(FATAL_ERROR "Required dependent file ${file_path} does not exist on disk.")
	endif()

	set(${out_var} "${file_path}" PARENT_SCOPE)
endfunction()

# A slight hack: raylib determines the best graphics backend to use
# automatically, which is helpful. It doesn't expost this value to us
# formally, but it does get set as a compile definition on the target.
# We can parse the definitions to get the graphics backend.
function(get_raylib_graphics_type out_var)
	get_target_property(raylib_defs raylib COMPILE_DEFINITIONS)
	string(REGEX MATCH "GRAPHICS_[A-Z0-9_]+" graphics "${raylib_defs}")

	if("${graphics}" STREQUAL "")
		message(FATAL_ERROR "Could not determine graphics backend to use!")
	endif()

	set(${out_var} "${graphics}" PARENT_SCOPE)
endfunction()
