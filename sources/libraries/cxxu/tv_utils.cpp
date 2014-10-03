#include <stdio.h>
#include <tv_utils.hpp>
#include <blosc/blosc.h>

void tv_utils_init()
{
    blosc_init();
    blosc_set_nthreads(1);
}

void tv_utils_fini()
{
    blosc_destroy();
}
