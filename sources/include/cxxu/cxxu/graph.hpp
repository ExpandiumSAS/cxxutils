#ifndef __CXXU_GRAPH_H__
#define __CXXU_GRAPH_H__

#include <vector>
#include <string>

#include <timevault/config.h>

#include <cxxu/list_map.hpp>
#include <cxxu/obj_list_map.hpp>

namespace cxxu {

enum class graph_type
{
    no_root,
    with_root
};

class TIMEVAULT_API graph
{
public:
    graph(graph_type type = graph_type::no_root);
    virtual ~graph();

    void add_node(const std::string& name, std::size_t weight = 0);
    void add_edge(const std::string& from, const std::string& to);

    const tv::list_map& resolve();

    std::string to_string();

private:
    struct node {
        std::string name;
        std::size_t dep_count;
        std::size_t weight;
        tv::list_map edges;
    };

    typedef tv::obj_list_map<node> node_list_map;

    void resolve(const node& n);

    graph_type type_;
    node_list_map nodes_;
    tv::list_map resolved_;
    tv::list_map unresolved_;
    tv::list_map ordered_;
    bool modified_;
};

} // namespace cxxu

#endif // __CXXU_GRAPH_H__
