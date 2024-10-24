#ifndef VGL_REALLOC_HPP
#define VGL_REALLOC_HPP

/// \file Replacements for new/delete.
///
/// As per spec, the operators are not inline, and are found in the correspondig source file.
///
/// Note that only single-object new/delete are replaced. The array versions are not.
/// Containers use array_new and manual constructor calls as needed.

#include "vgl_extern_stdlib.hpp"

#if defined(USE_LD)
#include <stdexcept>
#include <boost/throw_exception.hpp>
#endif

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
        // Can't use vgl_throw_exception.hpp yet.
        BOOST_THROW_EXCEPTION(std::runtime_error("array_new_internal(): resize to zero not supported"));
    }
#endif
    void* ret = dnload_realloc(ptr, new_size);
#if defined(USE_LD)
    if(!ret)
    {
        // Can't use vgl_throw_exception.hpp yet.
        BOOST_THROW_EXCEPTION(std::runtime_error("array_new_internal(): allocating " + std::to_string(new_size) +
                    " bytes failed"));
    }
#endif
    return ret;
}

}

/// Array delete.
///
/// Replacement for delete[] using free().
/// Use with types that have no destructors.
///
/// \param ptr Pointer to free.
inline void array_delete(void *ptr)
{
    dnload_free(ptr);
}

/// Array new.
///
/// Replacement for new[] using realloc().
/// Use with types that have no destructors.
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

#if !defined(USE_LD)
#include "vgl_realloc.cpp"
#endif

#endif
