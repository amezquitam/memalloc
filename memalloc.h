
#ifndef __cplusplus

#include <stdlib.h>

#else

#include <cstdlib>

extern "C" {

#endif // __cplusplus

void memalloc_init(void* arena, size_t arena_size);

void* memalloc(size_t size);

void memfree(void* mem);

#ifdef __cplusplus

}

#endif // __cplusplus