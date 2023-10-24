#ifndef VGL_MUTEX_HPP
#define VGL_MUTEX_HPP

#if defined(VGL_ENABLE_GTK)
#include "vgl_extern_gtk.hpp"
#else
#include "vgl_extern_sdl.hpp"
#endif

namespace vgl
{

/// Mutex class.
class Mutex
{
public:
    /// Internal mutex implementation.
    using mutex_type =
#if defined(VGL_ENABLE_GTK)
        GMutex
#else
        SDL_mutex
#endif
        ;

private:
    /// Actual mutex.
    mutex_type
#if !defined(VGL_ENABLE_GTK)
        *
#endif
        m_mutex;

private:
    /// Deleted copy constructor.
    Mutex(const Mutex&) = delete;
    /// Deleted move constructor.
    Mutex(Mutex&&) = delete;
    /// Deleted assignment.
    Mutex& operator=(const Mutex&) = delete;
    /// Deleted move.
    Mutex& operator=(Mutex&&) = delete;

public:
    /// Constructor.
    explicit Mutex()
#if !defined(VGL_ENABLE_GTK)
        : m_mutex(dnload_SDL_CreateMutex())
#endif
    {
#if defined(VGL_ENABLE_GTK)
        dnload_g_mutex_init(&m_mutex);
#endif
#if defined(USE_LD) && defined(DEBUG)
        if(!m_mutex)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("Mutex::Mutex(): ") + SDL_GetError()));
        }
#endif
    }

    /// Destructor.
    ~Mutex()
    {
#if defined(VGL_ENABLE_GTK)
        dnload_g_mutex_clear(&m_mutex);
#else
        VGL_ASSERT(m_mutex);
        dnload_SDL_DestroyMutex(m_mutex);
#endif
    }

public:
    /// Accessor.
    ///
    /// \return Inner mutex.
    constexpr mutex_type* getMutexImpl()
#if !defined(VGL_ENABLE_GTK)
        const
#endif
    {
        return
#if defined(VGL_ENABLE_GTK)
            &
#endif
            m_mutex;
    }

    /// Lock.
    void acquire()
    {
        internal_mutex_acquire(getMutexImpl());
    }

    /// Unlock.
    void release()
    {
        internal_mutex_release(getMutexImpl());
    }

public:
    /// Acquire mutex implementation.
    ///
    /// \param op Mutex implementation.
    static inline void internal_mutex_acquire(mutex_type* op)
    {
#if defined(VGL_ENABLE_GTK)
        dnload_g_mutex_lock(op);
#else
        int err = dnload_SDL_LockMutex(op);
#if defined(USE_LD) && defined(DEBUG)
        if(err)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("internal_mutex_acquire(): ") + SDL_GetError()));
        }
#else
        (void)err;
#endif
#endif
    }

    /// Release mutex implementation.
    ///
    /// \param op Mutex implementation.
    static inline void internal_mutex_release(mutex_type* op)
    {
#if defined(VGL_ENABLE_GTK)
        dnload_g_mutex_unlock(op);
#else
        int err = dnload_SDL_UnlockMutex(op);
#if defined(USE_LD) && defined(DEBUG)
        if(err)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("internal_mutex_release(): ") + SDL_GetError()));
        }
#else
        (void)err;
#endif
#endif
    }
};

}

#endif
