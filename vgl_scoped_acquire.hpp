#ifndef VGL_SCOPED_ACQUIRE_HPP
#define VGL_SCOPED_ACQUIRE_HPP

#include "vgl_mutex.hpp"

namespace vgl
{

/// Scoped mutex acquire.
///
/// Can be released and re-acquired manually. Consider using ScopedRelease.
class ScopedAcquire
{
private:
    /// Mutex that was acquired.
    Mutex::mutex_type* m_mutex;

public:
    /// Constructor.
    ///
    /// Implicitly acquires the mutex.
    ///
    /// \param op Mutex to lock.
    explicit ScopedAcquire(Mutex::mutex_type* op) :
        m_mutex(op)
    {
        acquire();
    }

    /// Constructor.
    ///
    /// Implicitly acquires the mutex.
    ///
    /// \param op Mutex to lock.
    explicit ScopedAcquire(Mutex& op) :
        ScopedAcquire(op.getMutexImpl())
    {
    }

    /// Move constructor.
    ///
    /// \param other Source object.
    constexpr ScopedAcquire(ScopedAcquire&& other) noexcept :
        m_mutex(other.m_mutex)
    {
        other.m_mutex = nullptr;
    }

    /// Destructor.
    ///
    /// Implicitly unlocks the mutex.
    ~ScopedAcquire()
    {
        destruct();
    }

    /// Move operator.
    ///
    /// \param other Source object.
    /// \return This object.
    ScopedAcquire& operator=(ScopedAcquire&& other)
    {
        destruct();
        m_mutex = other.m_mutex;
        other.m_mutex = nullptr;
        return *this;
    }

    /// Deleted copy constructor.
    ScopedAcquire(const ScopedAcquire&) = delete;
    /// Deleted assignment.
    ScopedAcquire& operator=(const ScopedAcquire&) = delete;

public:
    /// Accessor.
    ///
    /// \return Referred mutex.
    constexpr Mutex::mutex_type* getMutexImpl() const noexcept
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
};

}

#endif
