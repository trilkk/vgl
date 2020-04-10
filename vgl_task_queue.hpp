#ifndef VGL_TASK_QUEUE_HPP
#define VGL_TASK_QUEUE_HPP

#include "vgl_cond.hpp"
#include "vgl_queue.hpp"
#include "vgl_task.hpp"
#include "vgl_thread.hpp"
#include "vgl_vector.hpp"

namespace vgl
{

namespace detail
{

/// Task queue class.
class TaskQueue
{
private:
    /// Threads created for this task queue. May be empty.
    vector<Thread> m_threads;

    /// Queue for tasks.
    queue<TaskUptr> m_tasks;

    /// Free condition variables for tasks dispatching with wait.
    vector<Cond> m_cond_pool_free;

    /// Condition variables being waited on by tasks dispatched with wait.
    vector<Cond> m_cond_pool_used;

    /// Guard mutex.
    unique_ptr<Mutex> m_mutex;

    /// Main condition variable.
    unique_ptr<Cond> m_cond;

    /// Target concurrency level.
    unsigned m_concurrency = 0;

    /// Number of threads active currently.
    unsigned m_threads_active = 0;

    /// Flag signifying the task queue is being destroyed.
    bool m_quitting = false;

public:
    /// Default constructor.
    constexpr explicit TaskQueue() = default;

    /// Destructor.
    ~TaskQueue()
    {
        m_quitting = true;
        
        ScopedLock sl(*m_mutex);
        m_cond->broadcast();
    }

private:
    /// Thread function.
    void threadFunc()
    {
        ScopedLock sl(*m_mutex);

        while(!m_quitting)
        {
            if((m_threads_active < m_concurrency) && !m_tasks.empty())
            {
                ++m_threads_active;
                
                TaskUptr task = move(m_tasks.front());
                task->execute();

                --m_threads_active;
            }
            else
            {
                m_cond->wait(*m_mutex);
            }
        }
    }

public:
    /// Initialize the task queue.
    ///
    /// \param op Number of threads to initialize.
    void initialize(unsigned op)
    {
        m_concurrency = 0;

        m_mutex = new Mutex();
        m_cond = new Cond();

        while(op)
        {
            m_threads.emplace_back(task_thread_func, this);
        }
    }

    /// Dispatch a task.
    ///
    /// \param task Newly created task.
    void dispatch(Task* task)
    {
        ScopedLock sl(*m_mutex);
        m_tasks.emplace_back(task);
        m_cond->signal();
    }

private:
    /// Task dispatcher thread.
    ///
    /// \param op Pointer to task queue.
    static int task_thread_func(void* op)
    {
        TaskQueue* task_queue = static_cast<TaskQueue*>(op);
        task_queue->threadFunc();
        return 0;
    }
};

/// Queue for tasks.
TaskQueue g_tasks_any;

/// Queue for tasks for the main thread.
TaskQueue g_tasks_main;

}

/// Dispatch task to any thread.
template<typename T> void dispatch_any(const T& op)
{
    detail::g_tasks_any.dispatch(new detail::TaskImpl<T>(op));
}
/// Dispatch task to any thread.
template<typename T> void dispatch_any(T&& op)
{
    detail::g_tasks_any.dispatch(new detail::TaskImpl<T>(move(op)));
}

/// Dispatch task to main thread.
template<typename T> void dispatch_main(const T& op)
{
    detail::g_tasks_main.dispatch(new detail::TaskImpl<T>(op));
}
/// Dispatch task to main thread.
template<typename T> void dispatch_main(T&& op)
{
    detail::g_tasks_main.dispatch(new detail::TaskImpl<T>(move(op)));
}

}

#endif
