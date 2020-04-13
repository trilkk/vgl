#ifndef VGL_TASK_QUEUE_HPP
#define VGL_TASK_QUEUE_HPP

#include "vgl_queue.hpp"
#include "vgl_task.hpp"

namespace vgl
{

namespace detail
{

/// Task queue class.
class TaskQueue
{
private:
    /// Queue for tasks.
    queue<TaskUptr> m_tasks;

    /// Condition variable to be signalled when the task queue is modified.
    unique_ptr<Cond> m_cond;

private:
    /// Deleted copy constructor.
    TaskQueue(const TaskQueue&) = delete;
    /// Deleted assignment.
    TaskQueue& operator=(const TaskQueue&) = delete;

public:
    /// Default constructor.
    constexpr explicit TaskQueue() = default;

public:
    /// Initialize the task queue.
    void initialize()
    {
        m_cond.reset(new Cond());
    }

    /// Uninitialize the task queue.
    void uninitialize()
    {
        m_cond->broadcast();
#if defined(USE_LD)
        while(!m_tasks.empty())
        {
            m_tasks.pop();
        }
#endif
    }

    /// Signal on the task queue.
    void signal()
    {
        m_cond->signal();
    }

    /// Wait on the task queue.
    ///
    /// \param op Locked scope.
    void wait(ScopedLock& sl)
    {
        m_cond->wait(sl);
    }

    /// Tell if the task queue is empty.
    ///
    /// \return True if empty, false otherwise.
    constexpr bool empty() const
    {
        return m_tasks.empty();
    }

    /// Acquire from the task queue.
    ///
    /// \return Task.
    TaskUptr acquire()
    {
        TaskUptr ret(m_tasks.front().release());
        m_tasks.pop();
        return ret;
    }

    /// Emplace into the task queue.
    ///
    /// Implicitly signals.
    ///
    /// \param op New task.
    void emplace(Task* op)
    {
        m_tasks.emplace(op);
        m_cond->signal();
    }
};

}

}

#endif
