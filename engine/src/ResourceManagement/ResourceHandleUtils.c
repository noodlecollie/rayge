#include "ResourceManagement/ResourceHandleUtils.h"
#include "Debugging.h"
#include "raylib.h"

// This operation should be reversible with the same index.
static uint64_t MixKeyWithIndex(uint64_t key, uint32_t index)
{
	// Index is used twice, once in a bit-flipped form, so that we're
	// guaranteed at least one bit will change in the nested XOR operation)
	return key ^ (0x1234BA55FACE5678 ^ (((uint64_t)(~index) << 32) | index));
}

RayGE_ResourceHandle Resource_CreateInternalHandle(RayGE_InternalResourceDomain domain, uint32_t index, uint64_t key)
{
	RAYGE_ASSERT(!((uint32_t)domain & ~RESOURCE_DOMAIN_ID_MASK), "Resource domain contained bits outside ID mask");

	RAYGE_ASSERT(
		domain > RESOURCE_DOMAIN_INVALID && domain < RESOURCE_DOMAIN__COUNT,
		"Resource domain was outside valid range"
	);

	return (RayGE_ResourceHandle) {
		.domain = RESOURCEFLAG_INTERNAL_DOMAIN | ((uint32_t)domain & RESOURCE_DOMAIN_ID_MASK),
		.index = index,
		.key = key,
	};
}

uint64_t Resource_CreateKey(uint32_t index)
{
	typedef union Timestamp
	{
		uint64_t bitsAsUint64;
		double timeElapsed;
	} Timestamp;

	Timestamp ts;
	ts.timeElapsed = GetTime();

	// Shouldn't happen, but may if the window has not been created yet for some reason.
	RAYGE_ENSURE(ts.timeElapsed != 0.0f, "Cannot create entity key without access to underlying elapsed time");

	// Make sure the key's value is affected by the index.
	uint64_t outKey = MixKeyWithIndex(ts.bitsAsUint64, index);

	// I'd be amazed if this ever happened:
	RAYGE_ENSURE(
		outKey != 0,
		"Generated invalid entity key, which should be virtually impossible. Time: %lf Bits: 0x%016x",
		ts.timeElapsed,
		ts.bitsAsUint64
	);

	return outKey;
}

RayGE_InternalResourceDomain Resource_GetInternalDomain(RayGE_ResourceHandle handle)
{
	if ( !(handle.domain & RESOURCEFLAG_INTERNAL_DOMAIN) )
	{
		return RESOURCE_DOMAIN_INVALID;
	}

	uint32_t decodedDomain = handle.domain & RESOURCE_DOMAIN_ID_MASK;

	return (decodedDomain > RESOURCE_DOMAIN_INVALID && decodedDomain < RESOURCE_DOMAIN__COUNT)
		? (RayGE_InternalResourceDomain)decodedDomain
		: RESOURCE_DOMAIN_INVALID;
}

bool Resource_HandleIsValidForInternalDomain(
	RayGE_ResourceHandle handle,
	RayGE_InternalResourceDomain domain,
	uint32_t maxResourceCount
)
{
	RayGE_InternalResourceDomain decodedDomain = Resource_GetInternalDomain(handle);

	if ( decodedDomain == RESOURCE_DOMAIN_INVALID || decodedDomain != domain )
	{
		return false;
	}

	return handle.index < maxResourceCount;
}
