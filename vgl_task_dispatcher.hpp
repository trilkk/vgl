#ifndef VGL_TASK_DISPATCHER_HPP
#define VGL_TASK_DISPATCHER_HPP

#include "vgl_task_queue.hpp"
#include "vgl_thread.hpp"
#include "vgl_vector.hpp"

namespace vgl
{

namespace detail
{

/// Task queue class.
class TaskDispatcher
{
private:
    /// Threads created for this task queue.
    vector<Thread> m_threads;

    /// Queue for tasks (any thread).
    detail::TaskQueue m_tasks_any;

    /// Queue for tasks (main thread).
    detail::TaskQueue m_tasks_main;

    /// Pool of free data structures for fences.
    queue<detail::FenceDataUptr> m_fence_pool;

    /// Guard mutex.
    unique_ptr<Mutex> m_mutex;

    /// Main thread ID.
    SDL_threadID m_main_thread_id = 0;

    /// Target concurrency level.
    unsigned m_concurrency = 0;

    /// Number of threads active currently.
    unsigned m_threads_active = 0;

    /// Flag signifying the task queue is being destroyed.
    bool m_quitting = false;

private:
    /// Deleted copy constructor.
    TaskDispatcher(const TaskDispatcher&) = delete;
    /// Deleted assignment.
    TaskDispatcher& operator=(const TaskDispatcher&) = delete;

public:
    /// Default constructor.
    constexpr explicit TaskDispatcher() = default;

    /// Destructor.
    ~TaskDispatcher()
    {
#if defined(USE_LD) && defined(DEBUG)
        if(!m_mutex)
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("task queue never initialized"));
        }
#endif
        {
            ScopedLock sl(*m_mutex);
            m_quitting = true;
            m_tasks_any.uninitialize();
            m_tasks_main.uninitialize();
        }

        // Threads must be joined before destroying anything else.
        m_threads.clear();
    }

private:
    /// Acquire a new conditional.
    ///
    /// Creates a new one if the cond pool is empty.
    ///
    /// \return Newly created conditional.
    detail::FenceData* acquireFenceData()
    {
        if(m_fence_pool.empty())
        {
            return new detail::FenceData();
        }

        detail::FenceData* ret = m_fence_pool.front().release();
        m_fence_pool.pop();
        return ret;
    }

    /// Internally wait (create a fence) and dispatch.
    ///
    /// \param queue Internal task queue.
    /// \param task Task to push.
    detail::FenceData* internalDispatch(detail::TaskQueue& task_queue, Task* task)
    {
        ScopedLock sl(*m_mutex);
        detail::FenceData* ret = acquireFenceData();
        task->setFenceData(ret);
        task_queue.emplace(task);
        return ret;
    }

    /// Is the calling thread the main thread.
    ///
    /// \return True if yes, false if no.
    bool isMainThread() const
    {
        return (m_main_thread_id == dnload_SDL_ThreadID());
    }

    /// Is this a spawned thread?
    bool isSpawnedThread()
    {
        SDL_threadID current_thread_id = dnload_SDL_ThreadID();
        for(const auto& vv : m_threads)
        {
            if(vv.getId() == current_thread_id)
            {
                return true;
            }
        }
        return false;
    }

    /// Spawn a new thread.
    void spawnThread()
    {
        m_threads.emplace_back(task_thread_func, this);
    }

    /// Thread function.
    void threadFunc()
    {
        ScopedLock sl(*m_mutex);

        while(!m_quitting)
        {
            if((m_threads_active < m_concurrency) && !m_tasks_any.empty())
            {
                ++m_threads_active;

                // Release lock for the duration of executing the task.
                {
                    TaskUptr task = m_tasks_any.acquire();
                    sl.release();
                    task->execute();
                    sl.acquire();
                }

                --m_threads_active;
            }
            else
            {
                m_tasks_any.wait(sl);
            }
        }
    }

public:
    /// Initialize the task queue.
    ///
    /// \param op Number of threads to initialize.
    void initialize(unsigned op)
    {
        m_concurrency = op;
        m_main_thread_id = dnload_SDL_ThreadID();

        m_tasks_any.initialize();
        m_tasks_main.initialize();
        m_mutex.reset(new Mutex());

        while(op)
        {
            spawnThread();
            --op;
        }
    }

    /// Gets a main context task.
    ///
    /// \return Main context task.
    TaskUptr getMainTask()
    {
        ScopedLock sl(*m_mutex);

        while(m_tasks_main.empty())
        {
            m_tasks_main.wait(sl);
        }

        return m_tasks_main.acquire();
    }

    /// Dispatch a task (any thread).
    ///
    /// \param task Newly created task.
    void dispatch_any(Task* task)
    {
        ScopedLock sl(*m_mutex);
        m_tasks_any.emplace(task);
    }

    /// Dispatch a task (main thread).
    ///
    /// \param task Newly created task.
    void dispatch_main(Task* task)
    {
        ScopedLock sl(*m_mutex);
        m_tasks_main.emplace(task);
    }

    /// Dispatch a task and wait for it to complete (any thread).
    ///
    /// \param task Newly created task.
    Fence wait_any(Task* task)
    {
        // Prevent deadlock - main thread cannot wait.
        if(isMainThread())
        {
            task->execute();
            delete task;
            return Fence(nullptr);
        }

        detail::FenceData* data = internalDispatch(m_tasks_any, task);
        return Fence(data);
    }

    /// Dispatch a task and wait for it to complete (main thread).
    ///
    /// \param task Newly created task.
    Fence wait_main(Task* task)
    {
        // Prevent deadlock - main thread cannot wait.
        if(isMainThread())
        {
            task->execute();
            delete task;
            return Fence(nullptr);
        }

        detail::FenceData* data = internalDispatch(m_tasks_main, task);
        return Fence(data);
    }

    /// Wait on a fence internal state.
    ///
    /// \param op Fence data.
    void wait(detail::FenceData* op)
    {
        ScopedLock sl(*m_mutex);

#if defined(USE_LD) && defined(DEBUG)
        if(isMainThread())
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("cannot wait on main thread"));
        }
#endif

        // Fence may have turned inactive before the wait point is reached.
        if(op->isActive())
        {
            bool is_spawned = isSpawnedThread();

            // If waiting would lock the last concurrent thread, spawn a new one.
            if(is_spawned)
            {
                if(m_threads_active >= m_concurrency)
                {
                    spawnThread();
                }
                --m_threads_active;
            }

            m_tasks_any.signal();
            op->wait(sl);

            if(is_spawned)
            {
                ++m_threads_active;
            }
        }

        m_fence_pool.emplace(op);
    }

private:
    /// Task dispatcher thread.
    ///
    /// \param op Pointer to task queue.
    static int task_thread_func(void* op)
    {
        TaskDispatcher* task_queue = static_cast<TaskDispatcher*>(op);
        task_queue->threadFunc();
        return 0;
    }
};

/// Queue for tasks.
TaskDispatcher g_task_dispatcher;

/// Internal destruction of fence.
///
/// \param op Fence.
void internal_fence_destruct(Fence& op)
{
    g_task_dispatcher.wait(op.getFenceData());
}

}

/// Initialize task system.
///
/// \param op Concurrency level.
void tasks_initialize(unsigned op)
{
    detail::g_task_dispatcher.initialize(op);
}

/// Get a main loop task.
///
/// This function blocks if main loop tasks are not available.
///
/// \return New main loop task.
TaskUptr task_get_main()
{
    return detail::g_task_dispatcher.getMainTask();
}

/// Dispatch task (any thread).
///
/// \param op Task-compatible executable predicate.
template<typename T> void task_dispatch_any(const T& op)
{
    detail::g_task_dispatcher.dispatch_any(new detail::TaskImpl<T>(op));
}
/// Dispatch task (any thread).
///
/// \param op Task-compatible executable predicate.
template<typename T> void task_dispatch_any(T&& op)
{
    detail::g_task_dispatcher.dispatch_any(new detail::TaskImpl<T>(move(op)));
}

/// Wait on a task (any thread).
///
/// \param op Task-compatible executable predicate.
/// \return Fence.
template<typename T> Fence task_wait_any(const T& op)
{
    return detail::g_task_dispatcher.wait_any(new detail::TaskImpl<T>(op));
}
/// Wait on a task (any thread).
///
/// \param op Task-compatible executable predicate.
/// \return Fence.
template<typename T> Fence task_wait_any(T&& op)
{
    return detail::g_task_dispatcher.wait_any(new detail::TaskImpl<T>(op));
}

/// Dispatch task to main thread.
template<typename T> void task_dispatch_main(const T& op)
{
    detail::g_task_dispatcher.dispatch_main(new detail::TaskImpl<T>(op));
}
/// Dispatch task to main thread.
template<typename T> void task_dispatch_main(T&& op)
{
    detail::g_task_dispatcher.dispatch_main(new detail::TaskImpl<T>(move(op)));
}

/// Wait on a task (main thread).
///
/// \param op Task-compatible executable predicate.
/// \return Fence.
template<typename T> Fence task_wait_main(const T& op)
{
    return detail::g_task_dispatcher.wait_main(new detail::TaskImpl<T>(op));
}
/// Wait on a task (main thread).
///
/// \param op Task-compatible executable predicate.
/// \return Fence.
template<typename T> Fence task_wait_main(T&& op)
{
    return detail::g_task_dispatcher.wait_main(new detail::TaskImpl<T>(move(op)));
}

}

#endif
