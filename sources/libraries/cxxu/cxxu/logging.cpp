#include <system_error>
#include <mutex>

#include <cxxu/logging.hpp>
#include <cxxu/logging/clog_writer.hpp>
#include <cxxu/logging/syslog_writer.hpp>

namespace cxxu {

log_item::log_item(logging::log_writer_ptr writer_ptr, logging::level level)
: writer_ptr_(writer_ptr),
level_(level)
{}

log_item::log_item(log_item&& other)
: writer_ptr_(std::move(other.writer_ptr_)),
level_(other.level_)
{
    *this << other.str();
    other.rdbuf()->str("");
}

log_item::~log_item()
{ writer()(level_, str()); }

logging::log_writer&
log_item::writer()
{ return *writer_ptr_; }

timed_log_item::timed_log_item(logging::log_writer_ptr writer_ptr)
: log_item(writer_ptr),
start_(std::chrono::system_clock::now())
{}

timed_log_item::timed_log_item(
    logging::log_writer_ptr writer_ptr,
    const std::string& label
)
: timed_log_item(writer_ptr)
{ *this << label; }

timed_log_item::timed_log_item(timed_log_item&& other)
: log_item(std::forward<log_item>(other)),
start_(std::move(other.start_))
{}

timed_log_item::~timed_log_item()
{
    end_ = std::chrono::system_clock::now();
    std::chrono::duration<float> fsecs = end_ - start_;

    char str[20];
    std::snprintf(str, 20, "%.5f", fsecs.count());

    *this << " (" << str << "s)";
}

logger&
logger::get()
{
    static logger l;
    return l;
}

logger::logger()
: writer_ptr_(std::make_shared<logging::clog_writer>())
{}

logger::~logger()
{}

void
logger::daemon(bool val, const std::string& name)
{
    if (val) {
        writer_ptr_.reset(new logging::syslog_writer(name));
    } else {
        writer_ptr_.reset(new logging::clog_writer());
    }
}

log_item
logger::log()
{ return make_item<log_item>(); }

log_item
logger::info()
{ return make_item<log_item>(logging::level::info); }

log_item
logger::warning()
{ return make_item<log_item>(logging::level::warning); }

log_item
logger::error()
{ return make_item<log_item>(logging::level::error); }

log_item
logger::system_error()
{
    std::error_condition econd =
        std::system_category().default_error_condition(errno);

    log_item l(writer_ptr_, logging::level::error);
    l << econd.message() << ": ";
    return l;
}

log_item
logger::die()
{ return make_item<log_item>(logging::level::die); }

timed_log_item
logger::timed()
{ return make_item<timed_log_item>(); }

timed_log_item
logger::timed(const std::string& label)
{ return make_item<timed_log_item>(label); }

} // namespace cxxu
