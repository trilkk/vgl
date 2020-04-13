#ifndef VGL_TASK_HPP
#define VGL_TASK_HPP

#include "vgl_fence.hpp"
#include "vgl_unique_ptr.hpp"

namespace vgl
{

/// Task abstraction.
///
/// Virtual base class.
class Task
{
private:
    /// Pointer to internal fence state to release if set.
    detail::FenceData* m_fence_data = nullptr;

protected:
    /// Default constructor.
    explicit Task() = default;

public:
    /// Destructor.
    virtual ~Task()
    {
        if(m_fence_data)
        {
            m_fence_data->signal();
        }
    }

public:
    /// Setter.
    ///
    /// \param op Pointer to fence data.
    constexpr void setFenceData(detail::FenceData* op)
    {
        m_fence_data = op;
    }

    /// Execute function.
    void execute()
    {
        executeImpl();
    }

protected:
    /// Virtual execute function.
    virtual void executeImpl() = 0;
};

namespace detail
{

/// Specialized task parameters.
template<typename T> class TaskImpl : public Task
{
private:
    /// Parameters.
    T m_params;

private:
    /// Deleted copy constructor.
    TaskImpl(const TaskImpl&) = delete;
    /// Deleted assignment.
    TaskImpl& operator=(const TaskImpl&) = delete;

public:
    /// Constructor.
    ///
    /// \param op Source.
    constexpr TaskImpl(const T& op) :
        m_params(op)
    {
    }

    /// Move constructor.
    ///
    /// \param op Source.
    constexpr TaskImpl(T&& op) :
        m_params(move(op))
    {
    }

public:
    /// execute() implemetation.
    void executeImpl() override
    {
        m_params();
    }
};

}

/// Task unique pointer type.
using TaskUptr = unique_ptr<Task>;

}

#endif
