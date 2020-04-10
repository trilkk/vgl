#ifndef VGL_TASK_HPP
#define VGL_TASK_HPP

#include "vgl_unique_ptr.hpp"

namespace vgl
{

/// Task abstraction.
///
/// Virtual base class.
class Task
{
private:
    /// Condition variable to signal if set.
    Cond* m_cond = nullptr;

protected:
    /// Default constructor.
    explicit Task() = default;

public:
    /// Destructor.
    virtual ~Task() = default;

public:
    /// Execute function.
    void execute()
    {
        executeImpl();

        if(m_cond)
        {
            m_cond->signal();
        }
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
    TaskImpl(const T& op) :
        m_params(op)
    {
    }

    /// Move constructor.
    ///
    /// \param op Source.
    TaskImpl(T&& op) :
        m_params(move(op))
    {
    }

public:
    /// execute() implemetation.
    void execute() override
    {
        m_params();
    }
};

}

/// Task unique pointer type.
using TaskUptr = unique_ptr<Task>;

}

#endif
