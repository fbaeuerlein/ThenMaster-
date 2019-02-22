#ifndef _THENMASTER_ITASK_HXX_
#define _THENMASTER_ITASK_HXX_

#include <exception>

template<typename ... P>
class ITask
{
    public:
    virtual void operator()(P && ... p) = 0;
    virtual void set_exception(std::exception_ptr) = 0;
};

template<>
class ITask<void>
{
    public:
    virtual void operator()() = 0;
    virtual void set_exception(std::exception_ptr) = 0;
};

#endif // _THENMASTER_ITASK_HXX_