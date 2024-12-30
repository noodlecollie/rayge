#pragma once

#include <stddef.h>

// Including the null terminator
#define LOG_BUFFER_MESSAGE_MAX_LENGTH 512

typedef struct LogBackingBuffer LogBackingBuffer;

LogBackingBuffer* LogBackingBuffer_Create(size_t maxCapacityInBytes);
void LogBackingBuffer_Destroy(LogBackingBuffer* buffer);

// Length should not include the terminating character.
// If it is 0, strlen() is called to determine the length.
// Returns a pointer to the beginning of the usable buffer
// area, suitable for printing to something like eg. a console.
const char* LogBackingBuffer_Append(LogBackingBuffer* buffer, const char* string, size_t length);

// Returns a pointer to the beginning of the usable buffer
// area, suitable for printing to something like eg. a console.
const char* LogBackingBuffer_Begin(const LogBackingBuffer* buffer);

// Number of characters in the printable string, not including the null terminator.
size_t LogBackingBuffer_StringLength(const LogBackingBuffer* buffer);
