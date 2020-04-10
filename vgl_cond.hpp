#ifndef VGL_COND_HPP
#define VGL_COND_HPP

#include "vgl_scoped_lock.hpp"

namespace vgl
{

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
    /// \param mutex Mutex (already locked).
    void wait(Mutex& mutex)
    {
        int err = dnload_SDL_CondWait(m_cond, mutex.getInnerMutex());
#if defined(USE_LD) && defined(DEBUG)
        if(err)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("Cond::wait(): ") + SDL_GetError()));
        }
#else
        (void)err;
#endif
    }

    /// Wait on scoped lock.
    ///
    /// \param scoped_lock Scoped lock (already held).
    void wait(ScopedLock& scoped_lock)
    {
        wait(scoped_lock.getMutex());
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
