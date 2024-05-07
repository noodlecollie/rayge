#pragma once

// C11 allows (MyType){...}, but C++ does not like the () brackets.
// We cater for this here.
#ifndef __cplusplus
#define RAYGE_TYPE_LITERAL(type) (type)
#else
#define RAYGE_TYPE_LITERAL(type) type
#endif
