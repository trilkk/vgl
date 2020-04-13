#ifndef VGL_COND_HPP
#define VGL_COND_HPP

#include "vgl_scoped_lock.hpp"

namespace vgl
{

namespace detail
{

/// Internal wait on condition variable.
///
/// \param cond Condition variable.
/// \param mutex Mutex.
void internal_cond_wait(SDL_cond* cond, SDL_mutex* mutex)
{
    int err = dnload_SDL_CondWait(cond, mutex);
#if defined(USE_LD) && defined(DEBUG)
    if(err)
    {
        BOOST_THROW_EXCEPTION(std::runtime_error(std::string("internal_cond_wait(): ") + SDL_GetError()));
    }
#else
    (void)err;
#endif
}

}

/// Condition variable.
class Cond
{
private:
    /// Actual cond.
    SDL_cond *m_cond;

private:
    /// Deleted copy constructor.
    Cond(const Cond&) = delete;
    /// Deleted assignment.
    Cond& operator=(const Cond&) = delete;

public:
    /// Constructor.
    explicit Cond() :
        m_cond(dnload_SDL_CreateCond())
    {
#if defined(USE_LD) && defined(DEBUG)
        if(!m_cond)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("Cond::Cond(): ") + SDL_GetError()));
        }
#endif
    }

    /// Move constructor.
    ///
    /// \param op Source.
    Cond(Cond&& op) :
        m_cond(op.m_cond)
    {
        op.m_cond = nullptr;
    }

    /// Destructor.
    ~Cond()
    {
        if(m_cond)
        {
            dnload_SDL_DestroyCond(m_cond);
        }
    }

public:
    /// Signal the cond, waking all the threads.
    void broadcast()
    {
        int err = dnload_SDL_CondBroadcast(m_cond);
#if defined(USE_LD) && defined(DEBUG)
        if(err)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("Cond::broadcast(): ") + SDL_GetError()));
        }
#else
        (void)err;
#endif
    }

    /// Signal the cond, waking one thread.
    void signal()
    {
        int err = dnload_SDL_CondSignal(m_cond);
#if defined(USE_LD) && defined(DEBUG)
        if(err)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("Cond::signal(): ") + SDL_GetError()));
        }
#else
        (void)err;
#endif
    }

    /// Wait on cond.
    ///
    /// \param op Mutex (already locked).
    void wait(Mutex& op)
    {
        detail::internal_cond_wait(m_cond, op.getMutexImpl());
    }
    /// Wait on scoped lock.
    ///
    /// \param op Scoped lock (already held).
    void wait(ScopedLock& op)
    {
    detail::internal_cond_wait(m_cond, op.getMutexImpl());
    }

public:
    /// Move operator.
    ///
    /// \param op Source.
    /// \return This object.
    Cond& operator=(Cond&& op)
    {
        m_cond = op.m_cond;
        op.m_cond = nullptr;
        return *this;
    }
};

}

#endif
