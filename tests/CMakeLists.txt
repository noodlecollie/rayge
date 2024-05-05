# Enable this only for the test targets,
# since we want to limit other stuff to just C.
enable_language(CXX)

# Do these here so that they apply for all subtargets.
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/thirdparty/catch2-3.5.4/catch2-3.5.4/extras")
include(CTest)
include(Catch)

add_subdirectory(engine)
