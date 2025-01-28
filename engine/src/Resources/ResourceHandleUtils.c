#include "Resources/ResourceHandleUtils.h"
#include "Debugging.h"
#include "wzl_cutl/time.h"
// This operation should be reversible with the same index.
static uint64_t MixKeyWithIndex(uint64_t key, uint32_t index)
{
	// Index is used twice, once in a bit-flipped form, so that we're
	// guaranteed at least one bit will change in the nested XOR operation)
	return key ^ (0x1234BA55FACE5678 ^ (((uint64_t)(~index) << 32) | index));
}

RayGE_ResourceHandle Resource_CreateInternalHandle(InternalResourceDomain domain, uint32_t index, uint64_t key)
{
	RAYGE_ASSERT(!((uint32_t)domain & ~RESOURCE_DOMAIN_ID_MASK), "Resource domain contained bits outside ID mask");

	RAYGE_ASSERT(
		domain > RESOURCE_DOMAIN_INVALID && domain < RESOURCE_DOMAIN__COUNT,
		"Resource domain was outside valid range"
	);

	return (RayGE_ResourceHandle) {
		.domain = ((uint32_t)domain & RESOURCE_DOMAIN_ID_MASK) | RESOURCEFLAG_INTERNAL_DOMAIN,
		.index = index,
		.key = key,
	};
}

uint64_t Resource_CreateKey(uint32_t index)
{
	static unsigned int bitShift = 0;

	uint64_t timestamp = wzl_get_milliseconds_monotonic();
	RAYGE_ENSURE(timestamp != 0, "Expected non-zero timestamp");

	uint64_t timeBits = timestamp;

	// Shift the bits differently on each call.
	if ( bitShift > 0 )
	{
		timeBits = (timeBits << bitShift) | (timeBits >> ((8 * sizeof(uint64_t)) - bitShift));
	}

	bitShift = (bitShift + 1) % (8 * sizeof(uint64_t));

	// Make sure the key's value is affected by the index.
	uint64_t outKey = MixKeyWithIndex(timeBits, index);

	// I'd be amazed if this ever happened:
	RAYGE_ENSURE(
		outKey != 0,
		"Generated invalid entity key, which should be virtually impossible. Time: %ju Bits: 0x%016jx",
		(uintmax_t)timestamp,
		(uintmax_t)timeBits
	);

	return outKey;
}

InternalResourceDomain Resource_GetInternalDomain(RayGE_ResourceHandle handle)
{
	if ( !(handle.domain & RESOURCEFLAG_INTERNAL_DOMAIN) )
	{
		return RESOURCE_DOMAIN_INVALID;
	}

	uint32_t decodedDomain = handle.domain & RESOURCE_DOMAIN_ID_MASK;

	return (decodedDomain > RESOURCE_DOMAIN_INVALID && decodedDomain < RESOURCE_DOMAIN__COUNT)
		? (InternalResourceDomain)decodedDomain
		: RESOURCE_DOMAIN_INVALID;
}

bool Resource_HandleIsValidForInternalDomain(
	RayGE_ResourceHandle handle,
	InternalResourceDomain domain,
	uint32_t maxResourceCount
)
{
	InternalResourceDomain decodedDomain = Resource_GetInternalDomain(handle);

	if ( decodedDomain == RESOURCE_DOMAIN_INVALID || decodedDomain != domain )
	{
		return false;
	}

	return handle.index < maxResourceCount;
}
