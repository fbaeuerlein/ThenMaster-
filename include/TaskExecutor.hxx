#ifndef _THENMASTER_TASK_EXECUTOR_HXX_
#define _THENMASTER_TASK_EXECUTOR_HXX_

#include <functional>
#include <future>

namespace ThenMaster
{

/*
 * Default task executor 
 */ 

template<template<typename ...> class Task, typename ... Params>
class DefaultTaskExecutor
{
    public:
    typedef Task<Params ...> task_t;

    void operator()(task_t & task, Params && ... p) const
    {
        task.operator()(std::forward<Params>(p) ... );
    }
};

template<template<typename ...> class Task>
class DefaultTaskExecutor<Task, void>
{
    public:
    typedef Task<void> task_t;

    void operator()(task_t & task) const 
    {
        task.operator()();
    }
};

/*
 * Default task executor 
 */ 
template<template<typename ...> class Task, typename ... Params>
class AsyncTaskExecutor
{
    typedef Task<Params ...> task_t;
    public:

    void operator()(task_t & task, Params && ... p) const 
    {
        std::thread(&task_t::operator(), &task, std::forward<Params>(p) ...).detach();
    }
};

template<template<typename ...> class Task>
class AsyncTaskExecutor<Task, void>
{
    public:
    typedef Task<void> task_t;

    void operator()(Task<void> & task) const 
    {
        std::thread(&task_t::operator(), &task).detach();
    }
};

}
#endif // _THENMASTER_TASK_EXECUTOR_HXX_