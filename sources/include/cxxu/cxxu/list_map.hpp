#ifndef __CXXU_LIST_MAP_H__
#define __CXXU_LIST_MAP_H__

#include <vector>
#include <map>

namespace cxxu {

struct list_map {
    typedef std::vector<std::string> list;
    typedef std::map<std::string, std::size_t> map;
    typedef typename list::iterator iterator;
    typedef typename list::const_iterator const_iterator;

    iterator begin()
    { return l.begin(); }
    iterator end()
    { return l.end(); }
    const_iterator begin() const
    { return l.begin(); }
    const_iterator end() const
    { return l.end(); }

    std::size_t size(void) const
    { return l.size(); }

    bool empty() const
    { return l.empty(); }

    void clear(void)
    { l.clear(); m.clear(); }

    const std::string& operator[](std::size_t pos) const
    { return l[pos]; }

    bool has(const std::string& name) const
    { return m.find(name) != m.end(); }

    void add(const std::string& name) {
        if (has(name)) {
            return;
        }

        l.push_back(name);
        m[name] = l.size() - 1;
    }

    void remove(const std::string& name) {
        if (!has(name)) {
            return;
        }

        std::size_t pos = m[name];

        l.erase(l.begin() + pos);
        m.erase(name);
    }

    list l;
    map m;
};

} // namespace cxxu

#endif // __CXXU_LIST_MAP_H__
