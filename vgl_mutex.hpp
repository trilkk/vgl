#ifndef VGL_MUTEX_HPP
#define VGL_MUTEX_HPP

#include "vgl_extern_sdl.hpp"

namespace vgl
{

namespace detail
{

/// Acquire mutex implementation.
///
/// \param op Mutex implementation.
void internal_mutex_acquire(SDL_mutex* op)
{
    int err = dnload_SDL_LockMutex(op);
#if defined(USE_LD) && defined(DEBUG)
    if(err)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error(std::string("internal_mutex_acquire(): ") + SDL_GetError()));
    }
#else
    (void)err;
#endif
}

/// Release mutex implementation.
///
/// \param op Mutex implementation.
void internal_mutex_release(SDL_mutex* op)
{
    int err = dnload_SDL_UnlockMutex(op);
#if defined(USE_LD) && defined(DEBUG)
    if(err)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error(std::string("internal_mutex_release(): ") + SDL_GetError()));
    }
#else
    (void)err;
#endif
}

}

/// Mutex class.
class Mutex
{
private:
    /// Actual mutex.
    SDL_mutex *m_mutex;

private:
    /// Deleted copy constructor.
    Mutex(const Mutex&) = delete;
    /// Deleted assignment.
    Mutex& operator=(const Mutex&) = delete;

public:
    /// Constructor.
    explicit Mutex() :
        m_mutex(dnload_SDL_CreateMutex())
    {
#if defined(USE_LD) && defined(DEBUG)
        if(!m_mutex)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("Mutex::Mutex(): ") + SDL_GetError()));
        }
#endif
    }

    /// Move constructor.
    ///
    /// \param op Source.
    constexpr Mutex(Mutex&& op) noexcept :
        m_mutex(op.m_mutex)
    {
        op.m_mutex = nullptr;
    }

    /// Destructor.
    ~Mutex()
    {
        if(m_mutex)
        {
            dnload_SDL_DestroyMutex(m_mutex);
        }
    }

public:
    /// Accessor.
    ///
    /// \return Inner mutex.
    constexpr SDL_mutex* getMutexImpl() const
    {
        return m_mutex;
    }

    /// Lock.
    void acquire()
    {
        detail::internal_mutex_acquire(m_mutex);
    }

    /// Unlock.
    void release()
    {
        detail::internal_mutex_release(m_mutex);
    }

public:
    /// Move operator.
    ///
    /// \param op Source.
    /// \return This object.
    constexpr Mutex& operator=(Mutex&& op) noexcept
    {
        m_mutex = op.m_mutex;
        op.m_mutex = nullptr;
        return *this;
    }
};

}

#endif
