#include "vgl_realloc.hpp"

#include <new>

void operator delete(void *ptr) noexcept
{
    dnload_free(ptr);
}

void operator delete(void *ptr, size_t align) noexcept
{
    dnload_free(ptr);
    (void)align;
}

void* operator new(size_t sz)
{
#if defined(USE_LD) && defined(DEBUG)
    if(!sz)
    {
        std::cerr << "WARNING: call to new() with size 0" << std::endl;
    }
#endif
    return dnload_realloc(nullptr, sz);
}

void* operator new(size_t sz, size_t align)
{
#if defined(USE_LD) && defined(DEBUG)
    if(!sz)
    {
        std::cerr << "WARNING: call to new() with size 0" << std::endl;
    }
#endif
    return dnload_realloc(nullptr, sz);
    (void)align;
}

#if defined(USE_LD)

void operator delete[](void *ptr) noexcept
{
    dnload_free(ptr);
}

void operator delete[](void *ptr, size_t align) noexcept
{
    dnload_free(ptr);
    (void)align;
}

void* operator new[](size_t sz)
{
#if defined(DEBUG)
    if(!sz)
    {
        std::cerr << "WARNING: call to new[]() with size 0" << std::endl;
    }
#endif
    return dnload_realloc(nullptr, sz);
}

void* operator new[](size_t sz, size_t align)
{
#if defined(DEBUG)
    if(!sz)
    {
        std::cerr << "WARNING: call to new[]() with size 0" << std::endl;
    }
#endif
    return dnload_realloc(nullptr, sz);
    (void)align;
}

#endif

