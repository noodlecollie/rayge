# Creates a custom target called genexdebug, as per the CMake generator expression documentation.
# Build this target to see what the generator expression evaluates to.
function(debug_gen_expr expression)
	add_custom_target(genexdebug COMMAND ${CMAKE_COMMAND} -E echo "Generator expression: ${expression}")
	message(STATUS "DEBUG: Please build custom target \"genexdebug\" to debug generator expression: ${expression}")
endfunction()
