#ifndef VGL_FENCE_HPP
#define VGL_FENCE_HPP

#include "vgl_assert.hpp"
#include "vgl_cond.hpp"
#include "vgl_scoped_acquire.hpp"
#include "vgl_unique_ptr.hpp"

namespace vgl
{

/// \cond
class Fence;
/// \endcond

namespace detail
{

/// \cond
void* internal_fence_wait(Fence& op);
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
#if defined(USE_LD)
    /// Accessor.
    ///
    /// \return Internal implementation.
    constexpr Cond::cond_type* getCondImpl() const noexcept
    {
        return m_cond.getCondImpl();
    }
#endif

    /// Accessor.
    ///
    /// \return Return value stored in the fence data.
    constexpr void* getReturnValue() const noexcept
    {
        return m_return_value;
    }
    /// Setter.
    ///
    /// \param op Return value.
    constexpr void setReturnValue(void* ret) noexcept
    {
        m_return_value = ret;
    }

    /// Is the fence still active?
    ///
    /// \return True if fence is still active.
    constexpr bool isActive() const noexcept
    {
        return m_active;
    }
    /// Setter.
    ///
    /// \param op New active status flag.
    constexpr void setActive(bool op) noexcept
    {
        m_active = op;
    }

    /// Signal anyone waiting on the fence.
    void signal() const
    {
        m_cond.signal();
    }

    /// Wait on the fence.
    ///
    /// \param op Locked scope.
    void wait(const ScopedAcquire& op) const
    {
        m_cond.wait(op);
    }

public:
#if defined(USE_LD)
    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const FenceData& rhs)
    {
        return lhs << "FenceData(" << rhs.getCondImpl() << ")";
    }
#endif
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
    constexpr explicit Fence(detail::FenceData* op) noexcept :
        m_fence_data(op)
    {
    }

    /// Move constructor.
    ///
    /// \param other Source object.
    constexpr Fence(Fence&& other) noexcept :
        Fence(other.m_fence_data)
    {
        other.m_fence_data = nullptr;
    }

    /// Destructor.
    ~Fence()
    {
        destruct();
    }

public:
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
        void* ret = detail::internal_fence_wait(*this);
        VGL_ASSERT(!m_fence_data);
        return ret;
    }

    /// Release the fence data being held in this fence.
    ///
    /// \return Fence data pointer.
    constexpr detail::FenceData* releaseData() noexcept
    {
        detail::FenceData* ret = m_fence_data;
        m_fence_data = nullptr;
        VGL_ASSERT(ret);
        return ret;
    }

    /// Signal anyone waiting on the fence.
    void signal() const
    {
        VGL_ASSERT(m_fence_data);
        m_fence_data->signal();
    }

    /// Wait on the fence.
    ///
    /// \param op Locked scope.
    void wait(ScopedAcquire& op) const
    {
        VGL_ASSERT(m_fence_data);
        m_fence_data->wait(op);
    }

private:
    /// Internal destructor.
    void destruct()
    {
        if(m_fence_data)
        {
            void* ret = detail::internal_fence_wait(*this);
#if defined(USE_LD) && defined(DEBUG)
            if(ret)
            {
                std::cerr << "Fence::destruct(): unhandled return value '" << ret << "'" << std::endl;
            }
#else
            (void)ret;
#endif
            VGL_ASSERT(!m_fence_data);
        }
    }

public:    
    /// Move operator.
    ///
    /// \param other Source object.
    /// \return This object.
    Fence& operator=(Fence&& other)
    {
        destruct();
        m_fence_data = other.m_fence_data;
        other.m_fence_data = nullptr;
        return *this;
    }

    /// Bool operator.
    ///
    /// \return Flag indicating if the fence is still active.
    constexpr operator bool() const noexcept
    {
        VGL_ASSERT(m_fence_data);
        return m_fence_data->isActive();
    }

public:
#if defined(USE_LD)
    /// Stream output operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Output stream.
    friend std::ostream& operator<<(std::ostream& lhs, const Fence& rhs)
    {
        return lhs << "Fence(" << rhs.m_fence_data->getCondImpl() << ")";
    }
#endif
};

}

#endif
