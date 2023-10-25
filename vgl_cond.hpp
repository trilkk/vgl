#ifndef VGL_COND_HPP
#define VGL_COND_HPP

#include "vgl_scoped_lock.hpp"

namespace vgl
{

/// Condition variable.
class Cond
{
    /// Internal mutex implementation.
    using cond_type =
#if defined(VGL_ENABLE_GTK)
        GCond
#else
        SDL_cond
#endif
        ;

private:
    /// Actual cond.
    cond_type
#if !defined(VGL_ENABLE_GTK)
        *
#endif
        m_cond;

private:
    /// Deleted copy constructor.
    Cond(const Cond&) = delete;
    /// Deleted move constructor.
    Cond(Cond&&) = delete;
    /// Deleted assignment.
    Cond& operator=(const Cond&) = delete;
    /// Deleted move.
    Cond& operator=(Cond&&) = delete;

public:
    /// Constructor.
    explicit Cond()
#if !defined(VGL_ENABLE_GTK)
        : m_cond(dnload_SDL_CreateCond())
#endif
    {
#if defined(VGL_ENABLE_GTK)
        dnload_g_cond_init(&m_cond);
#elif defined(USE_LD) && defined(DEBUG)
        if(!m_cond)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("Cond::Cond(): ") + SDL_GetError()));
        }
#endif
    }

    /// Destructor.
    ~Cond()
    {
#if defined(VGL_ENABLE_GTK)
        dnload_g_cond_clear(&m_cond);
#else
        VGL_ASSERT(m_mutex);
        dnload_SDL_DestroyCond(m_cond);
#endif
    }

public:
    /// Signal the cond, waking all the threads.
    void broadcast()
    {
#if defined(VGL_ENABLE_GTK)
        dnload_g_cond_broadcast(&m_cond);
#else
        int err = dnload_SDL_CondBroadcast(m_cond);
#if defined(USE_LD) && defined(DEBUG)
        if(err)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("Cond::broadcast(): ") + SDL_GetError()));
        }
#else
        (void)err;
#endif
#endif
    }

    /// Signal the cond, waking one thread.
    void signal()
    {
#if defined(VGL_ENABLE_GTK)
        dnload_g_cond_broadcast(&m_cond);
#else
        int err = dnload_SDL_CondSignal(m_cond);
#if defined(USE_LD) && defined(DEBUG)
        if(err)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("Cond::signal(): ") + SDL_GetError()));
        }
#else
        (void)err;
#endif
#endif
    }

    /// Wait on cond.
    ///
    /// \param op Mutex (already locked).
    void wait(Mutex& op)
    {
        wait(op.getMutexImpl());
    }
    /// Wait on scoped lock.
    ///
    /// \param op Scoped lock (already held).
    void wait(ScopedLock& op)
    {
        wait(op.getMutexImpl());
    }

private:
    /// Internal wait on condition variable.
    ///
    /// \param cond Condition variable.
    /// \param mutex Mutex.
    void wait(Mutex::mutex_type* mutex)
    {
#if defined(VGL_ENABLE_GTK)
        dnload_g_cond_wait(&m_cond, mutex);
#else
        int err = dnload_SDL_CondWait(m_cond, mutex);
#if defined(USE_LD) && defined(DEBUG)
        if(err)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("internal_cond_wait(): ") + SDL_GetError()));
        }
#else
        (void)err;
#endif
#endif
    }
};

}

#endif
