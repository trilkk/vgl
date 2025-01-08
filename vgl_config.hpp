#ifndef VGL_CONFIG_HPP
#define VGL_CONFIG_HPP

/// \file Configuration macro parsing.
///
/// This top-level header should be the first in inclusion chain from all other headers. All configuration macros
/// should be declared before including vgl headers (that will eventually include this file). To avoid surprising
/// errors when using multiple source files, preferably declare the macros using the build system.
///
/// All options are default off. The following options are recognized:
///
/// - VGL_DISABLE_CEIL
///
///   Disable ceil() function in math. May decreases code footprint.
///
/// - VGL_DISABLE_DEPTH_WRITE
///
///   Disable ability to toggle depth write. Decreases code footprint.
///
/// - VGL_DISABLE_DEPTH_TEXTURE
///
///   Disable support for depth textures. Increases code footprint, but may increase performance by using
///   renderbuffers as opposed to textures for all FBO depth buffers. Use on platforms with limited support for depth
///   texture formats.
///
/// - VGL_DISABLE_EDGE
///
///   Disable support for edge buffers (and by extension, stencil shadows) in meshes. Decreases code footprint.
///
///   TODO: This feature is currently unsupported.
///
/// - VGL_DISABLE_OGG
///
///   Disable support for ogg containers for opus playback. Decreases code footprint, but requires custom opus data
///   generated using the opus2raw binary.
///
/// - VGL_DISABLE_OPUS
///
///   Disable support for opus. Decreases code footprint.
///
/// - VGL_DISABLE_STENCIL
///
///   Disable support for stencil buffer. Decreases code footprint.
///
/// - VGL_ENABLE_GTK
///
///   Enable support for GTK, mainly for implementing concurrency primitives. If not set, SDL is used instead.
///
/// - VGL_ENABLE_VERTEX_NORMAL_PACKING
///
///   Pack vertex normals into normalized integers. Increases code footprint but may also increase performance due to
///   reduced memory usage. Performance may be decreased due to vertex attribute misalignment on some platforms.
///
/// - VGL_USE_BONE_STATE_FULL_TRANSFORM
///
///   Interpolate complete transformation matrices and renormalize them after interpolation as opposed to
///   interpolating quaternions. Increases code footprint.
///
///   TODO: Should not be required but something in the bone animation is buggy.
///
/// - VGL_USE_LD
///
///   Compile assuming a real linker is in use. Should be enabled whenever not doing minified builds.

#if defined(USE_LD) && USE_LD
/// Compile expecting a linker will be used.
#define VGL_USE_LD
#endif

#if defined(DNLOAD_GLESV2) && DNLOAD_GLESV2
/// Use OpenGL ES as opposed to OpenGL.
#define VGL_USE_GLES
#endif

#if !defined(VGL_DISABLE_EDGE)
/// Edge buffers are not currently supported.
#define VGL_DISABLE_EDGE
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
