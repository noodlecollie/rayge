add_executable(sanity main.c)
target_link_libraries(sanity PRIVATE raylib)
install(TARGETS sanity)
