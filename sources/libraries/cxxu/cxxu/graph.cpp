#include <sstream>

#include <cxxu/graph.hpp>
#include <cxxu/utils.hpp>
#include <cxxu/logging.hpp>

namespace cxxu {

static const std::string graph_root = "____root____";

graph::graph(graph_type type)
: type_(type),
modified_(false)
{
    if (type_ == graph_type::with_root) {
        nodes_.add(graph_root, { graph_root, 0, 0, {} });
    }
}

graph::~graph()
{}

void
graph::add_node(const std::string& name, std::size_t weight)
{
    nodes_.add(name, { name, 0, weight, {} });

    if (type_ == graph_type::with_root) {
        add_edge(graph_root, name);
    }

    modified_ = true;
}

void
graph::add_edge(const std::string& from, const std::string& to)
{
    nodes_[from].edges.add(to);
    nodes_[to].dep_count++;
    modified_ = true;
}

const cxxu::list_map&
graph::resolve()
{
    if (modified_) {
        modified_ = false;

        nodes_.sort(
            [](const node& a, const node& b) {
                return a.dep_count < b.dep_count;
            }
        );

        resolved_.clear();
        unresolved_.clear();
        ordered_.clear();

        if (!nodes_.empty()) {
            resolve(nodes_[0]);
            resolved_.remove(graph_root);

            for (const auto& name : resolved_) {
                const auto& n = nodes_[name];

                if (!n.edges.empty()) {
                    ordered_.add(n.name);

                    for (const auto& e : n.edges) {
                        ordered_.add(e);
                    }
                }
            }
        }
    }

    return ordered_;
}

void
graph::resolve(const node& n)
{
    unresolved_.add(n.name);

    for (const auto& e : n.edges) {
        if (!resolved_.has(e)) {
            if (unresolved_.has(e)) {
                CXXU_DIE("circular dependency: " << n.name << " -> " << e);
            }

            resolve(nodes_[e]);
        }
    }

    resolved_.add(n.name);
    unresolved_.remove(n.name);
}

std::string
graph::to_string()
{
    std::vector<std::string> all;

    resolve();

    for (const auto& name : ordered_) {
        all.push_back(name);
    }

    return cxxu::join(" ", all);
}

} // namespace cxxu
