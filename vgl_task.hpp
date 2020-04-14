#ifndef VGL_TASK_HPP
#define VGL_TASK_HPP

#include "vgl_fence.hpp"
#include "vgl_unique_ptr.hpp"

namespace vgl
{

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
    explicit Task(TaskFunc func, void* params) :
        m_func(func),
        m_params(params)
    {
    }

    /// Constructor.
    ///
    /// \param fence Fence data.
    /// \param func Function for execution.
    /// \param params Parameters to the function.
    explicit Task(detail::FenceData* fence, TaskFunc func, void* params) :
        m_fence_data(fence),
        m_func(func),
        m_params(params)
    {
    }

    /// Move constructor.
    ///
    /// \param op Source task.
    Task(Task&& op) :
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
        if(m_fence_data)
        {
            m_fence_data->signal();
        }
    }

public:
    /// Execute function.
    ///
    /// \return Pointer to function that was executed.
    TaskFunc operator()()
    {
        void* ret = m_func(this);
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
    Task& operator=(Task&& op)
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
