// File: InterfaceUtils.h
// Helper macros for ensuring robust passing of data
// across library boundaries.

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

#define RAYGE_INTERFACE_SENTRY_VALUE 0xFB15EF4F2AC07B8C

typedef struct RayGE_Interface_Header
{
	uint64_t beginSentry;
	uint64_t dataLength;
	uint64_t version;
} RayGE_Interface_Header;

typedef struct RayGE_Interface_Footer
{
	uint64_t endSentry;
} RayGE_Interface_Footer;

// Header and footer offsets here are provided primarily for extra safety.
// They are expected to receive the value from offsetof().
static inline bool
_RayGE_Interface_InitImpl(void* data, size_t length, size_t headerOffset, size_t footerOffset, uint64_t version)
{
	// Sanith check on data and length
	if ( !data || length <= sizeof(RayGE_Interface_Header) + sizeof(RayGE_Interface_Footer) )
	{
		assert(false);
		return false;
	}

	// Sanity check on version
	if ( version == 0 )
	{
		assert(false);
		return false;
	}

	// Check that the header is first in the struct
	if ( headerOffset != 0 )
	{
		assert(false);
		return false;
	}

	// Check that the footer is last in the struct
	if ( footerOffset + sizeof(RayGE_Interface_Footer) != length )
	{
		assert(false);
		return false;
	}

	// Set sentry and data length for later validation, and set version to provided value
	RayGE_Interface_Header* headerPtr = (RayGE_Interface_Header*)((char*)data + headerOffset);
	headerPtr->beginSentry = RAYGE_INTERFACE_SENTRY_VALUE;
	headerPtr->dataLength = length - sizeof(RayGE_Interface_Header) - sizeof(RayGE_Interface_Footer);
	headerPtr->version = version;

	// Set footer sentry for later validation
	RayGE_Interface_Footer* footerPtr = (RayGE_Interface_Footer*)((char*)data + footerOffset);
	footerPtr->endSentry = ~(RAYGE_INTERFACE_SENTRY_VALUE);

	return true;
}

static inline uint64_t _RayGE_Interface_ParseVersionImpl(const void* data, size_t length)
{
	// Sanity:
	if ( !data || length < sizeof(RayGE_Interface_Header) )
	{
		return 0;
	}

	// Header should be first
	RayGE_Interface_Header* headerPtr = (RayGE_Interface_Header*)data;

	// Ensure header makes sense
	if ( headerPtr->beginSentry != RAYGE_INTERFACE_SENTRY_VALUE ||
		 headerPtr->dataLength + sizeof(RayGE_Interface_Header) + sizeof(RayGE_Interface_Footer) != length )
	{
		return 0;
	}

	// Footer should be last
	RayGE_Interface_Footer* footerPtr =
		(RayGE_Interface_Footer*)((char*)data + sizeof(RayGE_Interface_Header) + headerPtr->dataLength);

	// Ensure footer makes sense
	if ( footerPtr->endSentry != ~(RAYGE_INTERFACE_SENTRY_VALUE) )
	{
		return 0;
	}

	return headerPtr->version;
}

static inline const void* _RayGE_Interface_VerifyImpl(const void* data, size_t length, uint64_t version)
{
	if ( !data || length < 1 )
	{
		return NULL;
	}

	const uint64_t parsedVersion = _RayGE_Interface_ParseVersionImpl(data, length);

	return (parsedVersion != 0 && parsedVersion == version) ? data : NULL;
}

#define RAYGE_INTERFACE_INIT_EXPLICIT(typeName, interfacePtr, inVersion, headerMemberName, footerMemberName) \
	do \
	{ \
		/* Assign local variable here, for convenience and extra compile-time checking */ \
		typeName* _ifc = (interfacePtr); \
\
		/* Check entire struct has a sensible length */ \
		static_assert( \
			sizeof(typeName) > (sizeof(RayGE_Interface_Header) + sizeof(RayGE_Interface_Footer)), \
			"Expected interface struct to contain data between header and footer"); \
\
		/* Check that header is first in the struct*/ \
		static_assert(offsetof(typeName, headerMemberName) == 0, "Expected interface header to be the first member"); \
\
		/* Check that footer is last in the struct */ \
		static_assert( \
			offsetof(typeName, footerMemberName) + sizeof(RayGE_Interface_Footer) == sizeof(typeName), \
			"Expected interface footer to be the last member"); \
\
		_RayGE_Interface_InitImpl( \
			_ifc, \
			sizeof(*_ifc), \
			offsetof(typeName, headerMemberName), \
			offsetof(typeName, footerMemberName), \
			(inVersion)); \
	} \
	while ( 0 )

#define RAYGE_INTERFACE_INIT(typeName, interfacePtr, version) \
	RAYGE_INTERFACE_INIT_EXPLICIT(typeName, interfacePtr, version, _header, _footer)

#define RAYGE_INTERFACE_VERIFY(interfacePtr, version) \
	_RayGE_Interface_VerifyImpl((interfacePtr), sizeof(*(interfacePtr)), (version))
