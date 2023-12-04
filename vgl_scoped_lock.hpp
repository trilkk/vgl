#ifndef VGL_SCOPED_LOCK_HPP
#define VGL_SCOPED_LOCK_HPP

#include "vgl_mutex.hpp"

namespace vgl
{

/// Scoped lock.
class ScopedLock
{
private:
    /// Mutex that was locked.
    Mutex::mutex_type* m_mutex;

private:
    /// Deleted copy constructor.
    ScopedLock(const ScopedLock&) = delete;
    /// Deleted assignment.
    ScopedLock& operator=(const ScopedLock&) = delete;

public:
    /// Constructor.
    ///
    /// Implicitly locks the mutex.
    ///
    /// \param op Mutex to lock.
    explicit ScopedLock(Mutex& op) :
        m_mutex(op.getMutexImpl())
    {
        acquire();
    }

    /// Move constructor.
    ///
    /// \param op Source.
    constexpr ScopedLock(ScopedLock&& op) noexcept :
        m_mutex(op.m_mutex)
    {
        op.m_mutex = nullptr;
    }

    /// Destructor.
    ///
    /// Implicitly unlocks the mutex.
    ~ScopedLock()
    {
        destruct();
    }

public:
    /// Accessor.
    ///
    /// \return Referred mutex.
    constexpr Mutex::mutex_type* getMutexImpl()
    {
        return m_mutex;
    }

    /// Lock.
    void acquire()
    {
        Mutex::internal_mutex_acquire(m_mutex);
    }

    /// Unlock.
    void release()
    {
        Mutex::internal_mutex_release(m_mutex);
    }

private:
    /// Internal destructor.
    void destruct()
    {
        if(m_mutex)
        {
            release();
        }
    }

public:
    /// Move operator.
    ///
    /// \param op Source.
    /// \return This object.
    ScopedLock& operator=(ScopedLock&& op)
    {
        destruct();
        m_mutex = op.m_mutex;
        op.m_mutex = nullptr;
        return *this;
    }
};

}

#endif
