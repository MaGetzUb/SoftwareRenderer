#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <malloc.h>


#ifdef __linux__

#define _aligned_malloc(size, align) aligned_alloc(align, size)

#define _aligned_free(mem) free(mem)

inline void* _aligned_realloc(void* memory, size_t size, size_t align) {
    if(memory) _aligned_free(memory);
    return _aligned_malloc(size, align);
}

#endif 

#endif 