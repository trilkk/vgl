#ifndef VGL_COND_HPP
#define VGL_COND_HPP

#include "vgl_scoped_acquire.hpp"

namespace vgl
{

/// Condition variable.
class Cond
{
public:
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
    cond_type* m_cond;

private:
    /// Deleted copy constructor.
    Cond(const Cond&) = delete;
    /// Deleted assignment.
    Cond& operator=(const Cond&) = delete;

public:
    /// Constructor.
    explicit Cond() :
#if defined(VGL_ENABLE_GTK)
        m_cond(new cond_type)
#else
        m_cond(dnload_SDL_CreateCond())
#endif
    {
#if defined(VGL_ENABLE_GTK)
        dnload_g_cond_init(m_cond);
#elif defined(USE_LD) && defined(DEBUG)
        if(!m_cond)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error(std::string("Cond::Cond(): ") + SDL_GetError()));
        }
#endif
    }

    /// Constructor.
    ///
    /// \param op Internal type to adapt.
    constexpr Cond(cond_type* op) noexcept :
        m_cond(op)
    {
    }

    /// Destructor.
    ~Cond()
    {
        destruct();
    }

    /// Move constructor.
    ///
    /// \param other Source object.
    constexpr Cond(Cond&& other) noexcept :
        Cond(other.m_cond)
    {
        other.m_cond = nullptr;
    }

public:
#if defined(USE_LD)
    /// Accessor.
    ///
    /// \return Internal implementation.
    constexpr cond_type* getCondImpl() const noexcept
    {
        return m_cond;
    }
#endif

    /// Signal the cond, waking all the threads.
    void broadcast() const
    {
#if defined(VGL_ENABLE_GTK)
        dnload_g_cond_broadcast(m_cond);
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
    void signal() const
    {
#if defined(VGL_ENABLE_GTK)
        dnload_g_cond_broadcast(m_cond);
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
    void wait(const Mutex& op) const
    {
        wait(op.getMutexImpl());
    }
    /// Wait on scoped lock.
    ///
    /// \param op Scoped lock (already held).
    void wait(const ScopedAcquire& op) const
    {
        wait(op.getMutexImpl());
    }

private:
    /// Internal destructor.
    void destruct()
    {
        if(m_cond)
        {
#if defined(VGL_ENABLE_GTK)
            dnload_g_cond_clear(m_cond);
            delete m_cond;
#else
            dnload_SDL_DestroyCond(m_cond);
#endif
        }
    }

    /// Internal wait on condition variable.
    ///
    /// \param cond Condition variable.
    /// \param mutex Mutex.
    void wait(Mutex::mutex_type* mutex) const
    {
#if defined(VGL_ENABLE_GTK)
        dnload_g_cond_wait(m_cond, mutex);
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

public:
    /// Move operator.
    ///
    /// \param other Source object.
    /// \return This object.
    Cond& operator=(Cond&& other)
    {
        destruct();
        m_cond = other.m_cond;
        other.m_cond = nullptr;
        return *this;
    }

public:
#if defined(USE_LD)
    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const Cond& rhs)
    {
        return lhs << "Cond(" << rhs.getCondImpl() << ")";
    }
#endif
};

}

#endif
