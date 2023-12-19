#ifndef VGL_SCOPED_RELEASE_HPP
#define VGL_SCOPED_RELEASE_HPP

#include "vgl_scoped_acquire.hpp"

namespace vgl
{

/// Scoped mutex release.
///
/// Unlike ScopedAcquire, the release has not explicit functions for acquiring or releasing the mutex.
class ScopedRelease
{
private:
    /// Mutex that was released.
    Mutex::mutex_type* m_mutex;

public:
    /// Constructor.
    ///
    /// Implicitly releases the mutex.
    ///
    /// \param op Mutex to lock.
    explicit ScopedRelease(Mutex::mutex_type* op) :
        m_mutex(op)
    {
        Mutex::internal_mutex_release(m_mutex);
    }

    /// Constructor.
    ///
    /// Implicitly releases the mutex.
    ///
    /// \param op Mutex to lock.
    explicit ScopedRelease(Mutex& op) :
        ScopedRelease(op.getMutexImpl())
    {
    }

    /// Constructor.
    ///
    /// Implicitly releases the mutex.
    ///
    /// \param op Locked scope.
    explicit ScopedRelease(const ScopedAcquire& op) :
        ScopedRelease(op.getMutexImpl())
    {
    }

    /// Move constructor.
    ///
    /// \param other Source object.
    constexpr ScopedRelease(ScopedRelease&& other) noexcept :
        m_mutex(other.m_mutex)
    {
        other.m_mutex = nullptr;
    }

    /// Destructor.
    ///
    /// Implicitly unlocks the mutex.
    ~ScopedRelease()
    {
        destruct();
    }

    /// Move operator.
    ///
    /// \param other Source object.
    /// \return This object.
    ScopedRelease& operator=(ScopedRelease&& other)
    {
        destruct();
        m_mutex = other.m_mutex;
        other.m_mutex = nullptr;
        return *this;
    }

    /// Deleted copy constructor.
    ScopedRelease(const ScopedRelease&) = delete;
    /// Deleted assignment.
    ScopedRelease& operator=(const ScopedRelease&) = delete;

public:
    /// Accessor.
    ///
    /// \return Referred mutex.
    constexpr Mutex::mutex_type* getMutexImpl() const noexcept
    {
        return m_mutex;
    }

private:
    /// Internal destructor.
    void destruct()
    {
        if(m_mutex)
        {
            Mutex::internal_mutex_acquire(m_mutex);
        }
    }
};

}

#endif
