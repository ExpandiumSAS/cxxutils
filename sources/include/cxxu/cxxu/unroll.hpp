#ifndef __CXXU_UNROLL_H__
#define __CXXU_UNROLL_H__

#include <boost/preprocessor/repetition/repeat_from_to.hpp>

#if defined(__GNUC__) && !defined(__clang__)
    #define CXXU_UNROLL __attribute__((optimize("unroll-loops")))
#else
    #define CXXU_UNROLL
#endif

#define CXXU_UNROLL_FROM_TO(START, END, BODY) \
        BOOST_PP_REPEAT_FROM_TO(START, END, BODY, "")

#endif // __CXXU_UNROLL_H__
