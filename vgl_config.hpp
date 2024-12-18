#ifndef VGL_CONFIG_HPP
#define VGL_CONFIG_HPP

/// \file Configuration macro parsing.
///
/// This top-level header should be the first in inclusion chain from all other headers.

#if defined(USE_LD) && USE_LD
/// Compile expecting a linker will be used.
#define VGL_USE_LD
#endif

#if defined(DNLOAD_GLESV2) && DNLOAD_GLESV2
/// Use OpenGL ES as opposed to OpenGL.
#define VGL_USE_GLES
#endif

/// Declarator for functions that allow returning iterators.
///
/// Must be inserted into class public scope.
/// Includes both friend functions and cbegin()/cend() wrappers.
/// Class itself must only declare begin() and end().
///
/// \param Type Type to declare for.
#define VGL_ITERATOR_FUNCTIONS(Type) \
constexpr const_iterator cbegin() const noexcept \
{ \
    return begin(); \
} \
friend constexpr iterator begin(Type& op) noexcept \
{ \
    return op.begin(); \
} \
friend constexpr const_iterator begin(const Type& op) noexcept \
{ \
    return op.begin(); \
} \
friend constexpr const_iterator cbegin(const Type& op) noexcept \
{ \
    return op.begin(); \
} \
constexpr const_iterator cend() const noexcept \
{ \
    return end(); \
} \
friend constexpr iterator end(Type& op) noexcept \
{ \
    return op.end(); \
} \
friend constexpr const_iterator end(const Type& op) noexcept \
{ \
    return op.end(); \
} \
friend constexpr const_iterator cend(const Type& op) noexcept \
{ \
    return op.end(); \
}

#endif
