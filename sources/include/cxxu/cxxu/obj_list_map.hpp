#ifndef __CXXU_OBJ_LIST_MAP_H__
#define __CXXU_OBJ_LIST_MAP_H__

#include <iostream>
#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>
#include <map>

#include <cxxu/logging.hpp>
#include <cxxu/obj_list_map_iterator.hpp>

namespace cxxu {

template <typename T>
struct obj_list_map {
    typedef obj_list_map<T> this_type;
    typedef T value_type;

    typedef is_shared_ptr<T> is_shared_ptr_type;
    typedef typename is_shared_ptr<T>::element_type element_type;

    typedef element_type& reference;
    typedef const element_type& const_reference;

    typedef std::vector<T> list;
    typedef std::vector<std::string> name_list;
    typedef std::map<std::string, std::size_t> map;
    typedef obj_list_map_iterator<list> iterator;
    typedef obj_list_map_iterator<list const> const_iterator;

    obj_list_map()
    : kind_("unknown")
    {}

    obj_list_map(const std::string& kind)
    : kind_(kind)
    {}

    obj_list_map(const this_type& other)
    { *this = other; }

    obj_list_map(this_type&& other)
    { *this = std::move(other); }

    virtual ~obj_list_map()
    {}

    this_type& operator=(const this_type& other)
    {
        kind_ = other.kind_;
        label = other.label;
        l = other.l;
        names = other.names;
        m = other.m;

        return *this;
    }

    this_type& operator=(this_type&& other)
    {
        kind_ = std::move(other.kind_);
        label = std::move(other.label);
        l = std::move(other.l);
        names = std::move(other.names);
        m = std::move(other.m);

        return *this;
    }

    iterator begin()
    { return iterator(l.begin()); }
    iterator end()
    { return iterator(l.end()); }
    const_iterator begin() const
    { return const_iterator(l.begin()); }
    const_iterator end() const
    { return const_iterator(l.end()); }
    reference front()
    { return is_shared_ptr_type::deref(l.front()); }
    const_reference front() const
    { return is_shared_ptr_type::deref(l.front()); }
    reference back()
    { return is_shared_ptr_type::deref(l.back()); }
    const_reference back() const
    { return is_shared_ptr_type::deref(l.back()); }

    std::size_t size() const
    { return l.size(); }

    bool empty() const
    { return l.empty(); }

    void clear()
    { l.clear(); names.clear(); m.clear(); }

    const name_list& keys() const
    { return names; }

    const T& ptr(const std::string& name) const
    {
        static_assert(
            is_shared_ptr_type::value,
            "not a shared_ptr obj_list_map"
        );

        if (!has(name)) {
            CXXU_DIE(kind_ << " '" << name << "' does not exist");
        }

        return l[m.find(name)->second];
    }

    reference operator[](std::size_t pos)
    { return is_shared_ptr_type::deref(l[pos]); }

    const_reference operator[](std::size_t pos) const
    { return is_shared_ptr_type::deref(l[pos]); }

    reference operator[](const std::string& name)
    {
        if (!has(name)) {
            CXXU_DIE(kind_ << " '" << name << "' does not exist");
        }

        return is_shared_ptr_type::deref(l[m.find(name)->second]);
    }

    const_reference operator[](const std::string& name) const
    {
        if (!has(name)) {
            CXXU_DIE(kind_ << " '" << name << "' does not exist");
        }

        return is_shared_ptr_type::deref(l[m.find(name)->second]);
    }

    std::size_t pos(const std::string& name) const
    {
        if (!has(name)) {
            CXXU_DIE(kind_ << " '" << name << "' does not exist");
        }

        return m.find(name)->second;
    }

    bool has(const std::string& name) const
    { return m.find(name) != m.end(); }

    void add(const std::string& name, value_type&& t) {
        if (has(name)) {
            CXXU_DIE(kind_ << " '" << name << "' already exists");
        }

        add_if_not_exists(name, std::forward<value_type&&>(t));
    }

    void add(const std::string& name, const value_type& t) {
        if (has(name)) {
            CXXU_DIE(kind_ << " '" << name << "' already exists");
        }

        add_if_not_exists(name, t);
    }

    void add_if_not_exists(const std::string& name, value_type&& t) {
        if (has(name)) {
            return;
        }

        l.push_back(std::forward<value_type&&>(t));
        names.push_back(name);
        m[name] = l.size() - 1;
    }

    void add_if_not_exists(const std::string& name, const value_type& t) {
        if (has(name)) {
            return;
        }

        l.push_back(t);
        names.push_back(name);
        m[name] = l.size() - 1;
    }

    void remove(const std::string& name) {
        if (!has(name)) {
            CXXU_DIE(kind_ << " '" << name << "' does not exist");
        }

        std::size_t pos = m[name];

        l.erase(l.begin() + pos);
        names.erase(names.begin() + pos);
        m.erase(name);

        for (pos = 0; pos < names.size(); pos++) {
            m[names[pos]] = pos;
        }
    }

    void merge(const this_type& other)
    {
        for (const auto& name : other.names) {
            add_if_not_exists(name, other[name]);
        }
    }

    template <typename Pred>
    void sort(Pred p)
    {
        std::vector<std::size_t> indices(l.size());

        std::iota(
            indices.begin(), indices.end(),
            static_cast<size_t>(0)
        );

        std::vector<bool> swapped(l.size(), false);

        std::sort(
            indices.begin(), indices.end(),
            [this,p](std::size_t left, std::size_t right) {
                return p(
                    is_shared_ptr_type::deref(l[left]),
                    is_shared_ptr_type::deref(l[right])
                );
            }
        );

        std::size_t orig = 0;

        for (const auto i : indices) {
            if (i != orig && !swapped[i]) {
                std::swap(l[orig], l[i]);
                std::swap(names[orig], names[i]);
                m[names[orig]] = orig;
                m[names[i]] = i;
                swapped[orig] = true;
                swapped[i] = true;
            }

            orig++;
        }
    }

    std::string
    to_string() const
    {
        std::ostringstream oss;
        oss << "obj_list_map: " << cxxu::join(", ", names);
        return oss.str();
    }

    void dump() const
    {
        std::cout << to_string() << std::endl;
    }

    std::string kind_;
    std::string label;
    list l;
    name_list names;
    map m;
};

} // namespace cxxu

#endif // __CXXU_OBJ_LIST_MAP_H__
