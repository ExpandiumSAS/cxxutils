#pragma once

#include <unordered_map>
#include <functional>
#include <typeinfo>

#include <cxxu/utils.hpp>

#include <boost/any.hpp>

namespace cxxu {

struct any_visitor {
    struct type_info_hash {
        std::size_t operator()(const std::type_info & t) const {
            return t.hash_code();
        }
    };

    struct equal_ref {
        template <typename T>
        bool operator()(std::reference_wrapper<T> a,
                        std::reference_wrapper<T> b) const {
            return a.get() == b.get();
        }
    };

    using type_info_ref = std::reference_wrapper<const std::type_info>;
    using function = std::function<void(boost::any&)>;
    std::unordered_map<type_info_ref, function, type_info_hash, equal_ref> fs;

    template <typename T>
    void insert_visitor(std::function<void(T&)> f) {
        fs.insert(std::make_pair(
            std::ref(typeid(T)),
            function([fct = std::move(f)](boost::any & x) { fct(boost::any_cast<T&>(x)); })));
    }

    bool operator()(boost::any & x) const {
        auto it = fs.find(x.type());
        if (it != fs.end()) {
            it->second(x);
            return true;
        } else {
            std::cout << "Warning: type " << cxxu::demangle_type_name(x.type().name()) << " not registered" << std::endl;
            return false;
        }
    }
};

} // namespace cxxu
