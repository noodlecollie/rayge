# This version of raylib is copied into the repo so that
# we can patch it as needed. (Upstream does not want the
# patches - https://github.com/raysan5/raylib/pull/3955)
option(WITH_PIC "" ON)
option(BUILD_SHARED_LIBS "" OFF)

add_subdirectory(raylib-5.0 EXCLUDE_FROM_ALL)
