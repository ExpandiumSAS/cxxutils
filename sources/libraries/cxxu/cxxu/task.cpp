#include <cxxu/task.hpp>

namespace cxxu {

task_base::task_base() noexcept
{}

task_base::task_base(void_function&& f) noexcept
: f_(std::move(f))
{}

task_base::task_base(void_function&& f, void_function&& h) noexcept
: f_(std::move(f)),
h_(std::move(h))
{}

task_base::task_base(const task_base& other) noexcept
{ throw std::logic_error("task_base is not copyable"); }

task_base::task_base(task_base&& other) noexcept
: p_(std::move(other.p_)),
f_(std::move(other.f_)),
h_(std::move(other.h_)),
gh_(std::move(other.gh_)),
eptr_(other.eptr_)
{}

task_base::~task_base()
{}

task_base&
task_base::operator=(task_base&& other) noexcept
{
    p_ = std::move(other.p_);
    f_ = std::move(other.f_);
    h_ = std::move(other.h_);
    gh_ = std::move(other.gh_);
    eptr_ = other.eptr_;

    return *this;
}

void
task_base::set_function(void_function&& f)
{ f_ = std::move(f); }

void
task_base::set_handler(void_function&& h)
{ h_ = std::move(h); }

void
task_base::set_generic_handler(void_function&& gh)
{ gh_ = std::move(gh); }

void
task_base::set_value()
{ p_.set_value(); }

task_future
task_base::get_future()
{ return p_.get_future(); }

bool
task_base::has_exception() const
{ return (bool) eptr_; }

std::exception_ptr
task_base::get_exception() const
{ return eptr_; }

void
task_base::operator()()
{
    try {
        f_();
        call_handler();
        set_value();
    } catch (...) {
        set_exception(std::current_exception());
    }

    call_generic_handler();
}

void
task_base::set_exception(std::exception_ptr eptr)
{
    if (!eptr_) {
        eptr_ = eptr;
        p_.set_exception(eptr);
    }
}

void
task_base::call_handler()
{ call_handler(h_); }

void
task_base::call_generic_handler()
{ call_handler(gh_); }

void
task_base::call_handler(void_function& h)
{
    if (h) {
        h();
    }
}

} // namespace cxxu
