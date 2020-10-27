#ifndef VGL_TASK_HPP
#define VGL_TASK_HPP

#include "vgl_fence.hpp"
#include "vgl_unique_ptr.hpp"

namespace vgl
{

namespace detail
{

/// \cond
void internal_fence_data_signal(detail::FenceData&);
/// \endcond

}

/// Task function prototype.
using TaskFunc = void* (*)(void*);

/// Task abstraction.
///
/// Virtual base class.
class Task
{
private:
    /// Pointer to internal fence state to release if set.
    detail::FenceData* m_fence_data = nullptr;

    /// Function pointer to be executed.
    TaskFunc m_func;

    /// Parameter to the function pointer.
    void* m_params;

private:
    /// Deleted copy constructor.
    Task(const Task&) = delete;
    /// Deleted assignment.
    Task& operator=(const Task&) = delete;

public:
    /// Constructor.
    ///
    /// \param func Function for execution.
    /// \param params Parameters to the function.
    constexpr explicit Task(TaskFunc func, void* params) noexcept :
        m_func(func),
        m_params(params)
    {
    }

    /// Constructor.
    ///
    /// \param fence Fence data.
    /// \param func Function for execution.
    /// \param params Parameters to the function.
    constexpr explicit Task(detail::FenceData* fence, TaskFunc func, void* params) noexcept :
        m_fence_data(fence),
        m_func(func),
        m_params(params)
    {
    }

    /// Move constructor.
    ///
    /// \param op Source task.
    constexpr Task(Task&& op) noexcept :
        m_fence_data(op.m_fence_data),
        m_func(op.m_func),
        m_params(op.m_params)
    {
        op.m_fence_data = nullptr;
    }

public:
    /// Destructor.
    ~Task()
    {
        // Mutexes are recursive, so it's safe to lock just for signalling.
        if(m_fence_data)
        {
            detail::internal_fence_data_signal(*m_fence_data);
        }
    }

public:
    /// Gets the function task function.
    ///
    /// The function returned should not be executed, as potential fence data will not be updated.
    ///
    /// \return Task function.
    TaskFunc getFunc() const
    {
        return m_func;
    }

    /// Execute function.
    ///
    /// \return Pointer to function that was executed.
    TaskFunc operator()()
    {
        void* ret = m_func(m_params);
        if(m_fence_data)
        {
            m_fence_data->setReturnValue(ret);
        }
#if defined(USE_LD)
        else if(ret)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("task return value was not handled"));
        }
#endif
        return m_func;
    }

public:
    /// Move operator.
    ///
    /// \param op Source task.
    constexpr Task& operator=(Task&& op) noexcept
    {
        m_fence_data = op.m_fence_data;
        m_func = op.m_func;
        m_params = op.m_params;
        op.m_fence_data = nullptr;
        return *this;
    }
};

}

#endif
