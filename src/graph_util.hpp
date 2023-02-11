#ifndef GRAPHSTORE_GRAPH_UTIL_HPP
#define GRAPHSTORE_GRAPH_UTIL_HPP

#include <cstdint>
#include <vector>
#include <optional>

namespace graph_util {

    struct Edge {
        std::uint64_t from;
        std::uint64_t to;
    };

    struct Path {
        std::uint64_t length;
        std::vector<int> vertices;
    };

} // graph_util

#endif //GRAPHSTORE_GRAPH_UTIL_HPP
