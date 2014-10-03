#ifndef __CXXU_SORT_DETAILS_HISTOGRAM_H__
#define __CXXU_SORT_DETAILS_HISTOGRAM_H__

#include <stdint.h>

namespace cxxu {
namespace sort {
namespace details {

template <int N, class Radixer>
class histogram
{
public:
    typedef typename Radixer::pod_type pod_type;
    typedef typename Radixer::hist_type hist_type;
    typedef typename Radixer::pos_type pos_type;
    static const int hist_size = Radixer::hist_size;

    static inline
    void make(Radixer& r, hist_type* hist_base, pod_type* source, pos_type pos)
    {
        hist_type* hist = hist_base + N * hist_size;

        hist[r.template get<N>(source[pos])]++;
    }

    static inline
    void sum(hist_type* sums, hist_type* hist_base, pos_type pos)
    {
        hist_type* hist = hist_base + N * hist_size;

        hist_type tsum = hist[pos] + sums[N];
        hist[pos] = sums[N] - 1;
        sums[N] = tsum;
    }

    static inline
    void process(
        Radixer& r,
        pod_type* dest, pod_type* source, std::size_t size,
        hist_type* hist_base
    )
    {
        hist_type* hist = hist_base + N * hist_size;

        pod_type* reader = (N % 2 == 0 ? source : dest);
        pod_type* writer = (N % 2 == 0 ? dest : source);

        pos_type hpos;

        for (pos_type i = 0; i < size; i++) {
            hpos = r.template get<N>(reader[i]);
            writer[++hist[hpos]] = reader[i];
        }
    }

    static inline
    void process_indices(
        Radixer& r,
        std::size_t* idest, std::size_t* isource,
        std::size_t size,
        pod_type* source,
        hist_type* hist_base
    )
    {
        if (N == 0) {
            for (pos_type i = 0; i < size; i++) {
                isource[i] = i;
            }
        }

        hist_type* hist = hist_base + N * hist_size;

        std::size_t* ireader = (N % 2 == 0 ? isource : idest);
        std::size_t* iwriter = (N % 2 == 0 ? idest : isource);

        pos_type hpos;

        for (pos_type i = 0; i < size; i++) {
            hpos = r.template get<N>(source[ireader[i]]);
            iwriter[++hist[hpos]] = ireader[i];
        }
    }

    static inline
    void process_indices(
        Radixer& r,
        std::size_t* idest, std::size_t* isource,
        std::size_t* odest, std::size_t* osource,
        std::size_t size,
        pod_type* source,
        hist_type* hist_base
    )
    {
        hist_type* hist = hist_base + N * hist_size;

        std::size_t* ireader = (N % 2 == 0 ? isource : idest);
        std::size_t* iwriter = (N % 2 == 0 ? idest : isource);
        std::size_t* oreader = (N % 2 == 0 ? osource : odest);
        std::size_t* owriter = (N % 2 == 0 ? odest : osource);

        pos_type hpos;

        if (N == 0) {
            for (pos_type i = 0; i < size; i++) {
                isource[i] = i;
                hpos = r.template get<N>(source[ireader[i]]);
                osource[i] = hist[hpos] + 1;
            }
        }

        for (pos_type i = 0; i < size; i++) {
            hpos = r.template get<N>(source[ireader[i]]);
            iwriter[++hist[hpos]] = ireader[i];
            owriter[hist[hpos]] = oreader[i];
        }
    }
};

template <int N, class Radixer>
class histogram_maker
{
public:
    typedef typename Radixer::pod_type pod_type;
    typedef typename Radixer::hist_type hist_type;
    typedef typename Radixer::pos_type pos_type;

    template <typename ...Args>
    static inline
    void make(Args&&... args)
    {
        histogram_maker<N - 1, Radixer>::make(std::forward<Args>(args)...);
        histogram<N, Radixer>::make(std::forward<Args>(args)...);
    }

    template <typename ...Args>
    static inline
    void sum(Args&&... args)
    {
        histogram_maker<N - 1, Radixer>::sum(std::forward<Args>(args)...);
        histogram<N, Radixer>::sum(std::forward<Args>(args)...);
    }

    template <typename ...Args>
    static inline
    void process(Args&&... args)
    {
        histogram_maker<N - 1, Radixer>::process(std::forward<Args>(args)...);
        histogram<N, Radixer>::process(std::forward<Args>(args)...);
    }

    template <typename ...Args>
    static inline
    void process_indices(Args&&... args)
    {
        histogram_maker<N - 1, Radixer>::process_indices(std::forward<Args>(args)...);
        histogram<N, Radixer>::process_indices(std::forward<Args>(args)...);
    }
};

template <class Radixer>
class histogram_maker<0, Radixer>
{
public:
    typedef typename Radixer::pod_type pod_type;
    typedef typename Radixer::hist_type hist_type;
    typedef typename Radixer::pos_type pos_type;

    template <typename ...Args>
    static inline
    void make(Args&&... args)
    {
        histogram<0, Radixer>::make(std::forward<Args>(args)...);
    }

    template <typename ...Args>
    static inline
    void sum(Args&&... args)
    {
        histogram<0, Radixer>::sum(std::forward<Args>(args)...);
    }

    template <typename ...Args>
    static inline
    void process(Args&&... args)
    {
        histogram<0, Radixer>::process(std::forward<Args>(args)...);
    }

    template <typename ...Args>
    static inline
    void process_indices(Args&&... args)
    {
        histogram<0, Radixer>::process_indices(std::forward<Args>(args)...);
    }
};

} // namespace details
} // namespace sort
} // namespace cxxu

#endif // __CXXU_SORT_DETAILS_HISTOGRAM_H__
