# The pthreads-win32 readme states:
#
#   Use either pthreadGCv.* if you use GCC, or pthreadVCv.* if you
#   use MSVC - where 'v' is the DLL versioning (compatibility) number.
#
# Which means we probably want pthreadVC2. However, we'll build the
# sources manually instead of using the pre-built DLLs, becase I want
# everything here to be buildable from source in case we need to
# tweak anything.
#
# The "VC" suffix means build with MSVC (V) and use C-style exception
# handling (C).
# "nmake clean VC-static" will build the static lib, and
# "nmake clean VC-static-debug" will build the above with debugging
# symbols.
#
# The following target is set up according to the info specified in
# the Makefile of the nested project.

add_library(pthreads-win32 STATIC
	pthreads.2/attr.c
	pthreads.2/barrier.c
	pthreads.2/cancel.c
	pthreads.2/cleanup.c
	pthreads.2/condvar.c
	pthreads.2/create.c
	pthreads.2/dll.c
	pthreads.2/exit.c
	pthreads.2/fork.c
	pthreads.2/global.c
	pthreads.2/misc.c
	pthreads.2/mutex.c
	pthreads.2/private.c
	pthreads.2/rwlock.c
	pthreads.2/sched.c
	pthreads.2/semaphore.c
	pthreads.2/spin.c
	pthreads.2/sync.c
	pthreads.2/tsd.c
	pthreads.2/nonportable.c
)

target_include_directories(pthreads-win32 PUBLIC pthreads.2)
target_include_directories(pthreads-win32 PRIVATE pthreads.2)

target_compile_definitions(pthreads-win32 PUBLIC
	-D__CLEANUP_C
	-DPTW32_BUILD_INLINED
	-DPTW32_STATIC_LIB
	-DHAVE_PTW32_CONFIG_H

	# Apparently struct timespec is defined on Windows, at least in the
	# version of the Windows SDK that I'm using? The compiler reports it's at
	# Windows Kits\10\Include\10.0.17763.0\ucrt\time.h(39)
	# Let's just define this for now, and deal with any more complicated
	# deduction regarding it later on, if we need to.
	-D_TIMESPEC_DEFINED
)

# Just in case (see https://cmake.org/cmake/help/latest/prop_tgt/MSVC_RUNTIME_LIBRARY.html):
set_property(TARGET pthreads-win32 PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
