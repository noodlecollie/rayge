include(rayge_definitions)

macro(generate_engine_install install_root)
	install(TARGETS ${TARGETNAME_ENGINE}
		# This is only for Windows.
		# If games want to use the API, they will need the .lib file
		# to be read by the linker. This is not the same as the
		# engine being statically compiled - the shared engine
		# library should live alongside the launcher, as specified below.
		ARCHIVE DESTINATION ${install_root}/api/lib

		RUNTIME DESTINATION ${install_root}
		LIBRARY DESTINATION ${install_root}
		FILE_SET HEADERS DESTINATION ${install_root}/api/include
	)

	install(TARGETS ${TARGETNAME_LAUNCHER}
		DESTINATION ${install_root}
	)

	install(FILES ${CMAKE_SOURCE_DIR}/launcher/launch.sh
		DESTINATION ${install_root}
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)
endmacro()
