#ifndef _THENMASTER_TASKBASE_HXX_
#define _THENMASTER_TASKBASE_HXX_

#include "ITask.hxx"
#include <cassert>
#include <functional>
#include <future>
#include <memory>
#include "TaskExecutor.hxx"

namespace ThenMaster
{

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
    typedef std::shared_ptr<future_t> p_future_t;
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
    typedef std::shared_ptr<future_t> p_future_t;    
};

template<typename Result, typename ... Params>
class TaskBase : public ITask<Params ...>
{
    public: 

    typedef typename TaskTypes<Result, Params ...>::function_t function_t;
    typedef typename TaskTypes<Result, Params ...>::promise_t promise_t;
    typedef typename TaskTypes<Result, Params ...>::future_t future_t;
    typedef typename TaskTypes<Result, Params ...>::p_future_t p_future_t;
    typedef typename TaskTypes<Result, Params ...>::p_promise_t p_promise_t;
    typedef typename TaskTypes<Result, Params ...>::p_next_t p_next_t;

    TaskBase(function_t && f)
        : m_next(nullptr)
        , m_promise(std::make_shared<promise_t>())
        , m_future(std::make_shared<future_t>(m_promise->get_future()))
        , m_function(std::move(f)) { }

    future_t & future() const { return *m_future; }

    void set_exception(std::exception_ptr e) final
    {
        m_promise->set_exception(e);
        if ( m_next ) m_next->set_exception(e);
    } 

    protected:
        p_next_t m_next;         ///< next task to execute after result is there
        p_promise_t m_promise;   ///< promise for the result
        p_future_t m_future;     ///< future from the promise
        function_t m_function;   ///< function that should be executed
};

template<typename Result, typename ... Params>
class Task : public TaskBase<Result, Params ...>
{
    public:

    typedef typename TaskTypes<Result, Params ...>::function_t function_t;
    typedef typename TaskTypes<Result, Params ...>::next_t next_t;

    Task(function_t && f)
        : TaskBase<Result, Params ...>(std::move(f)) {}

    Task & operator=(Task const &) = delete;
    Task(Task const &) = delete;

    void execute(Params && ... p)
    {
        try
        {
            auto value = this->m_function(std::forward<Params>(p) ...);
            this->m_promise->set_value(value);
            if ( this->m_next )
            {
                this->m_next->operator()(std::move(value));
            }             
        }
        catch(...)
        {
            this->set_exception(std::current_exception());
        }
    }

    template<typename X>
    Task<X, Result> & then(std::function<X(Result &&)> && f)
    {
        auto next_one = std::make_shared<Task<X, Result>>(std::move(f));
        this->m_next = std::static_pointer_cast<next_t>(next_one);
        return *next_one.get();
    }    

}; 

/**
 * @brief Specialization for Params = void
 * 
 * @tparam Result result type of task
 */
template<typename Result>
class Task<Result, void> : public TaskBase<Result, void>
{
    public:

    typedef typename TaskTypes<Result, void>::function_t function_t;
    typedef typename TaskTypes<Result, void>::next_t next_t;

    Task(function_t && f)
        : TaskBase<Result, void>(std::move(f)) {}

    Task & operator=(Task const &) = delete;
    Task(Task const &) = delete;

    void execute()
    {
        try
        {
            auto value = this->m_function();
            this->m_promise->set_value(value);
            if ( this->m_next )
            {
                this->m_next->operator()(std::move(value));
            }   
        }
        catch(...)
        {
            this->set_exception(std::current_exception());
        }
    }

    template<typename X>
    Task<X, Result> & then(std::function<X(Result &&)> && f)
    {
        auto next_one = std::make_shared<Task<X, Result>>(std::move(f));
        this->m_next = std::static_pointer_cast<next_t>(next_one);
        return *next_one.get();
    }    

}; 

/**
 * @brief specialization for Params = void, Result = Void
 * 
 * @tparam Params 
 */
template<>
class Task<void, void> : public TaskBase<void, void>
{
    public:

    typedef typename TaskTypes<void, void>::function_t function_t;
    typedef typename TaskTypes<void, void>::next_t next_t;

    Task(function_t && f)
        : TaskBase<void, void>(std::move(f)) {}

    Task & operator=(Task const &) = delete;
    Task(Task const &) = delete;

    void execute()
    {
        try
        {
            this->m_function();
            this->m_promise->set_value();
            if ( this->m_next )
            {
                this->m_next->operator()();
            }
        }
        catch(...)
        {
            this->set_exception(std::current_exception());
        }
    }

    template<typename X>
    Task<X, void> & then(std::function<X()> && f)
    {
        auto next_one = std::make_shared<Task<X, void>>(std::move(f));
        this->m_next = std::static_pointer_cast<next_t>(next_one);
        return *next_one.get();
    }    
}; 

/**
 * @brief specialization for Result = void
 * 
 * @tparam Params 
 */
template<typename ... Params>
class Task<void, Params ...> : public TaskBase<void, Params ...>
{
    public:

    typedef typename TaskTypes<void, Params ...>::function_t function_t;
    typedef typename TaskTypes<void, Params ...>::next_t next_t;

    Task(function_t && f)
        : TaskBase<void, Params ...>(std::move(f)) {}

    Task & operator=(Task const &) = delete;
    Task(Task const &) = delete;

    void operator()(Params && ... p)
    {
        try
        {
            this->m_function(std::forward<Params>(p) ...);
            this->m_promise->set_value();
            if ( this->m_next )
            {
                this->m_next->operator()();
            }        
        }
        catch(...)
        {
            this->set_exception(std::current_exception());
        }
    }

    template<typename X>
    Task<X, void> & then(std::function<X()> && f)
    {
        auto next_one = std::make_shared<Task<X, void>>(std::move(f));
        this->m_next = std::static_pointer_cast<next_t>(next_one);
        return *next_one.get();
    }    

}; 

}

#endif // _THENMASTER_TASKBASE_HXX_