#ifndef GRAPHSTORE_GRAPH_UTIL_HPP
#define GRAPHSTORE_GRAPH_UTIL_HPP

#include <cstdint>
#include <vector>
#include <optional>
#include <unordered_set>
#include <unordered_map>
#include <string>

namespace graph_util {

    using Label = std::string;
    using VertexSet = std::unordered_set<std::uint64_t>;
    using VertexVector = std::vector<std::uint64_t>;

    ///
    /// @brief Directed Graph edge
    ///
    struct Edge {
        /// The origin of the edge
        std::uint64_t source_vertex;
        /// The destination of the edge
        std::uint64_t destination_vertex;
    };

    ///
    /// @brief Path in the graph, stores it's length and the vertices of the path.
    ///
    struct Path {
        /// The length of the path
        std::uint64_t length;
        /// The vertics of the path. The first element of the vertices vector is the starting point of the path.
        VertexVector vertices;

        /// Operator ==, needed to compare the paths.
        bool operator==(const Path &other) const {
            return length == other.length && vertices == other.vertices;
        }
    };

    ///
    /// @brief LabelledGraph is a directed unweighted graph where each vertex has set of string labels associated to it.
    ///
    struct LabelledGraph {
        /// Adjacency list structure to store the edges. i-th element of neighbours vector is the adjacency list for vertex i.
        std::vector<VertexVector> neighbours;
        /// The hash map from a label to the hash set of vertices that have this label set
        std::unordered_map<Label, VertexSet> label_to_vertices;
    };

} // namespace graph_util

#endif //GRAPHSTORE_GRAPH_UTIL_HPP
