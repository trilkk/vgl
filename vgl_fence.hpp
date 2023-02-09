#ifndef VGL_FENCE_HPP
#define VGL_FENCE_HPP

#include "vgl_cond.hpp"
#include "vgl_scoped_lock.hpp"
#include "vgl_unique_ptr.hpp"

namespace vgl
{

namespace detail
{

/// \cond
class FenceData;
void* internal_fence_data_wait(FenceData* op);
/// \endcond

/// Internal fence state.
class FenceData
{
private:
    /// Condition variable for this fence.
    Cond m_cond;

    /// Return value from an associated function.
    void* m_return_value = nullptr;

    /// Is the fence still active?
    bool m_active = true;

private:
    /// Deleted copy constructor.
    FenceData(const FenceData&) = delete;
    /// Deleted assignment.
    FenceData& operator=(const FenceData&) = delete;

public:
    /// Default constructor.
    explicit FenceData() = default;

public:
    /// Accessor.
    ///
    /// \return Return value stored in the fence data.
    constexpr void* getReturnValue() const
    {
        return m_return_value;
    }
    /// Setter.
    ///
    /// \param op Return value.
    constexpr void setReturnValue(void* ret)
    {
        m_return_value = ret;
    }

    /// Is the fence still active?
    ///
    /// \return True if fence is still active.
    constexpr bool isActive() const
    {
        return m_active;
    }
    /// Setter.
    ///
    /// \param op New active status flag.
    constexpr void setActive(bool op)
    {
        m_active = op;
    }

    /// Signal anyone waiting on the fence.
    void signal()
    {
        m_cond.signal();
    }

    /// Wait on the fence.
    ///
    /// \param op Locked scope.
    void wait(ScopedLock& op)
    {
        m_cond.wait(op);
    }
};

/// Internal fence state unique pointer type.
using FenceDataUptr = unique_ptr<FenceData>;

}

/// Fence prevents execution from moving forward until someone has cleared it.
class Fence
{
private:
    /// Internal fence state.
    detail::FenceData* m_fence_data;

private:
    /// Deleted copy constructor.
    Fence(const Fence&) = delete;
    /// Deleted assignment.
    Fence& operator=(const Fence&) = delete;

public:
    /// Constructor.
    ///
    /// \param op Mutex.
    explicit Fence(detail::FenceData* op) :
        m_fence_data(op)
    {
    }

    /// Move constructor.
    ///
    /// \param op Source.
    constexpr Fence(Fence&& op) noexcept :
        m_fence_data(op.m_fence_data)
    {
        op.m_fence_data = nullptr;
    }

    /// Destructor.
    ~Fence()
    {
        if(m_fence_data)
        {
            void* ret = detail::internal_fence_data_wait(m_fence_data);
#if defined(USE_LD)
            if(ret)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("fence being destructed has unhandled return value"));
            }
#else
            (void)ret;
#endif
        }
    }

public:
    /// Accessor.
    ///
    /// \return Fence data.
    constexpr detail::FenceData* getFenceData() const
    {
        return m_fence_data;
    }

    /// Wait until the fence has executed and get the return value.
    ///
    /// \return Return value from the function.
    void* getReturnValue()
    {
#if defined(USE_LD)
        if(!m_fence_data)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("fence data has already been cleared"));
        }
#endif
        void* ret = detail::internal_fence_data_wait(m_fence_data);
        m_fence_data = nullptr;
        return ret;
    }

public:    
    /// Move operator.
    ///
    /// \param op Source.
    constexpr Fence& operator=(Fence&& op) noexcept
    {
        m_fence_data = op.m_fence_data;
        op.m_fence_data = nullptr;
        return *this;
    }
};

}

#endif
