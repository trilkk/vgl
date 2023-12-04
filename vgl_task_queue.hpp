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
    queue<Task> m_tasks;

    /// Condition variable to be signalled when the task queue is modified.
    Cond m_cond = Cond(nullptr);

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
        m_cond = Cond();
    }

    /// Uninitialize the task queue.
    void uninitialize()
    {
        m_cond.broadcast();
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
        m_cond.signal();
    }

    /// Wait on the task queue.
    ///
    /// \param op Locked scope.
    void wait(ScopedLock& sl)
    {
        m_cond.wait(sl);
    }

    /// Gets the number of tasks in the queue.
    ///
    /// \return Number of tasks in the queue.
    constexpr unsigned size() const noexcept
    {
        return m_tasks.size();
    }

    /// Tell if the task queue is empty.
    ///
    /// \return True if empty, false otherwise.
    constexpr bool empty() const noexcept
    {
        return m_tasks.empty();
    }

    /// Acquire from the task queue.
    ///
    /// \return Task.
    Task acquire()
    {
        Task ret = move(m_tasks.front());
        m_tasks.pop();
        return ret;
    }

    /// Emplace into the task queue.
    ///
    /// Implicitly signals.
    ///
    /// \param func Function to dispatch.
    /// \param params Function parameters.
    void emplace(TaskFunc func, void* params)
    {
        m_tasks.emplace(func, params);
        m_cond.signal();
    }
    /// Emplace into the task queue.
    ///
    /// Implicitly signals.
    ///
    /// \param fence_data Fence data to use with the task.
    /// \param func Function to dispatch.
    /// \param params Function parameters.
    void emplace(detail::FenceData* fence_data, TaskFunc func, void* params)
    {
        m_tasks.emplace(fence_data, func, params);
        m_cond.signal();
    }
};

}

}

#endif
