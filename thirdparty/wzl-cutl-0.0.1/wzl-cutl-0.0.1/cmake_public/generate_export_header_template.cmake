function(generate_export_header_template)
	set(options STATIC SHARED)
	set(oneValueArgs TARGET OUTPUT IS_STATIC PRODUCER_DEFINE)
	set(multiValueArgs "")

	cmake_parse_arguments(EXPORTHEADER "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(EXPORTHEADER_STATIC AND EXPORTHEADER_SHARED)
		message(FATAL_ERROR "Cannot specify both STATIC and SHARED in this function invocation")
	endif()

	if("${EXPORTHEADER_PRODUCER_DEFINE}" STREQUAL "")
		message(FATAL_ERROR "PRODUCER_DEFINE must be supplied to this function")
	endif()

	string(TOUPPER "${EXPORTHEADER_TARGET}" EXPORTHEADER_TARGET_UPPERCASE)
	string(REGEX REPLACE "[^A-Z0-9_]" "_" EXPORTHEADER_TARGET_UPPERCASE "${EXPORTHEADER_TARGET_UPPERCASE}")

	# Prepend a prefix if the string begins with a number
	if("${EXPORTHEADER_TARGET_UPPERCASE}" MATCHES "^[0-9]")
		set(EXPORTHEADER_TARGET_UPPERCASE "_${EXPORTHEADER_TARGET_UPPERCASE}")
	endif()

	if(EXPORTHEADER_STATIC)
		set(EXPORTHEADER_STATIC_VAL 1)
	elseif(EXPORTHEADER_SHARED)
		set(EXPORTHEADER_STATIC_VAL 0)
	else()
		if(BUILD_SHARED_LIBS)
			set(EXPORTHEADER_STATIC_VAL 0)
		else()
			set(EXPORTHEADER_STATIC_VAL 1)
		endif()
	endif()

	configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/export_header_template.in" "${EXPORTHEADER_OUTPUT}")
endfunction()
