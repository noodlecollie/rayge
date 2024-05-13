#include "Input/InputBuffer.h"
#include "Subsystems/MemPoolSubsystem.h"
#include "Debugging.h"

struct RayGE_InputBuffer
{
	int* buffers[2];
	size_t bufferLength;
	size_t currentIndex;
};

#define OTHER_BUFFER_INDEX(index) (((index) + 1) % 2)

static void SetValues(int* buffer, size_t length, int value)
{
	for ( size_t index = 0; index < length; ++index )
	{
		buffer[index] = value;
	}
}

RayGE_InputBuffer* InputBuffer_Create(size_t maxSimultaneousInputs)
{
	RAYGE_ASSERT(maxSimultaneousInputs > 0 , "Expected max simultaneous input count to be greater than zero");

	if ( maxSimultaneousInputs < 1 )
	{
		return NULL;
	}

	RayGE_InputBuffer* buffer = MEMPOOL_CALLOC_STRUCT(MEMPOOL_INPUT, RayGE_InputBuffer);

	buffer->bufferLength = maxSimultaneousInputs;
	buffer->buffers[0] = MEMPOOL_CALLOC(MEMPOOL_INPUT, buffer->bufferLength, sizeof(int));
	buffer->buffers[1] = MEMPOOL_CALLOC(MEMPOOL_INPUT, buffer->bufferLength, sizeof(int));

	return buffer;
}

void InputBuffer_Destroy(RayGE_InputBuffer* buffer)
{
	if ( !buffer )
	{
		return;
	}

	MEMPOOL_FREE(buffer->buffers[0]);
	MEMPOOL_FREE(buffer->buffers[1]);
	MEMPOOL_FREE(buffer);
}

size_t InputBuffer_GetMaxLength(const RayGE_InputBuffer* buffer)
{
	return buffer ? buffer->bufferLength : 0;
}

int* InputBuffer_GetCurrentBuffer(RayGE_InputBuffer* buffer)
{
	return buffer ? buffer->buffers[buffer->currentIndex] : NULL;
}

int* InputBuffer_GetLastBuffer(RayGE_InputBuffer* buffer)
{
	return buffer ? buffer->buffers[OTHER_BUFFER_INDEX(buffer->currentIndex)] : NULL;
}

void InputBuffer_SetCurrentBufferValues(RayGE_InputBuffer* buffer, int value)
{
	if ( !buffer )
	{
		return;
	}

	SetValues(buffer->buffers[buffer->currentIndex], buffer->bufferLength, value);
}

void InputBuffer_SetLastBufferValues(RayGE_InputBuffer* buffer, int value)
{
	if ( !buffer )
	{
		return;
	}

	SetValues(buffer->buffers[OTHER_BUFFER_INDEX(buffer->currentIndex)], buffer->bufferLength, value);
}

void InputBuffer_Swap(RayGE_InputBuffer* buffer)
{
	if ( !buffer )
	{
		return;
	}

	buffer->currentIndex = OTHER_BUFFER_INDEX(buffer->currentIndex);
}
