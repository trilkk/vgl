#ifndef VGL_REALLOC_HPP
#define VGL_REALLOC_HPP

#include "vgl_extern_stdlib.hpp"

#include <new>

#if defined(USE_LD)
#include <boost/throw_exception.hpp>
#include <iostream>
#endif

/// A global delete operator using free().
///
/// \param ptr Pointer to free.
inline void operator delete(void *ptr) noexcept
{
    dnload_free(ptr);
}
/// Aligned global delete operator using free().
///
/// \param ptr Pointer to free.
/// \param align Ignored alignment.
inline void operator delete(void *ptr, size_t align) noexcept
{
    dnload_free(ptr);
    (void)align;
}

/// A global new operator using realloc().
///
/// \param sz Size to allocate.
/// \return Allocated pointer.
inline void* operator new(size_t sz)
{
#if defined(USE_LD) && defined(DEBUG)
    if(!sz)
    {
        std::cerr << "WARNING: call to new() with size 0" << std::endl;
    }
#endif
    return dnload_realloc(nullptr, sz);
}

namespace vgl
{

namespace detail
{

/// Internal memcpy.
///
/// \param dst Destination pointer.
/// \param src Source pointer.
/// \param count Number of bytes to copy.
/// \return Destination pointer.
constexpr void* internal_memcpy(void* dst, const void* src, unsigned count)
{
    uint8_t* udst = static_cast<uint8_t*>(dst);
    const uint8_t* usrc = static_cast<const uint8_t*>(src);
    for(unsigned ii = 0; (ii < count); ++ii)
    {
        udst[ii] = usrc[ii];
    }
    return dst;
}

/// Internal memset.
///
/// \param ptr Destination pointer.
/// \param value Value to clear to.
/// \param count Number of bytes to clear.
/// \return Destination pointer.
constexpr void* internal_memset(void* ptr, int value, unsigned count)
{
    uint8_t write_value = static_cast<uint8_t>(value);
    uint8_t* udst = static_cast<uint8_t*>(ptr);
    for(unsigned ii = 0; (ii < count); ++ii)
    {
        udst[ii] = write_value;
    }
    return ptr;
}

/// Internal array_new implementation to hide typed information.
///
/// \param ptr Pointer to reallocate. May be nullptr.
/// \param new_size Size in bytes to reallocate.
/// \return (re)Allocated pointer.
inline void* array_new_internal(void* ptr, size_t new_size)
{
#if defined(USE_LD)
    if(!new_size)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("resize to zero not supported"));
    }
#endif
    void* ret = dnload_realloc(ptr, new_size);
#if defined(USE_LD)
    if(!ret)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error("allocating " + std::to_string(new_size) + " bytes failed"));
    }
#endif
    return ret;
}

}

/// Array delete.
///
/// Repolacement for delete[] using free().
/// Use with types that have destructors is not supported.
///
/// \param ptr Pointer to free.
inline void array_delete(void *ptr)
{
    dnload_free(ptr);
}

/// Array new.
///
/// Replacement for new[] using realloc().
/// Use with types that have destructors is not supported.
///
/// \param ptr Existing ptr (may be NULL).
/// \param count Number of elements to allocate.
/// \return Reallocated ptr.
template <typename T> inline T* array_new(T* ptr, size_t count)
{
    void* ret = detail::array_new_internal(ptr, sizeof(T) * count);
    return static_cast<T*>(ret);
}

}

#endif
