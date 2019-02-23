#ifndef _THENMASTER_ITASK_HXX_
#define _THENMASTER_ITASK_HXX_

#include <exception>
#include <thread>
#include "TaskExecutor.hxx"

namespace ThenMaster
{

template<typename ... Params>
class ITask
{
    protected:
    virtual void execute(Params && ... p) = 0;
    std::once_flag m_once_flag;

    public:
    virtual void operator()(Params && ... p)
    {
        std::call_once(m_once_flag, &ITask::execute, this, std::forward<Params>(p) ...);
    }

    virtual void set_exception(std::exception_ptr) = 0;
    
    template<class ... p> using task_t = ITask<p ...>;
    
    template<template<template<typename ...> class, typename ...>  class Executor = DefaultTaskExecutor>
    void launch(Params && ... p)
    {
        Executor<task_t, Params ...>()(*this, std::forward<Params>(p) ...);
    }

    template<template<template<typename ...> class, typename ...>  class Executor = DefaultTaskExecutor>
    void launch(Executor<task_t, Params...> const & executor, Params && ... p)
    {
        executor(*this, std::forward<Params>(p) ...);
    }
};

template<>
class ITask<void>
{
    protected:
    virtual void execute() = 0;
    std::once_flag m_once_flag;

    public:
    virtual void operator()()
    {
        std::call_once(m_once_flag, &ITask::execute, this);
    }

    virtual void set_exception(std::exception_ptr) = 0;

    template<class ... p> using task_t = ITask<p ...>; 

    template<template<template<typename ...> class, typename ...>  class Executor = DefaultTaskExecutor>
    void launch()
    {
        Executor<task_t, void>()(*this);
    }

    template<template<template<typename ...> class, typename ... >  class Executor = DefaultTaskExecutor>
    void launch(Executor<task_t, void> const & executor)
    {
        executor(*this);
    }    
};

}

#endif // _THENMASTER_ITASK_HXX_