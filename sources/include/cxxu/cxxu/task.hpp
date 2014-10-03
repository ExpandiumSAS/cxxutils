#ifndef __CXXU_TASK_H__
#define __CXXU_TASK_H__

#include <functional>
#include <future>
#include <exception>
#include <stdexcept>
#include <memory>
#include <queue>
#include <type_traits>

#include <timevault/config.h>

namespace cxxu {

class task_runner;

typedef std::promise<void> task_promise;
typedef std::future<void> task_future;
typedef std::function<void()> void_function;

class TIMEVAULT_API task_base {
public:
    task_base() noexcept;
    task_base(void_function&& f) noexcept;
    task_base(void_function&& f, void_function&& h) noexcept;
    task_base(const task_base& other) noexcept;
    task_base(task_base&& other) noexcept;

    virtual ~task_base();

    task_base& operator=(task_base&& other) noexcept;

    void set_function(void_function&& f);
    void set_handler(void_function&& h);
    void set_generic_handler(void_function&& gh);

    void set_value();

    task_future get_future();

    bool has_exception() const;
    std::exception_ptr get_exception() const;

    virtual void operator()();

protected:
    friend class task_runner;

    void set_exception(std::exception_ptr eptr);
    void call_handler();
    void call_generic_handler();

private:
    void call_handler(void_function& h);

    task_promise p_;
    void_function f_;
    void_function h_;
    void_function gh_;
    std::exception_ptr eptr_;
};

typedef std::unique_ptr<task_base> task_ptr;
typedef std::queue<task_ptr> task_ptr_queue;

struct void_result {};
struct non_void_result {};
struct no_handler {};

template <typename H, typename ...Args>
void call_task_handler(H& h, Args&&... args)
{ h(std::forward<Args>(args)...); }

template <typename ...Args>
void call_task_handler(no_handler& h, Args&&... args)
{}

template <
    typename Function = void_function,
    typename Handler = no_handler
>
class task : public task_base
{
public:
    typedef task<Function, Handler> this_type;
    typedef Function function_type;
    typedef Handler handler_type;
    typedef typename std::result_of<Function()>::type result_type;

    typedef typename std::conditional<
        std::is_void<result_type>::value,
        void_result,
        non_void_result
    >::type is_result_void_type;

    task() noexcept
    : task_base()
    {}

    task(function_type&& f) noexcept
    : f_(std::forward<function_type>(f)),
    h_()
    {}

    task(function_type&& f, handler_type&& h) noexcept
    : f_(std::forward<function_type>(f)),
    h_(std::forward<handler_type>(h))
    {}

    task(const this_type& other)
    : task_base(),
    f_(static_cast<function_type&&>(const_cast<function_type&>(other.f_))),
    h_(static_cast<handler_type&&>(const_cast<handler_type&>(other.h_)))
    { throw std::logic_error("task is not copyable"); }

    task(this_type&& other) noexcept
    : task_base(std::forward<task_base>(other)),
    f_(std::move(other.f_)),
    h_(std::move(other.h_))
    {}

    virtual ~task()
    {}

    virtual void operator()()
    {
        try {
            (*this)(is_result_void_);
            set_value();
        } catch (...) {
            set_exception(std::current_exception());
        }

        call_generic_handler();
    }

private:
    void operator()(const non_void_result& t)
    {
        auto res = f_();
        call_task_handler(h_, res);
    }

    void operator()(const void_result& t)
    {
        f_();
        call_task_handler(h_);
    }

    function_type f_;
    handler_type h_;
    is_result_void_type is_result_void_;
};

template <typename ...Args>
task_ptr
new_task(Args&&... args)
{
    return task_ptr(new task<Args...>(std::forward<Args>(args)...));
}

} // namespace cxxu

#endif // __CXXU_TASK_H__
