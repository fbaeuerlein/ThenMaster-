#ifndef _THENMASTER_TASKBASE_HXX_
#define _THENMASTER_TASKBASE_HXX_

#include "ITask.hxx"
#include <iostream>
#include <cassert>
#include <functional>
#include <future>
#include <memory>

template<typename Result, typename ... Params>
class TaskTypes
{
public:
    typedef std::function<Result(Params && ...)> function_t;
    typedef std::promise<Result> promise_t;
    typedef std::future<Result> future_t;
    typedef ITask<Result> next_t;
    typedef std::shared_ptr<next_t> p_next_t;         
    typedef std::shared_ptr<promise_t> p_promise_t;        
};

template<typename Result>
class TaskTypes<Result, void>
{
public:
    typedef std::function<Result(void)> function_t;
    typedef std::promise<Result> promise_t;
    typedef std::future<Result> future_t;
    typedef ITask<Result> next_t;
    typedef std::shared_ptr<next_t> p_next_t;         
    typedef std::shared_ptr<promise_t> p_promise_t;        
};

namespace 
{
    template<typename Result, typename ... Params>
    class Executor
    {
        public:
        static void execute(
            typename TaskTypes<Result, Params ...>::function_t const & f, 
            typename TaskTypes<Result, Params ...>::p_promise_t & promise, 
            typename TaskTypes<Result, Params ...>::p_next_t & next, 
            Params && ... p)    
        {
            auto value = f(std::forward<Params>(p) ...);
            promise->set_value(value);
            if ( next )
            {
                next->operator()(std::move(value));
            }
        }  
    };

    template<typename ... Params>
    class Executor<void, Params...>    
    {
        public:
        static void execute(
            typename TaskTypes<void, Params ...>::function_t const & f, 
            typename TaskTypes<void, Params ...>::p_promise_t & promise, 
            typename TaskTypes<void, Params ...>::p_next_t & next, 
            Params && ... p)    
        {
            f(std::forward<Params>(p) ...);
            promise->set_value();
            if ( next )
            {
                next->operator()();
            }
        }       
    };

    // template<
    //     template<typename, typename> typename Task, 
    //     typename Result>
    // class ThenExecutor
    // {
    //     public:

    //     template<typename X>
    //     typename std::shared_ptr<Task<X, void>> & then(std::function<X()> && f)
    //     {
    //         auto next_one = std::make_shared<Task<X, void>>(std::move(f));
    //     }

    //     // overload for void result function
    //     typename Task<void, void> & then(std::function<void()> && f)
    //     {
    //         auto next_one = std::make_shared<Task<void, void>>(std::move(f));
    //         this->m_next = std::static_pointer_cast<next_t>(next_one);
    //         return *next_one.get();
    //     }        



    //     // overload for void result function
    //     Task<void, Result> & then(std::function<void(Result &&)> && f)
    //     {
    //         auto next_one = std::make_shared<Task<void, Result>>(std::move(f));
    //         this->m_next = std::static_pointer_cast<next_t>(next_one);
    //         return *next_one.get();
    //     }        
    // };

    // template<template<typename, typename> typename Task, typename Result, typename Param>
    // class ThenExecutor
    // {
    //     typedef typename Task<Result, Param> task_t;
    //     typedef std::shared_ptr<task_t> p_task_t;
        
    //     p_task_t & then(std::function<Result(Param &&)> && f)
    //     {
    //         return std::make_shared<task_t>(std::move(f));
    //     }    
    // };
}

template<typename Result, typename ... Params>
class Task : public ITask<Params ...>
{
    public:

    typedef typename TaskTypes<Result, Params ...>::function_t function_t;
    typedef typename TaskTypes<Result, Params ...>::next_t next_t;
    typedef typename TaskTypes<Result, Params ...>::promise_t promise_t;
    typedef typename TaskTypes<Result, Params ...>::future_t future_t;
    typedef typename TaskTypes<Result, Params ...>::p_promise_t p_promise_t;
    typedef typename TaskTypes<Result, Params ...>::p_next_t p_next_t;

    Task(function_t && f)
        : m_next(nullptr)
        , m_promise(std::make_shared<promise_t>())
        , m_function(std::move(f)) { assert(m_promise); }

    future_t get_future() const
    {
        return m_promise->get_future();
    }

    void set_exception(std::exception_ptr e) final
    {
        m_promise->set_exception(e);
        if ( m_next ) m_next->set_exception(e);
    } 


    template<typename X>
    typename std::enable_if<!std::is_same<Result, void>::value, Task<X, void>>::type & then(std::function<X(Result &&)> && f)
    {
        auto next_one = std::make_shared<Task<X, Result>>(std::move(f));
        this->m_next = std::static_pointer_cast<next_t>(next_one);
        return *next_one.get();
    }    

    template<typename X>
    typename std::enable_if<std::is_same<Result, void>::value, Task<X, void>>::type & then(std::function<X()> && f)
    {
        auto next_one = std::make_shared<Task<X, Result>>(std::move(f));
        this->m_next = std::static_pointer_cast<next_t>(next_one);
        return *next_one.get();
    }      

    
    Task<void, void> & then(std::function<void()> && f)
    {
        auto next_one = std::make_shared<Task<void, void>>(std::move(f));
        this->m_next = std::static_pointer_cast<next_t>(next_one);
        return *next_one.get();
    }  

    protected:
        p_next_t m_next;         ///< 
        p_promise_t m_promise;    
        function_t m_function;
}; 


#endif // _THENMASTER_TASKBASE_HXX_