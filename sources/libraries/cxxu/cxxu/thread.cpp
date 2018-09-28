#include <cxxu/thread.hpp>

#ifdef LINUX
#  include <pthread.h>
#endif

#include <string.h>

namespace cxxu
{

void thread_set_name(const char* name) {
#ifdef LINUX
    char thread_name[16] = {0};
    strncpy(thread_name, name, sizeof(thread_name) - 1);
    
    pthread_setname_np(pthread_self(), thread_name);
#endif
}

} // namespace cxxu
