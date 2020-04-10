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
    Mutex* m_mutex;

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
        m_mutex(&op)
    {
        op.acquire();
    }

    /// Move constructor.
    ///
    /// \param op Source.
    ScopedLock(ScopedLock&& op) :
        m_mutex(op.m_mutex)
    {
        op.m_mutex = nullptr;
    }

    /// Destructor.
    ///
    /// Implicitly unlocks the mutex.
    ~ScopedLock()
    {
        if(m_mutex)
        {
            m_mutex->release();
        }
    }

public:
    /// Accessor.
    ///
    /// \return Referred mutex.
    constexpr Mutex& getMutex()
    {
        return *m_mutex;
    }

public:
    /// Move operator.
    ///
    /// \param op Source.
    /// \return This object.
    ScopedLock& operator=(ScopedLock&& op)
    {
        m_mutex = op.m_mutex;
        op.m_mutex = nullptr;
        return *this;
    }
};

}

#endif
