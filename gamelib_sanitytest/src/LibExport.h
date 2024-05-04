#ifndef GAMELIB_SANITYTEST_EXPORT_HEADER_INCLUDED
#define GAMELIB_SANITYTEST_EXPORT_HEADER_INCLUDED

////////////////////////////////////////////
// Extern C
////////////////////////////////////////////

#ifdef __cplusplus
// C++ needs extern C annotation
#define GAMELIB_SANITYTEST_EXTERNC extern "C"
#else
// C does not need this annotation
#define GAMELIB_SANITYTEST_EXTERNC
#endif  // __cplusplus

#if defined(_WIN32)

////////////////////////////////////////////
// Begin Windows
////////////////////////////////////////////

// Windows supports specifying the cdecl calling convention
#define GAMELIB_SANITYTEST_CDECL __cdecl

#ifdef GAMELIB_SANITYTEST_PRODUCER
// Shared library is being built, so mark symbols for export
#define GAMELIB_SANITYTEST_EXPORT __declspec(dllexport)
#else
// Shared library is being used, so mark symbols for import
#define GAMELIB_SANITYTEST_EXPORT __declspec(dllimport)
#endif  // GAMELIB_SANITYTEST_PRODUCER

////////////////////////////////////////////
// End Windows
////////////////////////////////////////////

#elif defined(__linux__)

////////////////////////////////////////////
// Begin Linux
////////////////////////////////////////////

// Not on Windows, so cdecl is not required
#define GAMELIB_SANITYTEST_CDECL

// Shared library is being built, so mark exported symbols as visible
#define GAMELIB_SANITYTEST_EXPORT __attribute__((visibility("default")))

////////////////////////////////////////////
// End Linux
////////////////////////////////////////////

#else
#error Current platform is not supported by this export header
#endif

// Now combine all of these switches into a macro that exposes a function in the library's public API:
#define GAMELIB_SANITYTEST_PUBLIC(returnType) \
	GAMELIB_SANITYTEST_EXTERNC GAMELIB_SANITYTEST_EXPORT returnType GAMELIB_SANITYTEST_CDECL

#endif  // GAMELIB_SANITYTEST_EXPORT_HEADER_INCLUDED
