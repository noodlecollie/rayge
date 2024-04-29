function(declare_dependent_file file_path out_var)
	if(NOT EXISTS "${file_path}")
		message(FATAL_ERROR "Required dependent file ${file_path} does not exist on disk.")
	endif()

	set(${out_var} "${file_path}" PARENT_SCOPE)
endfunction()
