#include "Logging/LogBackingBuffer.h"
#include "MemPool/MemPoolManager.h"
#include "Debugging.h"

struct LogBackingBuffer
{
	char* base;
	size_t lengthInBytes;
	size_t maxStringLength;  // Not including terminator

	char* primaryStringBegin;
	size_t primaryStringLength;  // Not including terminator
	size_t secondaryStringLength;  // Not including terminator
};

static char* GetHalfwayPoint(const LogBackingBuffer* buffer)
{
	RAYGE_ASSERT_VALID(buffer);
	return buffer->base + (buffer->lengthInBytes / 2);
}

static void AppendToPrimaryString(LogBackingBuffer* buffer, const char* string, size_t length)
{
	RAYGE_ASSERT_VALID(buffer);
	RAYGE_ASSERT_VALID(string);

	RAYGE_ENSURE(
		buffer->primaryStringBegin + buffer->primaryStringLength + length + 1 <= buffer->base + buffer->lengthInBytes,
		"Incoming primary string overflowed log backing buffer"
	);

	memcpy(buffer->primaryStringBegin + buffer->primaryStringLength, string, length);
	buffer->primaryStringLength += length;
	buffer->primaryStringBegin[buffer->primaryStringLength] = '\0';
}

static void AppendToSecondaryString(LogBackingBuffer* buffer, const char* string, size_t length)
{
	RAYGE_ASSERT_VALID(buffer);
	RAYGE_ASSERT_VALID(string);

	// The secondary string should never be larger than half the buffer,
	// because it should always be promoted before becoming that large.
	RAYGE_ENSURE(
		buffer->secondaryStringLength + length < buffer->lengthInBytes / 2,
		"Secondary string would be longer than theoretical max length"
	);

	// We don't need to terminate this since it's not used immediately,
	// and termination is handled when it is used.
	memcpy(buffer->base + buffer->secondaryStringLength, string, length);
	buffer->secondaryStringLength += length;
}

static char* FindBeginningOfNextNewLine(LogBackingBuffer* buffer, size_t offset)
{
	char* nextNewLine = strchr(buffer->base + offset, '\n');

	// If no newline was found, or it was the very last character in the buffer,
	// we can't return anything useful.
	if ( !nextNewLine || nextNewLine >= buffer->base + buffer->lengthInBytes - 2 )
	{
		return NULL;
	}

	// Skip past this newline.
	++nextNewLine;

	// Skip past any other ones that might be present in a row.
	while ( *nextNewLine == '\r' || *nextNewLine == '\n' )
	{
		++nextNewLine;
	}

	return *nextNewLine != '\0' ? nextNewLine : NULL;
}

static void PromoteSecondaryString(LogBackingBuffer* buffer)
{
	RAYGE_ASSERT_VALID(buffer);

	buffer->primaryStringBegin = buffer->base;
	buffer->primaryStringLength = buffer->secondaryStringLength;
	buffer->primaryStringBegin[buffer->primaryStringLength] = '\0';

	buffer->secondaryStringLength = 0;
}

LogBackingBuffer* LogBackingBuffer_Create(size_t maxCapacityInBytes)
{
	RAYGE_ENSURE(maxCapacityInBytes >= 2 * LOG_BUFFER_MESSAGE_MAX_LENGTH, "Buffer must be at least 2 lines long");

	LogBackingBuffer* buffer = MEMPOOL_CALLOC_STRUCT(MEMPOOL_LOGGING, LogBackingBuffer);

	buffer->lengthInBytes = maxCapacityInBytes;
	buffer->maxStringLength = (maxCapacityInBytes / 2) - 1;
	buffer->base = MEMPOOL_MALLOC(MEMPOOL_LOGGING, buffer->lengthInBytes);
	buffer->base[0] = '\0';
	buffer->primaryStringBegin = buffer->base;

	return buffer;
}

void LogBackingBuffer_Destroy(LogBackingBuffer* buffer)
{
	RAYGE_ASSERT_VALID(buffer);

	if ( !buffer )
	{
		return;
	}

	RAYGE_ASSERT_VALID(buffer->base);
	MEMPOOL_FREE(buffer->base);
	MEMPOOL_FREE(buffer);
}

const char* LogBackingBuffer_Append(LogBackingBuffer* buffer, const char* string, size_t length)
{
	RAYGE_ASSERT_VALID(buffer);

	if ( !buffer )
	{
		return NULL;
	}

	if ( !string )
	{
		return buffer->primaryStringBegin;
	}

	if ( length < 1 )
	{
		length = (size_t)strlen(string);

		if ( length < 1 )
		{
			return buffer->primaryStringBegin;
		}
	}

	if ( length > LOG_BUFFER_MESSAGE_MAX_LENGTH - 1 )
	{
		length = LOG_BUFFER_MESSAGE_MAX_LENGTH - 1;
	}

	// +1 to account for the terminator.
	char* primaryStringEnd = buffer->primaryStringBegin + buffer->primaryStringLength + 1;
	primaryStringEnd += length;

	AppendToPrimaryString(buffer, string, length);

	// If we don't cross the buffer halfway point, we don't have to do anything else.
	if ( primaryStringEnd < GetHalfwayPoint(buffer) )
	{
		return buffer->primaryStringBegin;
	}

	// We crossed half way through the buffer, so we will need to do some extra work.
	// How much space is left?
	const size_t bytesLeftInBuffer = buffer->lengthInBytes - (size_t)(primaryStringEnd - buffer->base);

	// We can still use the primary string if there is definitely enough space to fit another message.
	bool canStillUsePrimaryString = bytesLeftInBuffer >= LOG_BUFFER_MESSAGE_MAX_LENGTH;

	if ( canStillUsePrimaryString )
	{
		// See if we can find a good place for the beginning of the primary string to be moved to.
		char* nextLineBegin = FindBeginningOfNextNewLine(buffer, buffer->secondaryStringLength + length);

		if ( nextLineBegin )
		{
			// Move the beginning of the primary string to start here.
			buffer->primaryStringLength -= nextLineBegin - buffer->primaryStringBegin;
			buffer->primaryStringBegin = nextLineBegin;
		}
		else
		{
			// There was no newline found after the end of where the secondary string will be,
			// so there is nowhere to locate the new beginning of the primary string.
			canStillUsePrimaryString = false;
		}
	}

	// Make sure we store our incoming message in the secondary string too.
	AppendToSecondaryString(buffer, string, length);

	// If the primary string cannot be used any more, swap the strings.
	if ( !canStillUsePrimaryString )
	{
		PromoteSecondaryString(buffer);
	}

	RAYGE_ASSERT(
		buffer->primaryStringLength <= buffer->maxStringLength,
		"String exceeded max allowed length"
	);

	return buffer->primaryStringBegin;
}

const char* LogBackingBuffer_Begin(const LogBackingBuffer* buffer)
{
	RAYGE_ASSERT_VALID(buffer);

	return buffer ? buffer->primaryStringBegin : NULL;
}

size_t LogBackingBuffer_StringLength(const LogBackingBuffer* buffer)
{
	RAYGE_ASSERT_VALID(buffer);

	return buffer ? buffer->primaryStringLength : 0;
}

size_t LogBackingBuffer_MaxStringLength(const LogBackingBuffer* buffer)
{
	RAYGE_ASSERT_VALID(buffer);

	return buffer ? buffer->maxStringLength : 0;
}
