
#include <cstdlib>

#ifdef _WIN32 

#elif defined(__linux__)

#include <cstdlib>

#define _aligned_malloc(size, align) aligned_alloc(align, size)

#define _aligned_free(mem) free(mem) 

#endif 


